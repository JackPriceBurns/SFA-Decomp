/*
 * Manual recovery stub based on claimed split coverage and nearby corridor
 * organization work.
 *
 * This file is intentionally not wired into the build yet.
 *
 * Current EN split:
 * - main/expr.c
 * - 0x801FF044-0x801FF094
 *
 * Nearby corridor context:
 * - previous split: main/main.c
 * - next split: main/timer.c
 * - this sits in the late tail of the same broad game corridor that now has
 *   light.c, timer.c, and dll/anim.c materialized in-tree
 *
 * Why this stub exists:
 * - expr.c is already represented in splits.txt but had no source target.
 * - Materializing the file keeps the late-game corridor organized while the
 *   exact function ownership is still being recovered.
 */

/*
 * No function names were promoted here yet.
 * Start from the current EN split window and the main.c -> timer.c
 * neighborhood when this file is revisited.
 */
