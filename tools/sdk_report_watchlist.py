#!/usr/bin/env python3
"""Summarize high-signal SDK candidates from build/GSAE01/report.json."""

from __future__ import annotations

import argparse
import json
from pathlib import Path


def get_argparser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Show exact-report SDK files that are still unlinked, plus top near-miss SDK files."
    )
    parser.add_argument(
        "--report",
        type=Path,
        default=Path("build/GSAE01/report.json"),
        help="Path to the progress report JSON.",
    )
    parser.add_argument(
        "--limit",
        type=int,
        default=20,
        help="Maximum number of near-miss files to print.",
    )
    return parser


def is_sdk(unit: dict) -> bool:
    return "sdk" in unit.get("metadata", {}).get("progress_categories", [])


def format_misses(unit: dict) -> str:
    misses = [
        f"{fn['name']}={fn.get('fuzzy_match_percent', 100.0):.3f}"
        for fn in unit.get("functions", [])
        if fn.get("fuzzy_match_percent", 100.0) != 100.0
    ]
    return ", ".join(misses[:5])


def main() -> int:
    args = get_argparser().parse_args()
    data = json.loads(args.report.read_text())
    sdk_units = [unit for unit in data["units"] if is_sdk(unit)]

    exact_unlinked = []
    near_misses = []

    for unit in sdk_units:
        measures = unit["measures"]
        complete = unit.get("metadata", {}).get("complete", False)
        matched_code = measures.get("matched_code_percent", 0.0)
        matched_funcs = measures.get("matched_functions_percent", 0.0)
        fuzzy = measures.get("fuzzy_match_percent", 0.0)
        entry = (matched_code, matched_funcs, fuzzy, unit)

        if not complete and matched_code == 100.0 and matched_funcs == 100.0:
            exact_unlinked.append(entry)
        elif not complete:
            near_misses.append(entry)

    exact_unlinked.sort(key=lambda row: row[:3], reverse=True)
    near_misses.sort(key=lambda row: row[:3], reverse=True)

    print("Exact-report SDK files still not linked:")
    if exact_unlinked:
        for _, _, _, unit in exact_unlinked:
            print(f"  {unit['name']}")
    else:
        print("  (none)")

    print()
    print(f"Top {min(args.limit, len(near_misses))} SDK near-miss files:")
    for matched_code, matched_funcs, fuzzy, unit in near_misses[: args.limit]:
        miss_summary = format_misses(unit)
        print(
            f"  {unit['name']}: code={matched_code:.2f}% funcs={matched_funcs:.2f}% fuzzy={fuzzy:.3f}"
        )
        if miss_summary:
            print(f"    misses: {miss_summary}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
