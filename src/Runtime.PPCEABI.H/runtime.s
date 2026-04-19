.include "macros.inc"
.file "runtime"

.text
.balign 4

.fn __cvt_fp2unsigned, global
    stwu r1, -0x10(r1)
    lis r4, __constants@h
    ori r4, r4, __constants@l
    li r3, 0
    lfd f0, 0(r4)
    lfd f3, 8(r4)
    lfd f4, 0x10(r4)
    fcmpu cr0, f1, f0
    fcmpu cr6, f1, f3
    blt .L_8028676C
    subi r3, r3, 1
    bge cr6, .L_8028676C
    fcmpu cr7, f1, f4
    fmr f2, f1
    blt cr7, .L_80286758
    fsub f2, f1, f4
.L_80286758:
    fctiwz f2, f2
    stfd f2, 8(r1)
    lwz r3, 0xc(r1)
    blt cr7, .L_8028676C
    addis r3, r3, 0x8000
.L_8028676C:
    addi r1, r1, 0x10
    blr
.endfn __cvt_fp2unsigned

.fn __save_fpr, global
.sym _savefpr_14, global
    stfd f14, -0x90(r11)
.sym _savefpr_15, global
    stfd f15, -0x88(r11)
.sym _savefpr_16, global
    stfd f16, -0x80(r11)
.sym _savefpr_17, global
    stfd f17, -0x78(r11)
.sym _savefpr_18, global
    stfd f18, -0x70(r11)
.sym _savefpr_19, global
    stfd f19, -0x68(r11)
.sym _savefpr_20, global
    stfd f20, -0x60(r11)
.sym _savefpr_21, global
    stfd f21, -0x58(r11)
.sym _savefpr_22, global
    stfd f22, -0x50(r11)
.sym _savefpr_23, global
    stfd f23, -0x48(r11)
.sym _savefpr_24, global
    stfd f24, -0x40(r11)
.sym _savefpr_25, global
    stfd f25, -0x38(r11)
.sym _savefpr_26, global
    stfd f26, -0x30(r11)
.sym _savefpr_27, global
    stfd f27, -0x28(r11)
.sym _savefpr_28, global
    stfd f28, -0x20(r11)
.sym _savefpr_29, global
    stfd f29, -0x18(r11)
.sym _savefpr_30, global
    stfd f30, -0x10(r11)
.sym _savefpr_31, global
    stfd f31, -8(r11)
    blr
.endfn __save_fpr

.fn __restore_fpr, global
.sym _restfpr_14, global
    lfd f14, -0x90(r11)
.sym _restfpr_15, global
    lfd f15, -0x88(r11)
.sym _restfpr_16, global
    lfd f16, -0x80(r11)
.sym _restfpr_17, global
    lfd f17, -0x78(r11)
.sym _restfpr_18, global
    lfd f18, -0x70(r11)
.sym _restfpr_19, global
    lfd f19, -0x68(r11)
.sym _restfpr_20, global
    lfd f20, -0x60(r11)
.sym _restfpr_21, global
    lfd f21, -0x58(r11)
.sym _restfpr_22, global
    lfd f22, -0x50(r11)
.sym _restfpr_23, global
    lfd f23, -0x48(r11)
.sym _restfpr_24, global
    lfd f24, -0x40(r11)
.sym _restfpr_25, global
    lfd f25, -0x38(r11)
.sym _restfpr_26, global
    lfd f26, -0x30(r11)
.sym _restfpr_27, global
    lfd f27, -0x28(r11)
.sym _restfpr_28, global
    lfd f28, -0x20(r11)
.sym _restfpr_29, global
    lfd f29, -0x18(r11)
.sym _restfpr_30, global
    lfd f30, -0x10(r11)
.sym _restfpr_31, global
    lfd f31, -8(r11)
    blr
.endfn __restore_fpr

.fn __save_gpr, global
.sym _savegpr_14, global
    stw r14, -0x48(r11)
.sym _savegpr_15, global
    stw r15, -0x44(r11)
.sym _savegpr_16, global
    stw r16, -0x40(r11)
.sym _savegpr_17, global
    stw r17, -0x3c(r11)
.sym _savegpr_18, global
    stw r18, -0x38(r11)
.sym _savegpr_19, global
    stw r19, -0x34(r11)
.sym _savegpr_20, global
    stw r20, -0x30(r11)
.sym _savegpr_21, global
    stw r21, -0x2c(r11)
.sym _savegpr_22, global
    stw r22, -0x28(r11)
.sym _savegpr_23, global
    stw r23, -0x24(r11)
.sym _savegpr_24, global
    stw r24, -0x20(r11)
.sym _savegpr_25, global
    stw r25, -0x1c(r11)
.sym _savegpr_26, global
    stw r26, -0x18(r11)
