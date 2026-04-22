from __future__ import annotations

import argparse
import re
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path

import ghidra_bulk_import as importer


DEFAULT_STUB_LIST_PATH = Path("tools/ghidra_force_stub_functions.txt")
DEFAULT_SRC_ROOT = Path("src")
DEFAULT_SPLITS_PATH = Path("config/GSAE01/splits.txt")
DEFAULT_GHIDRA_DIR = Path("resources/ghidra-decomp-4-12-2026")
MAX_SIGNATURE_SCAN_LINES = 12

FILE_RE = re.compile(r"#\s+File:\s+(.+)")
LINE_RE = re.compile(r"#\s+(\d+):")
ERROR_RE = re.compile(r"#\s+Error:")


@dataclass(frozen=True)
class CompileError:
    source: Path
    line: int


def load_stubbed_functions(path: Path) -> set[str]:
    return importer.load_force_stub_functions(path)


def write_stubbed_functions(path: Path, names: set[str]) -> None:
    lines = [
        "# Exact imported Ghidra functions that must fall back to compile-first stubs.",
        "# Keep this list narrow and driven by real compiler failures.",
    ]
    for name in sorted(names):
        lines.append(name)
    path.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")


def run_ninja() -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        ["ninja"],
        cwd=Path.cwd(),
        capture_output=True,
        text=True,
        timeout=30,
    )


def parse_compile_errors(output: str) -> list[CompileError]:
    errors: list[CompileError] = []
    current_file: Path | None = None
    current_line: int | None = None
    for raw_line in output.splitlines():
        file_match = FILE_RE.search(raw_line)
        if file_match is not None:
            current_file = Path(file_match.group(1).strip())
            current_line = None
            continue
        line_match = LINE_RE.search(raw_line)
        if line_match is not None:
            current_line = int(line_match.group(1))
            continue
        if ERROR_RE.search(raw_line) and current_file is not None and current_line is not None:
            errors.append(CompileError(current_file, current_line))
            current_line = None
    return errors


def normalize_owner_from_source(source: Path, src_root: Path) -> str | None:
    source_text = source.as_posix().replace("\\", "/")
    src_text = src_root.as_posix().replace("\\", "/").rstrip("/")
    if not source_text.startswith(src_text + "/"):
        return None
    return source_text[len(src_text) + 1 :]


def definition_line_numbers(source_path: Path, names: list[str]) -> dict[str, int]:
    lines = source_path.read_text(encoding="utf-8", errors="replace").splitlines()
    result: dict[str, int] = {}
    for name in names:
        pattern = re.compile(rf"\b{re.escape(name)}\s*\(")
        for index, line in enumerate(lines):
            if pattern.search(line) is None:
                continue
            snippet_lines = lines[index : index + MAX_SIGNATURE_SCAN_LINES]
            snippet = "\n".join(snippet_lines)
            brace_index = snippet.find("{")
            semi_index = snippet.find(";")
            if brace_index == -1:
                continue
            if semi_index != -1 and semi_index < brace_index:
                continue
            result[name] = index + 1
            break
    return result


def resolve_function_for_error(
    error: CompileError,
    owner_map: dict[str, list[importer.FunctionDump]],
    src_root: Path,
) -> tuple[str, str] | None:
    owner = normalize_owner_from_source(error.source, src_root)
    if owner is None:
        return None
    functions = owner_map.get(owner)
    if not functions:
        return None
    line_numbers = definition_line_numbers(error.source, [function.name for function in functions])
    best_name: str | None = None
    best_line = -1
    for function in functions:
        line = line_numbers.get(function.name)
        if line is None or line > error.line or line < best_line:
            continue
        best_line = line
        best_name = function.name
    if best_name is None:
        return None
    return owner, best_name


def rerender_owners(owners: set[str], stub_list_path: Path) -> None:
    if not owners:
        return
    command = [
        sys.executable,
        "tools/ghidra_bulk_import.py",
        "--write",
        "--emit-headers",
        "--managed-existing-only",
        "--force-stub-functions-file",
        str(stub_list_path),
    ]
    for owner in sorted(owners):
        command.extend(["--owner", owner])
    subprocess.run(command, cwd=Path.cwd(), check=True, timeout=900)


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Iteratively stub only the exact imported Ghidra functions that fail to compile."
    )
    parser.add_argument("--stub-list", type=Path, default=DEFAULT_STUB_LIST_PATH)
    parser.add_argument("--src-root", type=Path, default=DEFAULT_SRC_ROOT)
    parser.add_argument("--splits", type=Path, default=DEFAULT_SPLITS_PATH)
    parser.add_argument("--ghidra-dir", type=Path, default=DEFAULT_GHIDRA_DIR)
    parser.add_argument("--max-rounds", type=int, default=20)
    args = parser.parse_args()

    spans = importer.parse_splits(args.splits)
    owner_map, _ = importer.build_owner_function_map(args.ghidra_dir, spans)
    stubbed_functions = load_stubbed_functions(args.stub_list)

    for round_index in range(1, args.max_rounds + 1):
        build = run_ninja()
        output = build.stdout + build.stderr
        if build.returncode == 0:
            print(f"Build passed after {round_index - 1} stubbing round(s).")
            return

        compile_errors = parse_compile_errors(output)
        if not compile_errors:
            print(output)
            raise SystemExit("Could not parse compiler failures from ninja output.")

        touched_owners: set[str] = set()
        new_stubs: list[tuple[str, str, int]] = []
        unresolved: list[CompileError] = []

        for error in compile_errors:
            resolved = resolve_function_for_error(error, owner_map, args.src_root)
            if resolved is None:
                unresolved.append(error)
                continue
            owner, function_name = resolved
            if function_name in stubbed_functions:
                continue
            stubbed_functions.add(function_name)
            touched_owners.add(owner)
            new_stubs.append((owner, function_name, error.line))

        if not new_stubs:
            print(output)
            if unresolved:
                for error in unresolved:
                    print(f"Unresolved compile error: {error.source}:{error.line}")
            raise SystemExit("No new failing imported functions could be mapped for stubbing.")

        write_stubbed_functions(args.stub_list, stubbed_functions)
        rerender_owners(touched_owners, args.stub_list)

        print(f"Round {round_index}: stubbed {len(new_stubs)} function(s).")
        for owner, function_name, line in new_stubs:
            print(f"  {owner}:{line} -> {function_name}")

    raise SystemExit(f"Hit --max-rounds={args.max_rounds} before the build passed.")


if __name__ == "__main__":
    main()
