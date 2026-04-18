#!/usr/bin/env python3
"""Audit assigned SDK asm .fn labels against config/GSAE01/symbols.txt."""

from __future__ import annotations

import argparse
import re
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable


SCRIPT_DIR = Path(__file__).resolve().parent
REPO_ROOT = SCRIPT_DIR.parent
DEFAULT_VERSION = "GSAE01"

FN_RE = re.compile(r"^\.fn\s+([A-Za-z_][A-Za-z0-9_]*),\s+(global|local)$")
ADDR_RE = re.compile(r"^# \.text:0x([0-9A-F]+) \| 0x([0-9A-F]+) \| size: 0x([0-9A-F]+)$")
SYM_RE = re.compile(
    r"^([A-Za-z_][A-Za-z0-9_]*) = \.text:0x([0-9A-F]+); // type:function size:0x([0-9A-F]+)(?: scope:(global|local))?$"
)
LOCAL_SUFFIX_RE = re.compile(r"^(?P<base>[A-Za-z_][A-Za-z0-9_]*)_[0-9A-F]{8}$")


@dataclass(frozen=True)
class AsmFunction:
    name: str
    address: int
    size: int
    scope: str
    asm_path: Path


@dataclass(frozen=True)
class SymbolEntry:
    name: str
    address: int
    size: int
    scope: str | None


@dataclass(frozen=True)
class SplitTextRange:
    path: str
    start: int
    end: int


FUNC_DEF_RE = re.compile(
    r"^(?P<prefix>[A-Za-z_][A-Za-z0-9_\s\*]*?)\s+(?P<name>[A-Za-z_][A-Za-z0-9_]*)\s*\("
)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "paths",
        nargs="*",
        help="Optional asm files or source files to audit. Source files are mapped to build/GSAE01/asm/...",
    )
    parser.add_argument("-v", "--version", default=DEFAULT_VERSION)
    parser.add_argument("--only-mismatched", action="store_true")
    return parser.parse_args()


def load_symbols(version: str) -> dict[int, SymbolEntry]:
    symbols_path = REPO_ROOT / "config" / version / "symbols.txt"
    symbols: dict[int, SymbolEntry] = {}
    for line in symbols_path.read_text(encoding="utf-8").splitlines():
        match = SYM_RE.match(line)
        if not match:
            continue
        name, addr_text, size_text, scope = match.groups()
        entry = SymbolEntry(
            name=name,
            address=int(addr_text, 16),
            size=int(size_text, 16),
            scope=scope,
        )
        symbols[entry.address] = entry
    return symbols


def load_split_text_ranges(version: str) -> dict[str, SplitTextRange]:
    splits_path = REPO_ROOT / "config" / version / "splits.txt"
    ranges: dict[str, SplitTextRange] = {}
    current_path: str | None = None
    for raw_line in splits_path.read_text(encoding="utf-8").splitlines():
        line = raw_line.rstrip()
        if not line:
            continue
        if not raw_line.startswith((" ", "\t")) and line.endswith(":"):
            current_path = line[:-1]
            continue
        if current_path is None:
            continue
        match = re.match(r"^\s*\.text\s+start:0x([0-9A-F]+)\s+end:0x([0-9A-F]+)$", line)
        if not match:
            continue
        start_text, end_text = match.groups()
        ranges[current_path] = SplitTextRange(
            path=current_path,
            start=int(start_text, 16),
            end=int(end_text, 16),
        )
    return ranges


def parse_asm_functions(asm_path: Path) -> list[AsmFunction]:
    functions: list[AsmFunction] = []
    current_addr: int | None = None
    current_size: int | None = None
    lines = asm_path.read_text(encoding="utf-8", errors="ignore").splitlines()
    for line in lines:
        addr_match = ADDR_RE.match(line)
        if addr_match:
            _, addr_text, size_text = addr_match.groups()
            current_addr = int(addr_text, 16)
            current_size = int(size_text, 16)
            continue
        fn_match = FN_RE.match(line)
        if fn_match and current_addr is not None and current_size is not None:
            name, scope = fn_match.groups()
            functions.append(
                AsmFunction(
                    name=name,
                    address=current_addr,
                    size=current_size,
                    scope=scope,
                    asm_path=asm_path,
                )
            )
            current_addr = None
            current_size = None
    return functions


def normalize_asm_name(name: str) -> str:
    match = LOCAL_SUFFIX_RE.match(name)
    if match:
        return match.group("base")
    return name


def normalize_asm_scope(name: str, scope: str) -> str:
    if LOCAL_SUFFIX_RE.match(name):
        return "local"
    return scope


def is_placeholder_name(name: str) -> bool:
    return name.startswith(("fn_", "gap_"))


def source_to_asm_path(version: str, input_path: Path) -> Path:
    resolved = input_path.resolve()
    try:
        rel = resolved.relative_to((REPO_ROOT / "src").resolve())
    except ValueError:
        return input_path
    return REPO_ROOT / "build" / version / "asm" / rel.with_suffix(".s")


