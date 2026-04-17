/*
 * Manual recovery stub based on exact debug-side source neighborhood.
 *
 * This file is intentionally not wired into the build yet.
 *
 * Corridor evidence:
 * - exact debug-side neighborhood in the camcontrol -> DIMBoss interval:
 *   tFrameAnimator154.c -> dll_155.c -> dll_144.c -> trigger.c -> dll_16C.c
 * - debug-side path: dll/trigger.c
 *
 * Why this stub exists:
 * - trigger.c is a concrete named bridge target in a stable debug-side
 *   neighborhood immediately after the tFrameAnimator154 packet.
 * - Materializing it keeps that local ownership clue visible until a safe
 *   split claim or better interval projection is justified.
 */
