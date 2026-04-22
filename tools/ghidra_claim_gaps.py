#!/usr/bin/env python3
"""Claim split gaps that still contain raw Ghidra dumps."""

from __future__ import annotations

import argparse
from dataclasses import dataclass
from pathlib import Path

from ghidra_bulk_import import DEFAULT_GHIDRA_DIR, DEFAULT_SPLITS_PATH, load_function_dump, parse_splits


@dataclass(frozen=True)
class GapWindow:
    split_start: int
    split_end: int
    first_function: int
    last_function: int
    previous_owner: str | None
    next_owner: str | None
    function_count: int

    @property
    def owner(self) -> str:
        return f"main/unknown/autos/placeholder_{self.first_function:08X}.c"

    @property
    def materialized_start(self) -> int:
        if self.previous_owner is None:
            return self.first_function
        return self.split_start

    @property
    def materialized_end(self) -> int:
        return self.split_end


def collect_gap_windows(splits_path: Path, ghidra_dir: Path) -> list[GapWindow]:
    spans = sorted(parse_splits(splits_path), key=lambda span: span.start)
    raw_functions = sorted(
        (load_function_dump(path) for path in ghidra_dir.glob("*_*.c")),
        key=lambda function: function.address,
    )
    if not spans or not raw_functions:
        return []

    windows: list[GapWindow] = []

    for index in range(len(spans) - 1):
        current = spans[index]
        nxt = spans[index + 1]
        if current.end >= nxt.start:
            continue
        between = [
            function
            for function in raw_functions
            if current.end <= function.address < nxt.start
        ]
        if not between:
            continue
        windows.append(
            GapWindow(
                split_start=current.end,
                split_end=nxt.start,
                first_function=between[0].address,
                last_function=between[-1].address,
                previous_owner=current.owner,
                next_owner=nxt.owner,
                function_count=len(between),
            )
        )

    return windows


def insert_gap_blocks(splits_path: Path, windows: list[GapWindow]) -> int:
    lines = splits_path.read_text(encoding="utf-8").splitlines(keepends=True)
    existing_paths: set[str] = set()
    existing_starts: set[int] = set()
    block_indices: dict[str, int] = {}
    current_owner: str | None = None

    for index, line in enumerate(lines):
        if line and not line.startswith(("\t", " ")) and line.rstrip().endswith(":"):
            current_owner = line.rstrip()[:-1]
            existing_paths.add(current_owner)
            block_indices[current_owner] = index
            continue
        if current_owner is None:
            continue
        stripped = line.strip()
        if stripped.startswith(".text"):
            for part in stripped.split():
                if part.startswith("start:0x"):
                    existing_starts.add(int(part.removeprefix("start:0x"), 16))
                    break

    insertions: list[tuple[int, str]] = []
    for window in windows:
        if window.owner in existing_paths or window.materialized_start in existing_starts:
            continue
        block = (
            f"{window.owner}:\n"
            f"\t.text       start:0x{window.materialized_start:08X} end:0x{window.materialized_end:08X}\n\n"
        )
        if window.next_owner is not None and window.next_owner in block_indices:
            insert_index = block_indices[window.next_owner]
        else:
            insert_index = len(lines)
        insertions.append((insert_index, block))

    if not insertions:
        return 0

    for insert_index, block in sorted(insertions, key=lambda item: item[0], reverse=True):
        lines.insert(insert_index, block)

    splits_path.write_text("".join(lines), encoding="utf-8", newline="\n")
    return len(insertions)


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Claim split gaps that still contain raw Ghidra dumps."
    )
    parser.add_argument("--ghidra-dir", type=Path, default=DEFAULT_GHIDRA_DIR)
    parser.add_argument("--splits", type=Path, default=DEFAULT_SPLITS_PATH)
    parser.add_argument("--write", action="store_true", help="Update splits.txt in place.")
    args = parser.parse_args()

    windows = collect_gap_windows(args.splits, args.ghidra_dir)
    if not windows:
        print("No raw Ghidra gap windows found.")
        return

    for window in windows:
        print(
            f"{window.owner}: start=0x{window.materialized_start:08X} "
            f"end=0x{window.materialized_end:08X} funcs={window.function_count} "
            f"prev={window.previous_owner or '-'} next={window.next_owner or '-'}"
        )

    if args.write:
        inserted = insert_gap_blocks(args.splits, windows)
        print(f"\nInserted {inserted} new split owner{'s' if inserted != 1 else ''}.")


if __name__ == "__main__":
    main()