.sym _savegpr_27, global
    stw r27, -0x14(r11)
.sym _savegpr_28, global
    stw r28, -0x10(r11)
.sym _savegpr_29, global
    stw r29, -0xc(r11)
.sym _savegpr_30, global
    stw r30, -8(r11)
.sym _savegpr_31, global
    stw r31, -4(r11)
    blr
.endfn __save_gpr

.fn __restore_gpr, global
.sym _restgpr_14, global
    lwz r14, -0x48(r11)
.sym _restgpr_15, global
    lwz r15, -0x44(r11)
.sym _restgpr_16, global
    lwz r16, -0x40(r11)
.sym _restgpr_17, global
    lwz r17, -0x3c(r11)
.sym _restgpr_18, global
    lwz r18, -0x38(r11)
.sym _restgpr_19, global
    lwz r19, -0x34(r11)
.sym _restgpr_20, global
    lwz r20, -0x30(r11)
.sym _restgpr_21, global
    lwz r21, -0x2c(r11)
.sym _restgpr_22, global
    lwz r22, -0x28(r11)
.sym _restgpr_23, global
    lwz r23, -0x24(r11)
.sym _restgpr_24, global
    lwz r24, -0x20(r11)
.sym _restgpr_25, global
    lwz r25, -0x1c(r11)
.sym _restgpr_26, global
    lwz r26, -0x18(r11)
.sym _restgpr_27, global
    lwz r27, -0x14(r11)
.sym _restgpr_28, global
    lwz r28, -0x10(r11)
.sym _restgpr_29, global
    lwz r29, -0xc(r11)
.sym _restgpr_30, global
    lwz r30, -8(r11)
.sym _restgpr_31, global
    lwz r31, -4(r11)
    blr
.endfn __restore_gpr

.fn __div2u, global
    cmpwi r3, 0
    cntlzw r0, r3
    cntlzw r9, r4
    bne .L_802868B8
    addi r0, r9, 0x20
.L_802868B8:
    cmpwi r5, 0
    cntlzw r9, r5
    cntlzw r10, r6
    bne .L_802868CC
    addi r9, r10, 0x20
.L_802868CC:
    cmpw r0, r9
    subfic r10, r0, 0x40
    bgt .L_80286984
    addi r9, r9, 1
    subfic r9, r9, 0x40
    add r0, r0, r9
    subf r9, r9, r10
    mtctr r9
    cmpwi r9, 0x20
    subi r7, r9, 0x20
    blt .L_80286904
    srw r8, r3, r7
    li r7, 0
    b .L_80286918
.L_80286904:
    srw r8, r4, r9
    subfic r7, r9, 0x20
    slw r7, r3, r7
    or r8, r8, r7
    srw r7, r3, r9
.L_80286918:
    cmpwi r0, 0x20
    subic r9, r0, 0x20
    blt .L_80286930
    slw r3, r4, r9
    li r4, 0
    b .L_80286944
.L_80286930:
    slw r3, r3, r0
    subfic r9, r0, 0x20
    srw r9, r4, r9
    or r3, r3, r9
    slw r4, r4, r0
.L_80286944:
    li r10, -1
    addic r7, r7, 0
.L_8028694C:
    adde r4, r4, r4
    adde r3, r3, r3
    adde r8, r8, r8
    adde r7, r7, r7
    subfc r0, r6, r8
    subfe. r9, r5, r7
    blt .L_80286974
    mr r8, r0
    mr r7, r9
    addic r0, r10, 1
.L_80286974:
    bdnz .L_8028694C
    adde r4, r4, r4
    adde r3, r3, r3
    blr
.L_80286984:
    li r4, 0
    li r3, 0
    blr
.endfn __div2u

.fn __div2i, global
    stwu r1, -0x10(r1)
    clrrwi. r9, r3, 31
    beq .L_802869A4
    subfic r4, r4, 0
    subfze r3, r3
.L_802869A4:
    stw r9, 8(r1)
    clrrwi. r10, r5, 31
    beq .L_802869B8
    subfic r6, r6, 0
    subfze r5, r5
.L_802869B8:
    stw r10, 0xc(r1)
    cmpwi r3, 0
    cntlzw r0, r3
    cntlzw r9, r4
    bne .L_802869D0
    addi r0, r9, 0x20
.L_802869D0:
    cmpwi r5, 0
    cntlzw r9, r5
    cntlzw r10, r6
    bne .L_802869E4
    addi r9, r10, 0x20
.L_802869E4:
    cmpw r0, r9
    subfic r10, r0, 0x40
    bgt .L_80286AB8
    addi r9, r9, 1
    subfic r9, r9, 0x40
    add r0, r0, r9
    subf r9, r9, r10
    mtctr r9
    cmpwi r9, 0x20
    subi r7, r9, 0x20
    blt .L_80286A1C
    srw r8, r3, r7
    li r7, 0
    b .L_80286A30
