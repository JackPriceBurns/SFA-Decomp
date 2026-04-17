/*
 * Manual recovery stub based on claimed split coverage and nearby debug-side
 * source order.
 *
 * This file is intentionally not wired into the build yet.
 *
 * Current EN split:
 * - main/dll/anim.c
 * - 0x801FF168-0x80206444
 *
 * Nearby corridor context:
 * - debug-side order after maketex.c: timer.c -> dll/anim.c
 * - this sits immediately after main/timer.c in the current EN split layout
 *
 * Why this stub exists:
 * - anim.c is already represented in splits.txt but had no source target.
 * - Materializing the file keeps the tail of the early giant packet concrete
 *   in src/ even before internal naming work starts.
 */

/*
 * No function names were promoted here yet.
 * Start from the current EN split window and the maketex.c neighborhood when
 * this file is revisited.
 */
