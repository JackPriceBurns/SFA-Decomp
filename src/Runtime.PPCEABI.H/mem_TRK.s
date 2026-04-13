/* TODO: restore stripped imported address metadata if needed. */

.include "macros.inc"
.file "mem_TRK"

.section .init, "ax"
.balign 4

.fn TRK_memset, global
stwu r1, -0x10(r1)
mflr r0
stw r0, 0x14(r1)
stw r31, 0xc(r1)
mr r31, r3
bl TRK_fill_mem
lwz r0, 0x14(r1)
mr r3, r31
lwz r31, 0xc(r1)
mtlr r0
addi r1, r1, 0x10
blr
.endfn TRK_memset

.fn TRK_memcpy, global
subi r4, r4, 0x1
subi r6, r3, 0x1
addi r5, r5, 0x1
b .L_8000352C
.L_80003524:
lbzu r0, 0x1(r4)
stbu r0, 0x1(r6)
.L_8000352C:
subic. r5, r5, 0x1
bne .L_80003524
blr
.endfn TRK_memcpy
