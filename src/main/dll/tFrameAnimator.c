/*
 * Manual recovery stub based on projected debug-side source order.
 *
 * This file is intentionally not wired into the build yet.
 *
 * Corridor evidence:
 * - dark game gap: main/dll/boulder.c -> main/light.c
 * - projected debug-side order:
 *   tFrameAnimator.c -> alphaanim.c -> groundAnimator.c -> crackanim.c -> light.c
 * - projected current EN window:
 *   0x801F5184-0x801F55C0
 * - debug-side path: dll/tFrameAnimator.c
 * - debug-side text: 0x801F643C-0x801F6668
 *
 * Why this stub exists:
 * - The corridor still lacks enough direct retail EN evidence for a safe split.
 * - tFrameAnimator.c is a concrete missing source target in the best current
 *   interval projection, so materializing it helps keep the gap organized.
 */

/*
 * No function names were promoted here yet.
 * Start from the projected 0x801F5184-0x801F55C0 window and the
 * boulder.c -> light.c interval projection when this file is revisited.
 */