def asm_to_split_path(version: str, asm_path: Path) -> str | None:
    try:
        rel = asm_path.resolve().relative_to((REPO_ROOT / "build" / version / "asm").resolve())
    except ValueError:
        return None
    suffix = rel.suffix.lower()
    if suffix not in {".s", ".c"}:
        return None
    source_like = rel.with_suffix(".c").as_posix()
    if source_like.startswith("Runtime.PPCEABI.H/"):
        return f"dolphin/{source_like}"
    return source_like


def asm_to_source_path(version: str, asm_path: Path) -> Path | None:
    try:
        rel = asm_path.resolve().relative_to((REPO_ROOT / "build" / version / "asm").resolve())
    except ValueError:
        return None
    candidate = REPO_ROOT / "src" / rel.with_suffix(".c")
    if candidate.exists():
        return candidate
    return None


def load_source_scopes(source_path: Path) -> dict[str, str]:
    scopes: dict[str, str] = {}
    for raw_line in source_path.read_text(encoding="utf-8", errors="ignore").splitlines():
        line = raw_line.strip()
        match = FUNC_DEF_RE.match(line)
        if not match:
            continue
        name = match.group("name")
        prefix = match.group("prefix")
        prefix_tokens = set(prefix.replace("*", " ").split())
        if prefix_tokens & {"return", "if", "while", "switch", "for", "else", "do", "sizeof"}:
            continue
        scopes[name] = "local" if "static" in prefix.split() else "global"
    return scopes


def iter_asm_paths(version: str, inputs: Iterable[str]) -> list[Path]:
    if not inputs:
        return sorted((REPO_ROOT / "build" / version / "asm" / "dolphin").rglob("*.s"))
    asm_paths: list[Path] = []
    for raw in inputs:
        path = Path(raw)
        if not path.is_absolute():
            path = (REPO_ROOT / path).resolve()
        else:
            path = path.resolve()
        if path.suffix in {".c", ".s"} and "src" in path.parts:
            asm_path = source_to_asm_path(version, path)
        else:
            asm_path = path
        if asm_path.exists():
            asm_paths.append(asm_path)
    return asm_paths


def classify(function: AsmFunction, symbol: SymbolEntry | None) -> tuple[str, str]:
    if is_placeholder_name(function.name):
        return ("placeholder", f"addr=0x{function.address:08X} name={function.name}")
    asm_name = normalize_asm_name(function.name)
    asm_scope = normalize_asm_scope(function.name, function.scope)
    if symbol is None:
        return ("missing", f"addr=0x{function.address:08X} name={asm_name} size=0x{function.size:X}")
    if symbol.name != asm_name:
        return (
            "rename",
            f"addr=0x{function.address:08X} current={symbol.name} -> asm={asm_name}",
        )
    if symbol.size != function.size:
        return (
            "size",
            f"addr=0x{function.address:08X} name={asm_name} current=0x{symbol.size:X} asm=0x{function.size:X}",
        )
    if symbol.scope != asm_scope:
        return (
            "scope",
            f"addr=0x{function.address:08X} name={asm_name} current={symbol.scope} asm={asm_scope}",
        )
    return ("match", f"addr=0x{function.address:08X} name={asm_name}")


def main() -> int:
    args = parse_args()
    symbols = load_symbols(args.version)
    split_ranges = load_split_text_ranges(args.version)
    asm_paths = iter_asm_paths(args.version, args.paths)
    any_output = False
    for asm_path in asm_paths:
        functions = parse_asm_functions(asm_path)
        if not functions:
            continue
        split_path = asm_to_split_path(args.version, asm_path)
        if split_path is None:
            continue
        split_range = split_ranges.get(split_path)
        if split_range is None:
            continue
        start = functions[0].address
        end = max(function.address + function.size for function in functions)
        if start != split_range.start or end != split_range.end:
            continue
        source_path = asm_to_source_path(args.version, asm_path)
        source_scopes = load_source_scopes(source_path) if source_path is not None else {}
        mismatches: list[str] = []
        for function in functions:
            expected_scope = source_scopes.get(normalize_asm_name(function.name))
            if expected_scope is not None:
                function = AsmFunction(
                    name=function.name,
                    address=function.address,
                    size=function.size,
                    scope=expected_scope,
                    asm_path=function.asm_path,
                )
            status, detail = classify(function, symbols.get(function.address))
            if status in {"match", "placeholder"} and args.only_mismatched:
                continue
            mismatches.append(f"  {status:<7} {detail}")
        if args.only_mismatched:
            mismatches = [line for line in mismatches if not line.lstrip().startswith("match")]
        if not mismatches:
            continue
        print(f"{asm_path.relative_to(REPO_ROOT)}")
        for line in mismatches:
            print(line)
        print()
        any_output = True
    return 0 if any_output or not args.only_mismatched else 0


if __name__ == "__main__":
    raise SystemExit(main())