.L_80286A1C:
    srw r8, r4, r9
    subfic r7, r9, 0x20
    slw r7, r3, r7
    or r8, r8, r7
    srw r7, r3, r9
.L_80286A30:
    cmpwi r0, 0x20
    subic r9, r0, 0x20
    blt .L_80286A48
    slw r3, r4, r9
    li r4, 0
    b .L_80286A5C
.L_80286A48:
    slw r3, r3, r0
    subfic r9, r0, 0x20
    srw r9, r4, r9
    or r3, r3, r9
    slw r4, r4, r0
.L_80286A5C:
    li r10, -1
    addic r7, r7, 0
.L_80286A64:
    adde r4, r4, r4
    adde r3, r3, r3
    adde r8, r8, r8
    adde r7, r7, r7
    subfc r0, r6, r8
    subfe. r9, r5, r7
    blt .L_80286A8C
    mr r8, r0
    mr r7, r9
    addic r0, r10, 1
.L_80286A8C:
    bdnz .L_80286A64
    adde r4, r4, r4
    adde r3, r3, r3
    lwz r9, 8(r1)
    lwz r10, 0xc(r1)
    xor. r7, r9, r10
    beq .L_80286AC0
    cmpwi r9, 0
    subfic r4, r4, 0
    subfze r3, r3
    b .L_80286AC0
.L_80286AB8:
    li r4, 0
    li r3, 0
.L_80286AC0:
    addi r1, r1, 0x10
    blr
.endfn __div2i

.fn __mod2u, global
    cmpwi r3, 0
    cntlzw r0, r3
    cntlzw r9, r4
    bne .L_80286ADC
    addi r0, r9, 0x20
.L_80286ADC:
    cmpwi r5, 0
    cntlzw r9, r5
    cntlzw r10, r6
    bne .L_80286AF0
    addi r9, r10, 0x20
.L_80286AF0:
    cmpw r0, r9
    subfic r10, r0, 0x40
    bgtlr
    addi r9, r9, 1
    subfic r9, r9, 0x40
    add r0, r0, r9
    subf r9, r9, r10
    mtctr r9
    cmpwi r9, 0x20
    subi r7, r9, 0x20
    blt .L_80286B28
    srw r8, r3, r7
    li r7, 0
    b .L_80286B3C
.L_80286B28:
    srw r8, r4, r9
    subfic r7, r9, 0x20
    slw r7, r3, r7
    or r8, r8, r7
    srw r7, r3, r9
.L_80286B3C:
    cmpwi r0, 0x20
    subic r9, r0, 0x20
    blt .L_80286B54
    slw r3, r4, r9
    li r4, 0
    b .L_80286B68
.L_80286B54:
    slw r3, r3, r0
    subfic r9, r0, 0x20
    srw r9, r4, r9
    or r3, r3, r9
    slw r4, r4, r0
.L_80286B68:
    li r10, -1
    addic r7, r7, 0
.L_80286B70:
    adde r4, r4, r4
    adde r3, r3, r3
    adde r8, r8, r8
    adde r7, r7, r7
    subfc r0, r6, r8
    subfe. r9, r5, r7
    blt .L_80286B98
    mr r8, r0
    mr r7, r9
    addic r0, r10, 1
.L_80286B98:
    bdnz .L_80286B70
    mr r4, r8
    mr r3, r7
    blr
.endfn __mod2u

.fn fn_80286BA8, global
    blr
.endfn fn_80286BA8

.fn __shl2i, global
    subfic r8, r5, 0x20
    subic r9, r5, 0x20
    slw r3, r3, r5
    srw r10, r4, r8
    or r3, r3, r10
    slw r10, r4, r9
    or r3, r3, r10
    slw r4, r4, r5
    blr
.endfn __shl2i

.fn __shr2u, global
    subfic r8, r5, 0x20
    subic r9, r5, 0x20
    srw r4, r4, r5
    slw r10, r3, r8
    or r4, r4, r10
    srw r10, r3, r9
    or r4, r4, r10
    srw r3, r3, r5
    blr
.endfn __shr2u

.fn __shr2i, global
    subfic r8, r5, 0x20
    addic. r9, r5, -0x20
    srw r4, r4, r5
    slw r10, r3, r8
    or r4, r4, r10
    sraw r10, r3, r9
    ble .L_80286C14
    or r4, r4, r10
.L_80286C14:
    sraw r3, r3, r5
    blr
.endfn __shr2i

.fn __cvt_sll_flt, global
    stwu r1, -0x10(r1)
    clrrwi. r5, r3, 31
    beq .L_80286C30
    subfic r4, r4, 0
    subfze r3, r3
