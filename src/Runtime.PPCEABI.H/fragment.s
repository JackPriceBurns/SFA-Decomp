.include "macros.inc"
.file "fragment"

.text
.balign 4

.fn __unregister_fragment, global
/* 80286EB8 00282DF8  2C 03 00 00 */	cmpwi r3, 0x0
/* 80286EBC 00282DFC  4D 80 00 20 */	bltlr
/* 80286EC0 00282E00  2C 03 00 01 */	cmpwi r3, 0x1
/* 80286EC4 00282E04  4C 80 00 20 */	bgelr
/* 80286EC8 00282E08  1C 83 00 0C */	mulli r4, r3, 0xc
/* 80286ECC 00282E0C  3C 60 80 3D */	lis r3, fragmentinfo_803D7540@ha
/* 80286ED0 00282E10  38 00 00 00 */	li r0, 0x0
/* 80286ED4 00282E14  38 63 75 40 */	addi r3, r3, fragmentinfo_803D7540@l
/* 80286ED8 00282E18  7C 63 22 14 */	add r3, r3, r4
/* 80286EDC 00282E1C  90 03 00 00 */	stw r0, 0x0(r3)
/* 80286EE0 00282E20  90 03 00 04 */	stw r0, 0x4(r3)
/* 80286EE4 00282E24  90 03 00 08 */	stw r0, 0x8(r3)
/* 80286EE8 00282E28  4E 80 00 20 */	blr
.endfn __unregister_fragment

.fn __register_fragment, global
/* 80286EEC 00282E2C  3C A0 80 3D */	lis r5, fragmentinfo_803D7540@ha
/* 80286EF0 00282E30  38 A5 75 40 */	addi r5, r5, fragmentinfo_803D7540@l
/* 80286EF4 00282E34  80 05 00 08 */	lwz r0, 0x8(r5)
/* 80286EF8 00282E38  2C 00 00 00 */	cmpwi r0, 0x0
/* 80286EFC 00282E3C  40 82 00 1C */	bne .L_80286F18
/* 80286F00 00282E40  90 65 00 00 */	stw r3, 0x0(r5)
/* 80286F04 00282E44  38 00 00 01 */	li r0, 0x1
/* 80286F08 00282E48  38 60 00 00 */	li r3, 0x0
/* 80286F0C 00282E4C  90 85 00 04 */	stw r4, 0x4(r5)
/* 80286F10 00282E50  90 05 00 08 */	stw r0, 0x8(r5)
/* 80286F14 00282E54  4E 80 00 20 */	blr
.L_80286F18:
/* 80286F18 00282E58  38 60 FF FF */	li r3, -0x1
/* 80286F1C 00282E5C  4E 80 00 20 */	blr
.endfn __register_fragment

.section .bss, "wa", @nobits
.balign 8

.obj fragmentinfo_803D7540, global
	.skip 0xC
.endobj fragmentinfo_803D7540
