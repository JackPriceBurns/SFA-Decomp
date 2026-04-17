/*
 * Manual recovery stub based on exact debug-side source neighborhood.
 *
 * This file is intentionally not wired into the build yet.
 *
 * Corridor evidence:
 * - exact debug-side neighborhood in the camcontrol -> DIMBoss interval:
 *   backpack.c -> dll_F5.c -> dll_12B.c -> landedArwing.c -> staffAction.c
 * - debug-side path: dll/dll_12B.c
 *
 * Why this stub exists:
 * - dll_12B.c is a concrete anonymous bridge target in a stable debug-side
 *   neighborhood leading into the landedArwing/staffAction packet.
 * - Materializing it keeps that local ownership clue visible until a safe
 *   split claim or better interval projection is justified.
 */