.L_80286C30:
    or. r7, r3, r4
    li r6, 0
    beq .L_80286CB8
    cntlzw r7, r3
    cntlzw r8, r4
    extlwi r9, r7, 5, 26
    srawi r9, r9, 31
    and r9, r9, r8
    add r7, r7, r9
    subfic r8, r7, 0x20
    subic r9, r7, 0x20
    slw r3, r3, r7
    srw r10, r4, r8
    or r3, r3, r10
    slw r10, r4, r9
    or r3, r3, r10
    slw r4, r4, r7
    subf r6, r7, r6
    clrlwi r7, r4, 21
    cmpwi r7, 0x400
    addi r6, r6, 0x43e
    blt .L_80286CA0
    bgt .L_80286C94
    rlwinm. r7, r4, 0, 20, 20
    beq .L_80286CA0
.L_80286C94:
    addic r4, r4, 0x800
    addze r3, r3
    addze r6, r6
.L_80286CA0:
    rotrwi r4, r4, 11
    rlwimi r4, r3, 21, 0, 10
    extrwi r3, r3, 20, 1
    slwi r6, r6, 20
    or r3, r6, r3
    or r3, r5, r3
.L_80286CB8:
    stw r3, 8(r1)
    stw r4, 0xc(r1)
    lfd f1, 8(r1)
    frsp f1, f1
    addi r1, r1, 0x10
    blr
.endfn __cvt_sll_flt

.fn __cvt_ull_flt, global
    stwu r1, -0x10(r1)
    or. r7, r3, r4
    li r6, 0
    beq .L_80286D58
    cntlzw r7, r3
    cntlzw r8, r4
    extlwi r9, r7, 5, 26
    srawi r9, r9, 31
    and r9, r9, r8
    add r7, r7, r9
    subfic r8, r7, 0x20
    subic r9, r7, 0x20
    slw r3, r3, r7
    srw r10, r4, r8
    or r3, r3, r10
    slw r10, r4, r9
    or r3, r3, r10
    slw r4, r4, r7
    subf r6, r7, r6
    clrlwi r7, r4, 21
    cmpwi r7, 0x400
    addi r6, r6, 0x43e
    blt .L_80286D44
    bgt .L_80286D38
    rlwinm. r7, r4, 0, 20, 20
    beq .L_80286D44
.L_80286D38:
    addic r4, r4, 0x800
    addze r3, r3
    addze r6, r6
.L_80286D44:
    rotrwi r4, r4, 11
    rlwimi r4, r3, 21, 0, 10
    extrwi r3, r3, 20, 1
    slwi r6, r6, 20
    or r3, r6, r3
.L_80286D58:
    stw r3, 8(r1)
    stw r4, 0xc(r1)
    lfd f1, 8(r1)
    frsp f1, f1
    addi r1, r1, 0x10
    blr
.endfn __cvt_ull_flt

.fn __cvt_dbl_usll, global
    stwu r1, -0x10(r1)
    stfd f1, 8(r1)
    lwz r3, 8(r1)
    lwz r4, 0xc(r1)
    extrwi r5, r3, 11, 1
    cmplwi r5, 0x3ff
    bge .L_80286D98
    li r3, 0
    li r4, 0
    b .L_80286E34
.L_80286D98:
    mr r6, r3
    clrlwi r3, r3, 12
    oris r3, r3, 0x10
    subi r5, r5, 0x433
    cmpwi r5, 0
    bge .L_80286DD8
    neg r5, r5
    subfic r8, r5, 0x20
    subic r9, r5, 0x20
    srw r4, r4, r5
    slw r10, r3, r8
    or r4, r4, r10
    srw r10, r3, r9
    or r4, r4, r10
    srw r3, r3, r5
    b .L_80286E24
.L_80286DD8:
    cmpwi r5, 0xa
    .4byte 0x40A10028
    clrrwi. r6, r6, 31
    beq .L_80286DF4
    lis r3, 0x8000
    li r4, 0
    b .L_80286E34
.L_80286DF4:
    lis r3, 0x7fff
    ori r3, r3, 0xffff
    li r4, -1
    b .L_80286E34
.L_80286E04:
    subfic r8, r5, 0x20
    subic r9, r5, 0x20
    slw r3, r3, r5
    srw r10, r4, r8
    or r3, r3, r10
    slw r10, r4, r9
    or r3, r3, r10
    slw r4, r4, r5
.L_80286E24:
    clrrwi. r6, r6, 31
    beq .L_80286E34
    subfic r4, r4, 0
    subfze r3, r3
.L_80286E34:
    addi r1, r1, 0x10
    blr
.endfn __cvt_dbl_usll

.section .rodata, "a"
.balign 8
.obj __constants, local
    .double 0
    .double 4294967296
    .double 2147483648
.endobj __constants
