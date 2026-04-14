# Retail source boundary worklist

This pass turns the existing `orig/` source-tag tooling into one boundary-planning queue.

The repo already had separate reports for:

- retail source-name recovery
- current EN source islands
- debug-side source-order corridors
- exact debug split size comparisons

What was still missing was one answer to the practical question:

> Which retail-backed file boundary should we recover next, and should we split it now, expand it, shrink it, or treat it as a corridor packet first?

## Tool

- `python tools/orig/source_worklist.py`
  - merges the committed `source_boundaries.py`, `source_corridors.py`, and `source_skeleton.py` signals
  - keeps the retail EN xref span, debug split size, source-order corridor neighbors, and shared-island context together
  - can emit machine-readable JSON or ready-to-work packet briefs under [source_worklist_packets/README.md](/C:/Projects/SFA-Decomp/docs/orig/source_worklist_packets/README.md)
  - classifies each source into one of:
    - `split-now`
    - `expand-window`
    - `shrink-window`
    - `shared-island`
    - `corridor-packet`
    - `seed-only`
    - `no-en-xrefs`

## Highest-leverage findings

### 1. Two retail-backed DLL files are ready for first-pass splits now

- `DIMBoss.c` -> `dll/DIM/DIMboss.c`
  - current EN seed: `0x801BD0E8-0x801BD7F4`
  - best near-fit window: `0x801BD0E8-0x801BDA04`
  - debug target size: `0x900`
  - result: strong `split-now`

- `SHthorntail.c` -> `dll/SH/SHthorntail.c`
  - current EN seed: `0x801D5764-0x801D5AFC`
  - current seed is already the best fit
  - debug target size: `0x3B8`
  - result: strong `split-now`

These are the cleanest retail-backed DLL skeletons in the current repo.

### 2. Three files want expansion before they should be named as final boundaries

- `laser.c`
  - retail label: `Init`
  - current EN seed: `0x802096AC-0x802096D8`
  - suggested expansion: `0x802093B4-0x80209D38`
  - note: shares the tiny retail island with `textblock.c`

- `camcontrol.c`
  - current EN seed: `0x80102D3C-0x80103130`
  - suggested expansion: `0x801024E8-0x80103648`

- `curves.c`
  - current EN seed: `0x800E556C-0x800E56A4`
  - suggested expansion: `0x800E1DA8-0x800E8118`
  - important because the source-order corridor already ties it to `modgfx.c`, `modelfx.c`, `dim_partfx.c`, `df_partfx.c`, `objfsa.c`, then `gameplay.c`

This is the main value of the worklist: it stops these from being treated as vague retail names and turns them into explicit expansion jobs.

### 3. `objanim.c` is still real, but the current EN seed is too wide

- `objanim.c` -> `main/objanim.c`
- retail label: `setBlendMove`
- current EN seed: `0x8002EC4C-0x80030780`
- debug target size: `0x3A8`
- best compact candidate from the current pass: `0x8002EC4C-0x8002F604`

This is not yet a clean split-now file. It is a shrink-first target.

### 4. `textblock.c`, `expgfx.c`, and `objHitReact.c` are better handled as packets first

- `textblock.c`
  - retail label: `Init`
  - shares one tiny EN island with `laser.c`
  - best handled as a shared-island packet before final file boundaries are asserted

- `expgfx.c`
  - strongest retail xref density among the unsized cases
  - no exact debug split in the current bundle
  - best handled as one corridor packet spanning the neighborhood between early render/object files and the `modgfx.c` / `modelfx.c` / `objfsa.c` corridor

- `objHitReact.c`
  - compact retail seed with one direct xref
  - sits between `objhits.c` and the `objlib.c` / `objprint.c` corridor
  - best handled as a corridor packet before a final narrow file boundary is claimed

## Low-signal leftovers

Two names still matter, but do not yet resolve to an EN work window:

- `dvdfs.c`
- `n_attractmode.c`

These stay useful as naming / SDK context, but they are not current split targets.

## Practical use

- summary:
  - `python tools/orig/source_worklist.py`
- inspect one file or action:
  - `python tools/orig/source_worklist.py --search SHthorntail`
  - `python tools/orig/source_worklist.py --search expand-window`
  - `python tools/orig/source_worklist.py --search corridor`
- spreadsheet dump:
  - `python tools/orig/source_worklist.py --format csv`
- machine-readable dump:
  - `python tools/orig/source_worklist.py --format json`
- packet briefs:
  - `python tools/orig/source_worklist.py --materialize-all`
  - writes one markdown packet per visible work item under [source_worklist_packets/README.md](/C:/Projects/SFA-Decomp/docs/orig/source_worklist_packets/README.md)

## Why this matters

The bundled `orig/` leftovers were already good enough to name several files, but the hard part was deciding what kind of recovery job each one actually represented.

This worklist makes that distinction explicit:

- split the near-fit DLL files now
- expand the undersized seeds before naming them
- shrink the oversized seed before materializing it
- keep packet-style cases grouped until the surrounding corridor is understood

The packet export closes the last handoff gap: a worker can open one generated markdown brief and immediately see the recommended EN window, the functions inside it, and the corridor neighbors that bound the next split attempt.
