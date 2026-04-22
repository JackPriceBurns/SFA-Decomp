#!/usr/bin/env python3
"""Keep all raw Ghidra dumps materialized somewhere in the repo."""

from __future__ import annotations

import argparse
import re
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parent.parent
TOOLS = ROOT / "tools"
AUDIT_MISSING_RE = re.compile(r"raw dumps still lacking any `\.c` home: `(\d+)`")
INSERTED_SPLITS_RE = re.compile(r"Inserted (\d+) new split owner")


def run_step(args: list[str], cwd: Path = ROOT) -> str:
    print(f"$ {' '.join(args)}")
    result = subprocess.run(
        args,
        cwd=cwd,
        text=True,
        capture_output=True,
        check=True,
    )
    if result.stdout:
        print(result.stdout, end="" if result.stdout.endswith("\n") else "\n")
    if result.stderr:
        print(result.stderr, file=sys.stderr, end="" if result.stderr.endswith("\n") else "\n")
    return result.stdout


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Claim gaps and refresh all managed and companion Ghidra homes."
    )
    parser.add_argument(
        "--audit-limit",
        type=int,
        default=12,
        help="Detailed entry limit passed to ghidra_import_audit.py.",
    )
    parser.add_argument(
        "--skip-ninja",
        action="store_true",
        help="Skip the final ninja verification build.",
    )
    parser.add_argument(
        "--refresh-managed",
        action="store_true",
        help="Also rerender every already-managed Ghidra import, not just autos and companion homes.",
    )
    parser.add_argument(
        "--refresh-autos",
        action="store_true",
        help="Force-refresh auto placeholder imports even when no new gap owners were inserted.",
    )
    args = parser.parse_args()

    py = sys.executable

    claim_output = run_step([py, str(TOOLS / "ghidra_claim_gaps.py"), "--write"])
    inserted_match = INSERTED_SPLITS_RE.search(claim_output)
    inserted_split_count = int(inserted_match.group(1)) if inserted_match else 0
    if inserted_split_count or args.refresh_autos:
        run_step(
            [
                py,
                str(TOOLS / "ghidra_bulk_import.py"),
                "--owner-prefix",
                "main/unknown/autos/",
                "--write",
                "--emit-headers",
                "--update-configure-main",
            ]
        )
    if args.refresh_managed:
        run_step(
            [
                py,
                str(TOOLS / "ghidra_bulk_import.py"),
                "--write",
                "--emit-headers",
            ]
        )
    run_step([py, str(TOOLS / "ghidra_unmanaged_homes.py"), "--write"])
    run_step([py, str(TOOLS / "ghidra_init_homes.py"), "--write"])
    audit_output = run_step(
        [
            py,
            str(TOOLS / "ghidra_import_audit.py"),
            "--limit",
            str(args.audit_limit),
        ]
    )

    match = AUDIT_MISSING_RE.search(audit_output)
    if match is None:
        raise SystemExit("Could not parse missing-home count from ghidra_import_audit.py output.")
    missing_home_count = int(match.group(1))
    if missing_home_count != 0:
        raise SystemExit(f"Ghidra sync incomplete: {missing_home_count} raw dumps still lack a .c home.")

    if not args.skip_ninja:
        run_step(["ninja"])


if __name__ == "__main__":
    main()
