/* TODO: restore stripped imported address metadata if needed. */

.include "macros.inc"
.file "__start"

.section .init, "ax"
.balign 4

.fn __check_pad3, local
mflr r0
lis r3, 0x8000
stw r0, 0x4(r1)
stwu r1, -0x8(r1)
lhz r0, 0x30e4(r3)
andi. r0, r0, 0xeef
cmpwi r0, 0xeef
bne .L_80003130
li r3, 0x0
li r4, 0x0
li r5, 0x0
bl OSResetSystem
.L_80003130:
lwz r0, 0xc(r1)
addi r1, r1, 0x8
mtlr r0
blr
.endfn __check_pad3

.fn __start, weak
.long 0x48000001
.reloc .-4, R_PPC_REL24, __init_registers
bl __init_hardware
li r0, -0x1
stwu r1, -0x8(r1)
stw r0, 0x4(r1)
stw r0, 0x0(r1)
.long 0x48000001
.reloc .-4, R_PPC_REL24, __init_data
li r0, 0x0
lis r6, 0x8000
addi r6, r6, 0x44
stw r0, 0x0(r6)
lis r6, 0x8000
addi r6, r6, 0xf4
lwz r6, 0x0(r6)
cmplwi r6, 0x0
beq .L_80003188
lwz r7, 0xc(r6)
b .L_800031A8
.L_80003188:
lis r5, 0x8000
addi r5, r5, 0x34
lwz r5, 0x0(r5)
cmplwi r5, 0x0
beq .L_800031D0
lis r7, 0x8000
addi r7, r7, 0x30e8
lwz r7, 0x0(r7)
.L_800031A8:
li r5, 0x0
cmplwi r7, 0x2
beq .L_800031C0
cmplwi r7, 0x3
bne .L_800031D0
li r5, 0x1
.L_800031C0:
lis r6, InitMetroTRK@ha
addi r6, r6, InitMetroTRK@l
mtlr r6
blrl
.L_800031D0:
lis r6, 0x8000
addi r6, r6, 0xf4
lwz r5, 0x0(r6)
cmplwi r5, 0x0
beq+ .L_80003230
lwz r6, 0x8(r5)
cmplwi r6, 0x0
beq+ .L_80003230
add r6, r5, r6
lwz r14, 0x0(r6)
cmplwi r14, 0x0
beq .L_80003230
addi r15, r6, 0x4
mtctr r14
.L_80003208:
addi r6, r6, 0x4
lwz r7, 0x0(r6)
add r7, r7, r5
stw r7, 0x0(r6)
bdnz .L_80003208
lis r5, 0x8000
addi r5, r5, 0x34
clrrwi r7, r15, 5
stw r7, 0x0(r5)
b .L_80003238
.L_80003230:
li r14, 0x0
li r15, 0x0
.L_80003238:
bl DBInit
bl OSInit
lis r4, 0x8000
addi r4, r4, 0x30e6
lhz r3, 0x0(r4)
andi. r5, r3, 0x8000
beq .L_80003260
andi. r3, r3, 0x7fff
cmplwi r3, 0x1
bne .L_80003264
.L_80003260:
.long 0x48000001
.reloc .-4, R_PPC_REL24, __check_pad3
.L_80003264:
bl __init_user
mr r3, r14
mr r4, r15
bl main
b exit
.endfn __start

.fn __init_registers, local
lis r1, _stack_addr@h
ori r1, r1, _stack_addr@l
lis r2, _SDA2_BASE_@h
ori r2, r2, _SDA2_BASE_@l
lis r13, _SDA_BASE_@h
ori r13, r13, _SDA_BASE_@l
blr
.endfn __init_registers

.fn __init_data, local
mflr r0
stw r0, 0x4(r1)
stwu r1, -0x18(r1)
stw r31, 0x14(r1)
stw r30, 0x10(r1)
stw r29, 0xc(r1)
lis r3, _rom_copy_info@ha
addi r0, r3, _rom_copy_info@l
mr r29, r0
b .L_800032BC
.L_800032BC:
b .L_800032C0
.L_800032C0:
lwz r30, 0x8(r29)
cmplwi r30, 0x0
beq .L_80003300
lwz r4, 0x0(r29)
lwz r31, 0x4(r29)
beq .L_800032F8
cmplw r31, r4
beq .L_800032F8
mr r3, r31
mr r5, r30
bl memcpy
mr r3, r31
mr r4, r30
bl __flush_cache
.L_800032F8:
addi r29, r29, 0xc
b .L_800032C0
.L_80003300:
lis r3, _bss_init_info@ha
addi r0, r3, _bss_init_info@l
mr r29, r0
b .L_80003310
.L_80003310:
b .L_80003314
.L_80003314:
lwz r5, 0x4(r29)
cmplwi r5, 0x0
beq .L_80003338
lwz r3, 0x0(r29)
beq .L_80003330
li r4, 0x0
bl memset
.L_80003330:
addi r29, r29, 0x8
b .L_80003314
.L_80003338:
lwz r0, 0x1c(r1)
lwz r31, 0x14(r1)
lwz r30, 0x10(r1)
lwz r29, 0xc(r1)
addi r1, r1, 0x18
mtlr r0
blr
.endfn __init_data

.fn __init_hardware, global
mfmsr r0
ori r0, r0, 0x2000
mtmsr r0
mflr r31
bl __OSPSInit
bl __OSCacheInit
mtlr r31
blr
.endfn __init_hardware

.fn __flush_cache, global
lis r5, 0xffff
ori r5, r5, 0xfff1
and r5, r5, r3
subf r3, r5, r3
add r4, r4, r3
.L_80003388:
dcbst r0, r5
sync
icbi r0, r5
addic r5, r5, 0x8
subic. r4, r4, 0x8
bge .L_80003388
isync
blr
.endfn __flush_cache
