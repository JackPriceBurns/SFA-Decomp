#!/usr/bin/env python3
"""Summarize whether reference project configs are usable for split inventory and DOL matching."""

from __future__ import annotations

import argparse
from dataclasses import dataclass
from pathlib import Path

from sdk_dol_match import configured_reference_object_path


ROOT = Path(__file__).resolve().parents[1]


@dataclass(frozen=True)
class RefSpec:
    project: str
    config: str

    @property
    def label(self) -> str:
        return f"{self.project}:{self.config}"

    @property
    def root(self) -> Path:
        return ROOT / "reference_projects" / self.project

    @property
    def config_dir(self) -> Path:
        return self.root / "config" / self.config

    @property
    def config_yml(self) -> Path:
        return self.config_dir / "config.yml"

    @property
    def splits_path(self) -> Path:
        return self.config_dir / "splits.txt"

    @property
    def symbols_path(self) -> Path:
        return self.config_dir / "symbols.txt"

    @property
    def configured_dol_rel(self) -> Path | None:
        return configured_reference_object_path(self.config_yml)

    @property
    def configured_dol_abs(self) -> Path | None:
        rel = self.configured_dol_rel
        return None if rel is None else self.root / rel


def parse_refspec(value: str) -> RefSpec:
    if ":" not in value:
        raise argparse.ArgumentTypeError("reference must be in project:config form")
    project, config = value.split(":", 1)
    spec = RefSpec(project=project.strip(), config=config.strip())
    if not (ROOT / "reference_projects" / spec.project).is_dir():
        raise argparse.ArgumentTypeError(f"missing project: {spec.project}")
    return spec


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Check whether reference project configs are ready for inventory and/or signature matching."
    )
    parser.add_argument(
        "--reference",
        type=parse_refspec,
        action="append",
        required=True,
        help="Reference in project:config form. Can be repeated.",
    )
    return parser


def main() -> int:
    args = build_parser().parse_args()
    for spec in args.reference:
        config_dir = spec.config_dir
        config_exists = config_dir.is_dir()
        splits_exists = spec.splits_path.is_file()
        symbols_exists = spec.symbols_path.is_file()
        config_yml_exists = spec.config_yml.is_file()
        dol_abs = spec.configured_dol_abs
        dol_exists = dol_abs is not None and dol_abs.is_file()
        inventory_ready = splits_exists and symbols_exists
        matcher_ready = inventory_ready and dol_exists

        print(spec.label)
        print(f"  config_dir={config_dir if config_exists else 'missing'}")
        print(f"  config_yml={'yes' if config_yml_exists else 'no'}")
        print(f"  splits={'yes' if splits_exists else 'no'} symbols={'yes' if symbols_exists else 'no'}")
        if dol_abs is None:
            print("  configured_dol=unresolved")
        else:
            print(f"  configured_dol={dol_abs}")
            print(f"  dol_present={'yes' if dol_exists else 'no'}")
        print(f"  inventory_ready={'yes' if inventory_ready else 'no'}")
        print(f"  matcher_ready={'yes' if matcher_ready else 'no'}")
        if config_exists and not inventory_ready:
            print("  note=config exists but is missing splits/symbols for current tooling")
        elif inventory_ready and not matcher_ready:
            print("  note=usable for split inventory today; add extracted orig/sys/main.dol for signature matching")
        print()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
