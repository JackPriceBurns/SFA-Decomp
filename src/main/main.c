/*
 * Manual recovery stub based on claimed split coverage and nearby corridor
 * organization work.
 *
 * This file is intentionally not wired into the build yet.
 *
 * Current EN split:
 * - main/main.c
 * - 0x801FD3A4-0x801FF044
 *
 * Nearby corridor context:
 * - previous split: main/light.c
 * - next split: main/expr.c
 * - this sits in the late tail of the same broad game corridor that now has
 *   light.c, timer.c, and dll/anim.c materialized in-tree
 *
 * Why this stub exists:
 * - main.c is already represented in splits.txt but had no source target.
 * - Materializing the file makes this late-game corridor look like a real
 *   source neighborhood instead of a run of anonymous split ownership.
 */

/*
 * No function names were promoted here yet.
 * Start from the current EN split window and the light.c -> timer.c
 * neighborhood when this file is revisited.
 */
