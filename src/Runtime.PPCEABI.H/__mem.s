/* TODO: restore stripped imported address metadata if needed. */

.include "macros.inc"
.file "__mem"

.section .init, "ax"
.balign 4

.fn memset, global
stwu r1, -0x10(r1)
mflr r0
stw r0, 0x14(r1)
stw r31, 0xc(r1)
mr r31, r3
bl __fill_mem
lwz r0, 0x14(r1)
mr r3, r31
lwz r31, 0xc(r1)
mtlr r0
addi r1, r1, 0x10
blr
.endfn memset

.fn __fill_mem, global
cmplwi r5, 0x20
clrlwi r4, r4, 24
subi r6, r3, 0x1
mr r7, r4
blt .L_8000347C
nor r0, r6, r6
clrlwi. r3, r0, 30
beq .L_80003408
subf r5, r3, r5
.L_800033FC:
subic. r3, r3, 0x1
stbu r7, 0x1(r6)
bne .L_800033FC
.L_80003408:
cmplwi r7, 0x0
beq .L_80003428
slwi r3, r7, 24
slwi r0, r7, 16
slwi r4, r7, 8
or r0, r3, r0
or r0, r4, r0
or r7, r7, r0
.L_80003428:
srwi. r3, r5, 5
subi r4, r6, 0x3
beq .L_8000345C
.L_80003434:
stw r7, 0x4(r4)
subic. r3, r3, 0x1
stw r7, 0x8(r4)
stw r7, 0xc(r4)
stw r7, 0x10(r4)
stw r7, 0x14(r4)
stw r7, 0x18(r4)
stw r7, 0x1c(r4)
stwu r7, 0x20(r4)
bne .L_80003434
.L_8000345C:
extrwi. r3, r5, 3, 27
beq .L_80003470
.L_80003464:
subic. r3, r3, 0x1
stwu r7, 0x4(r4)
bne .L_80003464
.L_80003470:
li r0, 0x3
addi r6, r4, 0x3
and r5, r5, r0
.L_8000347C:
cmplwi r5, 0x0
beqlr
.L_80003484:
subic. r5, r5, 0x1
stbu r7, 0x1(r6)
bne .L_80003484
blr
.endfn __fill_mem

.fn memcpy, global
cmplw r4, r3
blt .L_800034C0
subi r4, r4, 0x1
subi r6, r3, 0x1
addi r5, r5, 0x1
b .L_800034B4
.L_800034AC:
lbzu r0, 0x1(r4)
stbu r0, 0x1(r6)
.L_800034B4:
subic. r5, r5, 0x1
bne .L_800034AC
blr
.L_800034C0:
add r4, r4, r5
add r6, r3, r5
addi r5, r5, 0x1
b .L_800034D8
.L_800034D0:
lbzu r0, -0x1(r4)
stbu r0, -0x1(r6)
.L_800034D8:
subic. r5, r5, 0x1
bne .L_800034D0
blr
.endfn memcpy
