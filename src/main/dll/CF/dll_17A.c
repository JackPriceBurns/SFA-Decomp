/*
 * Manual recovery stub based on exact debug-side source neighborhood.
 *
 * This file is intentionally not wired into the build yet.
 *
 * Corridor evidence:
 * - exact debug-side neighborhood in the camcontrol -> DIMBoss interval:
 *   treasureRelated0177.c -> dll_179.c -> dll_17A.c -> CFlevelControl.c ->
 *   CFTreasSharpy.c
 * - debug-side path: dll/CF/dll_17A.c
 *
 * Why this stub exists:
 * - dll_17A.c is a concrete anonymous CF bridge target in a stable debug-side
 *   neighborhood before CFTreasSharpy.c.
 * - Materializing it keeps that local ownership clue visible until a safe
 *   split claim or better interval projection is justified.
 */

