from __future__ import annotations

import argparse
import re
import shlex
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path

from dolphin_sdk_symbols import load_config_symbols, load_splits


BUILD_LINE_RE = re.compile(
    r"^build build\\(?P<version>[^\\]+)\\src\\base\\PPCArch\.o: \S+ "
    r"src\\base\\PPCArch\.c(?:\s+\|.*)?$"
)
NAME_RE = re.compile(r"^\s+Name: (?P<name>.*?)(?: \(\d+\))?$")
SIZE_RE = re.compile(r"^\s+Size: (?P<size>0x[0-9A-Fa-f]+|\d+)$")
VALUE_RE = re.compile(r"^\s+Value: (?P<value>0x[0-9A-Fa-f]+|\d+)$")
SECTION_RE = re.compile(r"^\s+Section: (?P<section>.+?) \(")
TYPE_RE = re.compile(r"^\s+Type: (?P<type>.+?) \(")
SYMBOL_START_RE = re.compile(r"^\s*Symbol \{$")
SECTION_START_RE = re.compile(r"^\s*Section \{$")


@dataclass(frozen=True)
class BuildConfig:
    mw_version: str
    cflags: tuple[str, ...]


@dataclass(frozen=True)
class ObjectSection:
    name: str
    size: int


@dataclass(frozen=True)
class ObjectSymbol:
    name: str
    value: int
    size: int
    section: str
    type_name: str


@dataclass(frozen=True)
class AnchorCandidate:
    compiled_symbol: ObjectSymbol
    config_address: int
    predicted_start: int
    size_matches: bool


def parse_build_config(build_ninja: Path, version: str) -> BuildConfig:
    lines = build_ninja.read_text(encoding="utf-8").splitlines()
    for index, line in enumerate(lines):
        match = BUILD_LINE_RE.match(line)
        if match is None or match.group("version") != version:
            continue

        mw_version = ""
        cflags_parts: list[str] = []
        cursor = index + 1
        while cursor < len(lines) and lines[cursor].startswith("  "):
            entry = lines[cursor]
            if entry.startswith("  mw_version = "):
                mw_version = entry.split("=", 1)[1].strip()
            elif entry.startswith("  cflags = "):
                value = entry.split("=", 1)[1].strip()
                cflags_parts.append(value.removesuffix("$").strip())
                cursor += 1
                while cursor < len(lines) and lines[cursor].startswith("      "):
                    continuation = lines[cursor].strip()
                    cflags_parts.append(continuation.removesuffix("$").strip())
                    cursor += 1
                continue
            cursor += 1

        if not mw_version or not cflags_parts:
            break
        cflags = tuple(shlex.split(" ".join(cflags_parts), posix=True))
        return BuildConfig(mw_version=mw_version, cflags=cflags)

    raise SystemExit(f"Unable to locate Dolphin C build flags for {version} in {build_ninja}")


def compile_source(
    source: Path,
    build_config: BuildConfig,
    version: str,
    output_root: Path,
) -> Path:
    output_root.mkdir(parents=True, exist_ok=True)
    compiler = Path("build") / "compilers" / build_config.mw_version / "mwcceppc.exe"
    sjiswrap = Path("build") / "tools" / "sjiswrap.exe"
    if not compiler.is_file():
        raise SystemExit(f"Missing compiler: {compiler}")
    if not sjiswrap.is_file():
        raise SystemExit(f"Missing sjiswrap: {sjiswrap}")

    compile_args = [
        str(sjiswrap),
        str(compiler),
        *build_config.cflags,
        "-MMD",
        "-c",
        str(source).replace("/", "\\"),
        "-o",
        str(output_root).replace("/", "\\"),
    ]
    subprocess.run(compile_args, check=True)

    object_path = output_root / f"{source.stem}.o"
    if not object_path.is_file():
        raise SystemExit(f"Expected compiled object not found: {object_path}")
    return object_path


def parse_llvm_readobj(object_path: Path) -> tuple[list[ObjectSection], list[ObjectSymbol]]:
    llvm_readobj = shutil.which("llvm-readobj")
    if llvm_readobj is None:
        llvm_readobj = shutil.which("llvm-readobj.exe")
    if llvm_readobj is None:
        raise SystemExit("Missing llvm-readobj in PATH")

    result = subprocess.run(
        [llvm_readobj, "--sections", "--symbols", str(object_path)],
        check=True,
        capture_output=True,
        text=True,
    )
    lines = result.stdout.splitlines()

    sections: list[ObjectSection] = []
    symbols: list[ObjectSymbol] = []
    index = 0
    while index < len(lines):
        line = lines[index]
        if SECTION_START_RE.match(line):
            name = ""
            size = 0
            index += 1
            while index < len(lines) and lines[index].strip() != "}":
                name_match = NAME_RE.match(lines[index])
                size_match = SIZE_RE.match(lines[index])
                if name_match:
                    name = name_match.group("name")
                elif size_match:
                    size = int(size_match.group("size"), 0)
                index += 1
            if name:
                sections.append(ObjectSection(name=name, size=size))
        elif SYMBOL_START_RE.match(line):
            name = ""
            value = 0
            size = 0
            section = ""
            type_name = ""
            index += 1
            while index < len(lines) and lines[index].strip() != "}":
                name_match = NAME_RE.match(lines[index])
                value_match = VALUE_RE.match(lines[index])
                size_match = SIZE_RE.match(lines[index])
                section_match = SECTION_RE.match(lines[index])
                type_match = TYPE_RE.match(lines[index])
                if name_match:
                    name = name_match.group("name")
                elif value_match:
                    value = int(value_match.group("value"), 0)
                elif size_match:
                    size = int(size_match.group("size"), 0)
                elif section_match:
                    section = section_match.group("section")
                elif type_match:
                    type_name = type_match.group("type")
                index += 1
            if name:
                symbols.append(
                    ObjectSymbol(
                        name=name,
                        value=value,
                        size=size,
                        section=section,
                        type_name=type_name,
                    )
                )
        index += 1

    return sections, symbols


