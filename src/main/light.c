/*
 * Manual recovery stub based on claimed split coverage and nearby debug-side
 * corridor evidence.
 *
 * This file is intentionally not wired into the build yet.
 *
 * Current EN split:
 * - main/light.c
 * - 0x801FBA6C-0x801FD3A4
 *
 * Nearby corridor context:
 * - dark game gap before this file: boulder.c -> light.c
 * - projected debug-side order in that gap includes tFrameAnimator.c,
 *   alphaanim.c, groundAnimator.c, crackanim.c, then light.c
 *
 * Why this stub exists:
 * - light.c is already represented in splits.txt but had no source target.
 * - Materializing the file makes the late-game gap easier to organize even
 *   before the preceding mystery corridor is fully resolved.
 */

/*
 * No function names were promoted here yet.
 * Start from the current EN split window and the boulder.c -> light.c
 * interval projection when this file is revisited.
 */
