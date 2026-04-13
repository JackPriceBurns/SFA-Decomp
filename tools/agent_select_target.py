#!/usr/bin/env python3
"""
SFA-Decomp target selection script.
Refreshes objdiff data directly from the current source build and prints viable units.
"""

import argparse
import json
import random
import subprocess
import tempfile
from pathlib import Path

import extract_symbols

# NOTE: MAP-derived addresses/sizes may not match your current build.
WARNING_BUILD_MISMATCH = (
    "WARNING: ADDRESS AND SIZES ARE FOR A DIFFERENT BUILD AND COULD BE WRONG. ALWAYS CHECK GHIDRA."
)

DEFAULT_VERSION = "GSAE01"
DEFAULT_REPORT_NAME = "report.current.json"


def warn_build_mismatch():
    """Print a warning immediately before reporting any address/size (scoped per output block)."""
    print(WARNING_BUILD_MISMATCH)


def load_blacklist():
    """Load recently failed units to avoid."""
    state_file = Path.home() / ".openclaw/workspace/memory/decomp-state.json"
    try:
        with open(state_file) as f:
            state = json.load(f)
        return state.get("recentFailures", [])
    except Exception:
        return []


def calculate_gap(measures):
    """Calculate improvement potential."""
    fuzzy = measures.get("fuzzy_match_percent", 0)
    return 100.0 - fuzzy


def is_viable_target(unit, blacklist):
    """Check if a unit is a good target candidate."""
    name = unit["name"]
    measures = unit.get("measures", {})

    if unit.get("metadata", {}).get("auto_generated", False):
        return False, "auto-generated"

    if name in blacklist:
        return False, "recently failed"

    fuzzy = measures.get("fuzzy_match_percent", 0)
    if fuzzy >= 99.5:
        return False, "already perfect"

    return True, "viable"


def derive_object_file(unit):
    source_path = unit.get("metadata", {}).get("source_path")
    if source_path and source_path != "unknown":
        base = Path(source_path).stem
        return f"{base}.o"
    name = unit.get("name", "")
    base = Path(name).name
    return f"{base}.o"


def derive_source_file(unit):
    source_path = unit.get("metadata", {}).get("source_path")
    if source_path and source_path != "unknown":
        return Path(source_path).name
    name = unit.get("name", "")
    base = Path(name).name
    return f"{base}.cpp"


def summarize_symbols(label, all_info):
    """Return formatted lines for symbol summary (no printing inside)."""
    if not all_info or "error" in all_info:
        err = all_info.get("error") if isinstance(all_info, dict) else "unknown error"
        return [f"  {label}: error: {err}"]

    lines = []
    functions = all_info.get("functions", [])
    globals_data = all_info.get("globals", [])
    lines.append(
        f"  {label}: {len(functions)} funcs, {len(globals_data)} globals (showing up to 5 funcs)"
    )

    for func in functions[:5]:
        parsed = func.get("parsed", {})
        symbol = parsed.get("symbol", "unknown")
        size_raw = parsed.get("size", "unknown")
        addr = parsed.get("virtual_addr", "unknown")

        if size_raw not in ["unknown", "UNUSED"]:
            try:
                size_val = int(size_raw, 16)
                size = f"0x{size_val:x}"
            except ValueError:
                size = size_raw
        else:
            size = size_raw

        lines.append(f"    - {symbol} ({size}b at {addr})")

    return lines


def function_match(func):
    """Return a function's fuzzy score when available."""
    measures = func.get("measures", {})
    if "fuzzy_match_percent" in measures:
        return measures["fuzzy_match_percent"]
    return func.get("fuzzy_match_percent", 0)


def extract_targets(report_path, max_targets=10):
    """Extract viable targets from an objdiff report."""
    with open(report_path) as f:
        data = json.load(f)

    blacklist = load_blacklist()
    candidates = []

    for unit in data.get("units", []):
        viable, _reason = is_viable_target(unit, blacklist)
        if not viable:
            continue

        measures = unit.get("measures", {})
        functions = unit.get("functions", [])

        entry = {
            "name": unit["name"],
            "fuzzy_match": measures.get("fuzzy_match_percent", 0),
            "gap": calculate_gap(measures),
            "total_functions": measures.get("total_functions", 0),
            "matched_functions": measures.get("matched_functions", 0),
            "func_match_percent": measures.get("matched_functions_percent", 0),
            "total_code": measures.get("total_code", 0),
            "source_path": unit.get("metadata", {}).get("source_path", "unknown"),
            "top_functions": [],
        }

        for func in sorted(functions, key=function_match)[:3]:
            match = function_match(func)
            if match < 99:
                entry["top_functions"].append(
                    {
                        "name": func["name"],
                        "match": match,
                        "size": func.get("size", "unknown"),
                    }
                )

        candidates.append(entry)

    viable_candidates = [c for c in candidates if 0 <= c["fuzzy_match"] <= 90]
    random.shuffle(viable_candidates)
    return viable_candidates[:max_targets]


