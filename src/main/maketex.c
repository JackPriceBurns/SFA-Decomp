/*
 * Manual recovery stub based on retail/debug source-corridor evidence.
 *
 * This file is intentionally not wired into the build yet.
 * It exists to turn a globally unique source-packet hint into a concrete
 * source-file target that can be filled in during decomp work.
 *
 * Retail/debug evidence:
 * - source gap packet: objHitReact.c -> expgfx.c
 * - resolved gap name: maketex.c
 * - debug-side path: main/maketex.c
 * - debug-side text: 0x800BD2C4-0x800BEF64
 * - debug-side neighbors: track/intersect.c before, timer.c after
 *
 * Current EN corridor context:
 * - nearby claimed split before this unknown pocket: track/intersect.c
 *   at 0x8006F0B4-0x8007E7A0
 * - nearby SDK island: dolphin/os/OSAddress.c
 *   at 0x80080E28-0x80080E58
 * - nearby claimed split after this unknown pocket: main/expgfx.c
 *   at 0x8009B36C-0x8009FF68
 *
 * Why this stub exists:
 * - maketex.c is a global-unique filename hint in the recovered corridor.
 * - The current EN placement is not tight enough yet to claim a split window.
 * - Keeping the source target in-tree makes the corridor easier to organize
 *   once the surrounding EN window is pinned down.
 */

/*
 * No function names were promoted here yet.
 * Start from the objHitReact.c -> expgfx.c packet and local EN functions
 * around the OSAddress island when this corridor is revisited.
 */
