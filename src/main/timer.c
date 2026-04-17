/*
 * Manual recovery stub based on claimed split coverage and nearby debug-side
 * source order.
 *
 * This file is intentionally not wired into the build yet.
 *
 * Current EN split:
 * - main/timer.c
 * - 0x801FF094-0x801FF168
 *
 * Nearby corridor context:
 * - debug-side order after maketex.c: timer.c -> dll/anim.c
 * - this sits in the same broad early-game corridor as maketex.c and expgfx.c
 *
 * Why this stub exists:
 * - timer.c is already represented in splits.txt but had no source target.
 * - Materializing the file keeps the late portion of the early giant packet
 *   visible in src/ while the exact function ownership is still being worked out.
 */

/*
 * No function names were promoted here yet.
 * Start from the current EN split window and the maketex.c neighborhood when
 * this file is revisited.
 */