def find_anchor_candidates(
    compiled_symbols: list[ObjectSymbol],
    config_symbols_path: Path,
) -> list[AnchorCandidate]:
    config_symbols = load_config_symbols(config_symbols_path)
    by_name: dict[str, list] = {}
    for symbol in config_symbols:
        if symbol.section != ".text":
            continue
        by_name.setdefault(symbol.name, []).append(symbol)

    candidates: list[AnchorCandidate] = []
    for symbol in compiled_symbols:
        if symbol.type_name != "Function" or symbol.section != ".text":
            continue
        matches = by_name.get(symbol.name)
        if not matches or len(matches) != 1:
            continue
        config_symbol = matches[0]
        candidates.append(
            AnchorCandidate(
                compiled_symbol=symbol,
                config_address=config_symbol.address,
                predicted_start=config_symbol.address - symbol.value,
                size_matches=config_symbol.size == symbol.size,
            )
        )
    return candidates


def describe_overlap(version: str, start: int, end: int) -> list[str]:
    splits_path = Path("config") / version / "splits.txt"
    overlaps: list[str] = []
    for split in load_splits(splits_path):
        if split.section != ".text":
            continue
        if split.end <= start or split.start >= end:
            continue
        overlaps.append(f"{split.path}@0x{split.start:08X}-0x{split.end:08X}")
    return overlaps


def print_report(version: str, source: Path, sections: list[ObjectSection], symbols: list[ObjectSymbol]) -> None:
    text_size = next((section.size for section in sections if section.name == ".text"), 0)
    print(f"# {source.as_posix()}")
    print("sections:")
    for section in sections:
        if section.name.startswith(".rela") or section.name in {".symtab", ".strtab", ".shstrtab", ".comment"}:
            continue
        print(f"  {section.name:<8} 0x{section.size:X}")

    anchors = find_anchor_candidates(symbols, Path("config") / version / "symbols.txt")
    if not anchors:
        print("anchors: none")
        print()
        return

    start_counts: dict[int, int] = {}
    for anchor in anchors:
        start_counts[anchor.predicted_start] = start_counts.get(anchor.predicted_start, 0) + 1
    best_start = max(start_counts.items(), key=lambda item: (item[1], item[0]))[0]
    best_anchors = [anchor for anchor in anchors if anchor.predicted_start == best_start]
    span_end = best_start + text_size
    overlaps = describe_overlap(version, best_start, span_end)

    exact_count = sum(anchor.size_matches for anchor in best_anchors)
    print(
        "best-text-span: "
        f"0x{best_start:08X}-0x{span_end:08X} size=0x{text_size:X} "
        f"anchors={len(best_anchors)} exact-size={exact_count}"
    )
    if overlaps:
        print("overlaps:")
        for overlap in overlaps:
            print(f"  {overlap}")
    else:
        print("overlaps: none")

    print("anchor details:")
    for anchor in sorted(best_anchors, key=lambda item: item.compiled_symbol.value):
        symbol = anchor.compiled_symbol
        exact_text = "yes" if anchor.size_matches else "no"
        print(
            f"  +0x{symbol.value:04X} {symbol.name:<28} "
            f"size=0x{symbol.size:X} addr=0x{anchor.config_address:08X} size-match={exact_text}"
        )
    print()


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Compile dormant SDK imports and compare their emitted object spans against the active config."
    )
    parser.add_argument("sources", nargs="+", help="Source files under src/ to probe")
    parser.add_argument("-v", "--version", default="GSAE01", help="Target version (default: GSAE01)")
    parser.add_argument(
        "--build-ninja",
        default="build.ninja",
        help="Path to build.ninja used to recover Dolphin compile flags",
    )
    parser.add_argument(
        "--output-root",
        default="temp/sdk_import_probe",
        help="Directory used for temporary object output",
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    build_config = parse_build_config(Path(args.build_ninja), args.version)
    output_root = Path(args.output_root)

    for source_arg in args.sources:
        source = Path(source_arg)
        if not source.is_file():
            raise SystemExit(f"Missing source file: {source}")
        object_dir = output_root / source.stem
        object_path = compile_source(source, build_config, args.version, object_dir)
        sections, symbols = parse_llvm_readobj(object_path)
        print_report(args.version, source, sections, symbols)


if __name__ == "__main__":
    sys.exit(main())
