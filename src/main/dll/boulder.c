/*
 * Manual recovery stub based on claimed split coverage and nearby projected
 * dark-gap evidence.
 *
 * This file is intentionally not wired into the build yet.
 *
 * Current EN split:
 * - main/dll/boulder.c
 * - 0x801F4EF0-0x801F5184
 *
 * Nearby corridor context:
 * - this is the left anchor of the dark game gap: boulder.c -> light.c
 * - projected debug-side order in that gap includes tFrameAnimator.c,
 *   alphaanim.c, groundAnimator.c, crackanim.c, then light.c
 *
 * Why this stub exists:
 * - boulder.c is already represented in splits.txt but had no source target.
 * - Materializing the anchor file makes the unresolved late-game animation
 *   corridor easier to organize while the middle files are still only projected.
 */

/*
 * No function names were promoted here yet.
 * Start from the current EN split window and the boulder.c -> light.c
 * interval projection when this file is revisited.
 */
