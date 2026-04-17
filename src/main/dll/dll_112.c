/*
 * Manual recovery stub based on exact debug-side source neighborhood.
 *
 * This file is intentionally not wired into the build yet.
 *
 * Corridor evidence:
 * - exact debug-side neighborhood in the camcontrol -> DIMBoss interval:
 *   dll_112.c -> dll_117.c -> fireflyLantern.c -> dll_115.c -> dll_EE.c
 * - debug-side path: dll/dll_112.c
 *
 * Why this stub exists:
 * - dll_112.c is a concrete anonymous bridge target in a stable debug-side
 *   neighborhood around fireflyLantern.c.
 * - Materializing it keeps that local ownership clue visible until a safe
 *   split claim or better interval projection is justified.
 */

