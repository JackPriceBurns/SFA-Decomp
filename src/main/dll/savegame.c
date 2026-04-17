/*
 * Manual recovery stub based on retail/debug source-corridor evidence.
 *
 * This file is intentionally not wired into the build yet.
 * It exists to turn an already-claimed split into a concrete source-file
 * target that can be filled in during decomp work.
 *
 * Retail/debug evidence:
 * - exact debug interval: curves.c -> camcontrol.c
 * - projected local order:
 *   gameplay.c -> foodbag.c -> savegame.c -> screens.c -> pickup.c
 * - debug-side path: dll/savegame.c
 * - debug-side text: 0x8012F00C-0x8012F358
 *
 * Current EN split context:
 * - claimed split window: 0x800FA86C-0x800FC84C
 * - previous split: main/dll/foodbag.c
 * - next split: main/dll/screens.c
 *
 * Why this stub exists:
 * - The file already has a plausible split owner in current EN.
 * - Keeping the source target in-tree makes the gameplay corridor easier
 *   to organize while the surrounding functions are still unnamed.
 */

/*
 * No function names were promoted here yet.
 * Start from the claimed 0x800FA86C-0x800FC84C window and the
 * curves.c -> camcontrol.c corridor when this file is revisited.
 */
