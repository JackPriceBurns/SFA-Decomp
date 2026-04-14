# Retail source gap windows

This pass fills one remaining workflow gap in the retail-backed source-boundary tooling.

`source_gap_packets.py` could already tell us which missing source names likely sit between two anchors. What it could not do was split one large EN gap into estimated per-file windows for those missing names.

That left the worker with a correct neighborhood but no concrete first-pass boundaries inside it.

## Tool

- `python tools/orig/source_gap_windows.py`
  - starts from the committed retail-backed gap packets
  - pulls exact debug split sizes for the uniquely resolved gap names
  - proportionally fits those debug sizes back onto the current EN gap and snaps the boundaries to current EN function edges
  - emits per-file EN window estimates plus ready markdown briefs under [source_gap_window_briefs/README.md](/C:/Projects/SFA-Decomp/docs/orig/source_gap_window_briefs/README.md)
  - intentionally skips broad hint-only corridors by default so the output stays focused on small actionable gaps

## Highest-value findings

### 1. `expgfx.c -> curves.c` now splits into five concrete EN file windows

The strongest new boundary result is the render/effects corridor:

- `dll/modgfx.c` -> `0x8009FF68-0x800C2BA8`
- `dll/modelfx.c` -> `0x800C2BA8-0x800C8284`
- `dll/dim_partfx.c` -> `0x800C8284-0x800D6778`
- `dll/df_partfx.c` -> `0x800D6778-0x800D8FE0`
- `dll/objfsa.c` -> `0x800D8FE0-0x800E556C`

The important part is not that these are final source-truth boundaries. The important part is that the current EN gap no longer has to be attacked as one anonymous `0x45604`-byte block.

### 2. `curves.c -> camcontrol.c` now has a usable first-pass local skeleton

The current EN gap between the two anchors can now be attacked as:

- `dll/gameplay.c` -> `0x800E56A4-0x800FD9E0`
- `dll/pickup.c` -> `0x800FD9E0-0x80102CA0`
- `dll/modanimeflash1.c` -> `0x80102CA0-0x80102CC8`
- `dll/modcloudrunner2.c` -> `0x80102CC8-0x80102D3C`

This result stays explicitly medium-confidence because the exact debug interval between the anchors is much broader than the four uniquely resolved gap names. Even so, it turns the corridor into a practical split-planning sequence instead of a raw address swamp.

### 3. `objhits.c` is now directly bounded between `objanim.c` and `objHitReact.c`

The single-file packet is the cleanest case:

- `main/objhits.c` -> `0x80030780-0x8003549C`

That gives one concrete missing-file window instead of a generic “there is probably `objhits.c` somewhere in here” note.

## Confidence model

The tool keeps its confidence intentionally conservative:

- `high`
  for fully resolved local packets where debug sizes cover the EN gap closely
- `medium`
  for usable local skeletons where the names resolve cleanly but the broader corridor may still contain unnamed neighbors
- `low`
  for exploratory partial fits

The current packets are all `medium`, which matches the evidence quality. They are good enough for first-pass split planning, but not good enough to silently promote into final source-truth.

## Practical use

- summary:
  - `python tools/orig/source_gap_windows.py`
- inspect one gap or one file:
  - `python tools/orig/source_gap_windows.py --search expgfx curves`
  - `python tools/orig/source_gap_windows.py --search gameplay pickup`
  - `python tools/orig/source_gap_windows.py --search objhits`
- spreadsheet dump:
  - `python tools/orig/source_gap_windows.py --format csv`
- machine-readable dump:
  - `python tools/orig/source_gap_windows.py --format json`
- packet briefs:
  - `python tools/orig/source_gap_windows.py --materialize-all`

## Why this matters

This is the missing bridge between “the retail data names the missing files” and “here are the first EN windows to split.”

That makes it directly useful for the current phase of the repo:

- start a real multi-file source skeleton without hand-partitioning the whole gap
- give side agents bounded EN windows for missing files like `modgfx.c` or `objhits.c`
- keep the output grounded in retail-backed anchors and debug split sizes rather than inventing arbitrary file cuts