def generate_report(repo_root, report_path):
    """Generate a fresh objdiff report without requiring a checksum-clean linked DOL."""
    objdiff_path = repo_root / "build" / "tools" / "objdiff-cli.exe"
    if not objdiff_path.exists():
        raise FileNotFoundError(f"{objdiff_path} not found")

    report_path.parent.mkdir(parents=True, exist_ok=True)
    subprocess.run(
        [str(objdiff_path), "report", "generate", "-o", str(report_path)],
        cwd=repo_root,
        check=True,
    )


def parse_args():
    parser = argparse.ArgumentParser(
        description="Select viable SFA decomp targets from current objdiff data."
    )
    parser.add_argument("--list", action="store_true", help="Show more candidates.")
    parser.add_argument(
        "--refresh",
        action="store_true",
        help="Generate a fresh report before selecting targets.",
    )
    parser.add_argument(
        "--version",
        default=DEFAULT_VERSION,
        help=f"Build version to inspect (default: {DEFAULT_VERSION}).",
    )
    parser.add_argument("--report", help="Explicit path to an objdiff report JSON file.")
    return parser.parse_args()


def main():
    args = parse_args()
    repo_root = Path(__file__).resolve().parent.parent
    version = args.version
    default_report = repo_root / "build" / version / "report.json"
    current_report = repo_root / "build" / version / DEFAULT_REPORT_NAME

    if args.report:
        report_path = Path(args.report)
    elif args.refresh:
        report_dir = repo_root / "build" / version
        report_dir.mkdir(parents=True, exist_ok=True)
        report_path = Path(
            tempfile.NamedTemporaryFile(
                prefix="report.current.",
                suffix=".json",
                dir=report_dir,
                delete=False,
            ).name
        )
    elif not default_report.exists():
        report_path = current_report
    else:
        report_path = default_report

    pal_map = repo_root / "orig" / "GSAP01" / "game.MAP"
    en_map = repo_root / "orig" / version / "game.MAP"

    if args.refresh or not report_path.exists():
        try:
            generate_report(repo_root, report_path)
        except Exception as exc:
            print(f"ERROR: failed to generate report at {report_path}: {exc}")
            return 1

    max_targets = 20 if args.list else 10
    candidates = extract_targets(report_path, max_targets=max_targets)

    if not candidates:
        print("No viable targets found.")
        return 1

    print(f"Using report: {report_path}")
    print("RANDOM TARGETS:")
    print("=" * 70)

    for i, candidate in enumerate(candidates, 1):
        unit_info = {"name": candidate["name"], "metadata": {"source_path": candidate["source_path"]}}
        obj_file = derive_object_file(unit_info)
        src_file = derive_source_file(unit_info)

        print(
            f"{i:2}. Unit: {candidate['name']} "
            f"(gap: {candidate['gap']:.1f}%, current: {candidate['fuzzy_match']:.1f}%)"
        )
        print(f"    Source: {candidate['source_path']}")
        print(f"    Object: {obj_file}")
        print(f"    Source file: {src_file}")
        print(
            f"    Functions: {candidate['matched_functions']}/{candidate['total_functions']} "
            f"({candidate['func_match_percent']:.1f}%)"
        )

        if candidate["top_functions"]:
            print("    Targets:")
            warn_build_mismatch()
            for func in candidate["top_functions"]:
                print(
                    f"      - {func['name']} ({func['match']:.1f}% match, {func['size']}b)"
                )

        if pal_map.exists():
            pal_info = extract_symbols.extract_all_for_module(
                pal_map, object_file=obj_file, source_file=src_file
            )
            pal_lines = summarize_symbols("PAL symbols", pal_info)
            if pal_lines and not (len(pal_lines) == 1 and "error:" in pal_lines[0]):
                warn_build_mismatch()
            for line in pal_lines:
                print(line)

        if en_map.exists():
            en_info = extract_symbols.extract_all_for_module(
                en_map, object_file=obj_file, source_file=src_file
            )
            en_lines = summarize_symbols("EN symbols", en_info)
            if en_lines and not (len(en_lines) == 1 and "error:" in en_lines[0]):
                warn_build_mismatch()
            for line in en_lines:
                print(line)

        print()

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
