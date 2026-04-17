/*
 * Manual recovery stub based on exact debug-side source neighborhood.
 *
 * This file is intentionally not wired into the build yet.
 *
 * Corridor evidence:
 * - exact debug-side neighborhood in the camcontrol -> DIMBoss interval:
 *   genprops.c -> gfxEmit.c -> dll_13F.c -> dll_141.c -> dll_138.c ->
 *   transporter.c
 * - debug-side path: dll/dll_13F.c
 *
 * Why this stub exists:
 * - dll_13F.c is a concrete anonymous bridge target in a stable debug-side
 *   neighborhood leading into the transporter packet.
 * - Materializing it keeps that local ownership clue visible until a safe
 *   split claim or better interval projection is justified.
 */
