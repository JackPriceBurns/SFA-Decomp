/*
 * Target bytes at 0x801177DC..0x8011784C were mislabeled "AXInit/AXQuit" by
 * Dolphin signature analysis. They are not the Dolphin SDK AX init/quit
 * helpers (those call __AXAllocInit/__AXVPBInit/__AXSPBInit/... and SFA
 * does not link them). The two functions here are a game-side thread
 * lifecycle pair called from main/dll/FRONT/dll_3E: fn_801177DC cancels
 * the thread at lbl_803A6100 and clears an sbss "running" flag at
 * lbl_803DE2D8; fn_80117818 resumes that thread when the flag is set.
 * Kept as asm to preserve the exact byte image until the frontend
 * subsystem they belong to is decompiled.
 */

extern void OSCancelThread(void* thread);
extern void OSResumeThread(void* thread);

extern int lbl_803DE2D8;
extern char lbl_803A6100[];

asm void fn_801177DC(void) {
    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    lwz r0, lbl_803DE2D8(r0)
    cmpwi r0, 0x0
    beq _f1_0
    lis r3, lbl_803A6100@ha
    addi r3, r3, lbl_803A6100@l
    bl OSCancelThread
    li r0, 0x0
    stw r0, lbl_803DE2D8(r0)
_f1_0:
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
}

asm void fn_80117818(void) {
    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    lwz r0, lbl_803DE2D8(r0)
    cmpwi r0, 0x0
    beq _f2_0
    lis r3, lbl_803A6100@ha
    addi r3, r3, lbl_803A6100@l
    bl OSResumeThread
_f2_0:
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
}
