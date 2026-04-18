#!/usr/bin/env python3
"""Audit gaps between adjacent configure.py object entries against live splits."""

from __future__ import annotations

import argparse
import re
from dataclasses import dataclass
from pathlib import Path


OBJECT_RE = re.compile(r'Object\([^,]+,\s*"([^"]+)"')
SPLIT_HEADER_RE = re.compile(r"^([^:\s][^:]*\.(?:c|cpp|s)):$", re.MULTILINE)
TEXT_SPAN_RE = re.compile(r"start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)")
SYMBOL_RE = re.compile(
    r"^([^\s=]+)\s*=\s*\.text:0x([0-9A-Fa-f]+);.*size:0x([0-9A-Fa-f]+)",
    re.MULTILINE,
)


@dataclass(frozen=True)
class TextSpan:
    path: str
    start: int
    end: int


@dataclass(frozen=True)
class GapFunction:
    name: str
    address: int
    size: int

    @property
    def end(self) -> int:
        return self.address + self.size


def parse_config_order(configure_path: Path) -> list[str]:
    text = configure_path.read_text(encoding="utf-8")
    ordered: list[str] = []
    seen: set[str] = set()
    for match in OBJECT_RE.finditer(text):
        path = match.group(1).replace("\\", "/")
        if path in seen:
            continue
        seen.add(path)
        ordered.append(path)
    return ordered


def parse_text_spans(splits_path: Path) -> dict[str, TextSpan]:
    text = splits_path.read_text(encoding="utf-8")
    spans: dict[str, TextSpan] = {}
    current_path: str | None = None
    for line in text.splitlines():
        if line and not line.startswith((" ", "\t")) and line.endswith(":"):
            current_path = line[:-1]
            continue
        if current_path is None or ".text" not in line:
            continue
        match = TEXT_SPAN_RE.search(line)
        if match:
            spans[current_path] = TextSpan(
                path=current_path,
                start=int(match.group(1), 16),
                end=int(match.group(2), 16),
            )
        current_path = None
    return spans


def parse_gap_functions(symbols_path: Path) -> list[GapFunction]:
    text = symbols_path.read_text(encoding="utf-8")
    functions = [
        GapFunction(
            name=match.group(1),
            address=int(match.group(2), 16),
            size=int(match.group(3), 16),
        )
        for match in SYMBOL_RE.finditer(text)
    ]
    return sorted(functions, key=lambda item: item.address)


def classify_category(path: str) -> str:
    return "sdk" if path.startswith(("dolphin/", "Runtime.PPCEABI.H/")) else "game"


def function_summary(
    functions: list[GapFunction],
    start: int,
    end: int,
    limit: int,
) -> tuple[str, int]:
    hits = [fn for fn in functions if fn.address >= start and fn.end <= end]
    if not hits:
        return "none", 0
    shown = hits[:limit]
    summary = ", ".join(f"{fn.name}@0x{fn.address:08X}" for fn in shown)
    if len(hits) > limit:
        summary += f", ... (+{len(hits) - limit} more)"
    return summary, len(hits)


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Report positive text gaps between adjacent configure.py objects."
    )
    parser.add_argument(
        "--configure",
        type=Path,
        default=Path("configure.py"),
        help="Path to configure.py",
    )
    parser.add_argument(
        "--splits",
        type=Path,
        default=Path("config/GSAE01/splits.txt"),
        help="Path to splits.txt",
    )
    parser.add_argument(
        "--symbols",
        type=Path,
        default=Path("config/GSAE01/symbols.txt"),
        help="Path to symbols.txt",
    )
    parser.add_argument(
        "--category",
        choices=("game", "sdk", "all"),
        default="all",
        help="Filter by adjacent object category.",
    )
    parser.add_argument(
        "--path-contains",
        action="append",
        default=[],
        help="Only include gaps where either neighbor contains this substring. Repeatable.",
    )
    parser.add_argument(
        "--min-gap",
        type=lambda value: int(value, 0),
        default=1,
        help="Minimum positive gap size to report.",
    )
    parser.add_argument(
        "--contains",
        type=lambda value: int(value, 0),
        help="Only include gaps containing this address.",
    )
    parser.add_argument(
        "--limit",
        type=int,
        default=20,
        help="Maximum gaps to print.",
    )
    parser.add_argument(
        "--function-limit",
        type=int,
        default=8,
        help="Maximum symbols to summarize per gap.",
    )
    args = parser.parse_args()

    config_order = parse_config_order(args.configure)
    spans = parse_text_spans(args.splits)
    functions = parse_gap_functions(args.symbols)

    matches: list[tuple[int, TextSpan, TextSpan, str, int]] = []
    for left_path, right_path in zip(config_order, config_order[1:]):
        left = spans.get(left_path)
        right = spans.get(right_path)
        if left is None or right is None:
            continue
        if classify_category(left.path) != classify_category(right.path):
            if args.category != "all":
                continue
        elif args.category != "all" and classify_category(left.path) != args.category:
            continue

        if args.path_contains and not any(
            token in left.path or token in right.path for token in args.path_contains
        ):
            continue

        gap_start = left.end
        gap_end = right.start
        gap_size = gap_end - gap_start
        if gap_size < args.min_gap:
            continue
        if args.contains is not None and not (gap_start <= args.contains < gap_end):
            continue

        summary, count = function_summary(functions, gap_start, gap_end, args.function_limit)
        matches.append((gap_size, left, right, summary, count))

    matches.sort(key=lambda item: (-item[0], item[1].end, item[2].start))

    print("# Configure-order gap audit")
    print(f"- configure: `{args.configure.as_posix()}`")
    print(f"- splits: `{args.splits.as_posix()}`")
    print(f"- symbols: `{args.symbols.as_posix()}`")
    print(f"- category: `{args.category}`")
    print(f"- min gap: `0x{args.min_gap:X}`")
    if args.contains is not None:
        print(f"- contains: `0x{args.contains:08X}`")
    if args.path_contains:
        print("- path filters: " + ", ".join(f"`{token}`" for token in args.path_contains))
    print(f"- matches: `{min(args.limit, len(matches))}` / `{len(matches)}`")

    for gap_size, left, right, summary, count in matches[: args.limit]:
        category = f"{classify_category(left.path)}->{classify_category(right.path)}"
        print(
            f"- `0x{left.end:08X}-0x{right.start:08X}` gap=`0x{gap_size:X}` "
            f"`{category}` `{left.path}` -> `{right.path}`"
        )
        print(f"  functions: `{count}`")
        print(f"  summary: {summary}")


if __name__ == "__main__":
    main()
