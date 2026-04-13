.include "macros.inc"
.file "mem_TRK"

.section .init, "ax"
.balign 4

.fn TRK_memset, global
/* 800034E4 000004E4  94 21 FF F0 */ stwu r1, -0x10(r1)
/* 800034E8 000004E8  7C 08 02 A6 */ mflr r0
/* 800034EC 000004EC  90 01 00 14 */ stw r0, 0x14(r1)
/* 800034F0 000004F0  93 E1 00 0C */ stw r31, 0xc(r1)
/* 800034F4 000004F4  7C 7F 1B 78 */ mr r31, r3
/* 800034F8 000004F8  48 28 82 89 */ bl TRK_fill_mem
/* 800034FC 000004FC  80 01 00 14 */ lwz r0, 0x14(r1)
/* 80003500 00000500  7F E3 FB 78 */ mr r3, r31
/* 80003504 00000504  83 E1 00 0C */ lwz r31, 0xc(r1)
/* 80003508 00000508  7C 08 03 A6 */ mtlr r0
/* 8000350C 0000050C  38 21 00 10 */ addi r1, r1, 0x10
/* 80003510 00000510  4E 80 00 20 */ blr
.endfn TRK_memset

.fn TRK_memcpy, global
/* 80003514 00000514  38 84 FF FF */ subi r4, r4, 0x1
/* 80003518 00000518  38 C3 FF FF */ subi r6, r3, 0x1
/* 8000351C 0000051C  38 A5 00 01 */ addi r5, r5, 0x1
/* 80003520 00000520  48 00 00 0C */ b .L_8000352C
.L_80003524:
/* 80003524 00000524  8C 04 00 01 */ lbzu r0, 0x1(r4)
/* 80003528 00000528  9C 06 00 01 */ stbu r0, 0x1(r6)
.L_8000352C:
/* 8000352C 0000052C  34 A5 FF FF */ subic. r5, r5, 0x1
/* 80003530 00000530  40 82 FF F4 */ bne .L_80003524
/* 80003534 00000534  4E 80 00 20 */ blr
.endfn TRK_memcpy
