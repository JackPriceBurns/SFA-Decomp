/*
 * Manual recovery stub based on exact debug-side source neighborhood.
 *
 * This file is intentionally not wired into the build yet.
 *
 * Corridor evidence:
 * - exact debug-side neighborhood in the camcontrol -> DIMBoss interval:
 *   CFguardian.c -> windlift.c -> dll_15E.c -> CFcrystal.c -> CFBaby.c
 * - debug-side path: dll/CF/dll_15E.c
 *
 * Why this stub exists:
 * - dll_15E.c is a concrete CF bridge target in a stable debug-side
 *   neighborhood immediately before CFcrystal.c.
 * - Materializing it keeps that local ownership clue visible until a safe
 *   split claim or better interval projection is justified.
 */

