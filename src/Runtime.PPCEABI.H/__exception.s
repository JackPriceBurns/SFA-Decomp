.include "macros.inc"
.file "__exception"

.section .init, "ax"
.balign 4

# The MetroTRK exception vector table occupies this block in SFA EN 1.0.
# Keep the raw vector bytes until the individual handlers are recovered.
.global gTRKInterruptVectorTable
gTRKInterruptVectorTable:
.incbin "orig/GSAE01/sys/main.dol", 0x538, 0x1F34

.global gTRKInterruptVectorTableEnd
gTRKInterruptVectorTableEnd:

.fn __TRK_reset, global
/* 8000546C 0000246C  94 21 FF E0 */ stwu r1, -0x20(r1)
/* 80005470 00002470  7C 08 02 A6 */ mflr r0
/* 80005474 00002474  3C 60 80 3E */ lis r3, lc_base@ha
/* 80005478 00002478  90 01 00 24 */ stw r0, 0x24(r1)
/* 8000547C 0000247C  38 63 94 D8 */ addi r3, r3, lc_base@l
/* 80005480 00002480  BF 61 00 0C */ stmw r27, 0xc(r1)
/* 80005484 00002484  80 63 00 00 */ lwz r3, 0x0(r3)
/* 80005488 00002488  28 03 00 44 */ cmplwi r3, 0x44
/* 8000548C 0000248C  41 81 00 2C */ bgt .L_800054B8
/* 80005490 00002490  38 03 40 00 */ addi r0, r3, 0x4000
/* 80005494 00002494  28 00 00 44 */ cmplwi r0, 0x44
/* 80005498 00002498  40 81 00 20 */ ble .L_800054B8
/* 8000549C 0000249C  3C 60 80 3E */ lis r3, gTRKCPUState@ha
/* 800054A0 000024A0  38 63 90 00 */ addi r3, r3, gTRKCPUState@l
/* 800054A4 000024A4  80 03 02 38 */ lwz r0, 0x238(r3)
/* 800054A8 000024A8  54 00 07 BF */ clrlwi. r0, r0, 30
/* 800054AC 000024AC  41 82 00 0C */ beq .L_800054B8
/* 800054B0 000024B0  38 A0 00 44 */ li r5, 0x44
/* 800054B4 000024B4  48 00 00 0C */ b .L_800054C0
.L_800054B8:
/* 800054B8 000024B8  3C 60 80 00 */ lis r3, 0x8000
/* 800054BC 000024BC  38 A3 00 44 */ addi r5, r3, 0x44
.L_800054C0:
/* 800054C0 000024C0  3C 80 80 33 */ lis r4, TRK_ISR_OFFSETS@ha
/* 800054C4 000024C4  3C 60 80 3E */ lis r3, gTRKCPUState@ha
/* 800054C8 000024C8  83 A5 00 00 */ lwz r29, 0x0(r5)
/* 800054CC 000024CC  3B E4 2F 80 */ addi r31, r4, TRK_ISR_OFFSETS@l
/* 800054D0 000024D0  3B C3 90 00 */ addi r30, r3, gTRKCPUState@l
/* 800054D4 000024D4  3B 80 00 00 */ li r28, 0x0
.L_800054D8:
/* 800054D8 000024D8  38 00 00 01 */ li r0, 0x1
/* 800054DC 000024DC  7C 00 E0 30 */ slw r0, r0, r28
/* 800054E0 000024E0  7F A0 00 39 */ and. r0, r29, r0
/* 800054E4 000024E4  41 82 00 68 */ beq .L_8000554C
/* 800054E8 000024E8  3C 60 80 3E */ lis r3, lc_base@ha
/* 800054EC 000024EC  80 DF 00 00 */ lwz r6, 0x0(r31)
/* 800054F0 000024F0  38 63 94 D8 */ addi r3, r3, lc_base@l
/* 800054F4 000024F4  80 63 00 00 */ lwz r3, 0x0(r3)
/* 800054F8 000024F8  7C 06 18 40 */ cmplw r6, r3
/* 800054FC 000024FC  41 80 00 24 */ blt .L_80005520
/* 80005500 00002500  38 03 40 00 */ addi r0, r3, 0x4000
/* 80005504 00002504  7C 06 00 40 */ cmplw r6, r0
/* 80005508 00002508  40 80 00 18 */ bge .L_80005520
/* 8000550C 0000250C  80 1E 02 38 */ lwz r0, 0x238(r30)
/* 80005510 00002510  54 00 07 BF */ clrlwi. r0, r0, 30
/* 80005514 00002514  41 82 00 0C */ beq .L_80005520
/* 80005518 00002518  7C DB 33 78 */ mr r27, r6
/* 8000551C 0000251C  48 00 00 0C */ b .L_80005528
.L_80005520:
/* 80005520 00002520  54 C0 00 BE */ clrlwi r0, r6, 2
/* 80005524 00002524  64 1B 80 00 */ oris r27, r0, 0x8000
.L_80005528:
/* 80005528 00002528  3C 80 80 00 */ lis r4, gTRKInterruptVectorTable@ha
/* 8000552C 0000252C  7F 63 DB 78 */ mr r3, r27
/* 80005530 00002530  38 04 35 38 */ addi r0, r4, gTRKInterruptVectorTable@l
/* 80005534 00002534  38 A0 01 00 */ li r5, 0x100
/* 80005538 00002538  7C 80 32 14 */ add r4, r0, r6
/* 8000553C 0000253C  4B FF DF D9 */ bl TRK_memcpy
/* 80005540 00002540  7F 63 DB 78 */ mr r3, r27
/* 80005544 00002544  38 80 01 00 */ li r4, 0x100
/* 80005548 00002548  48 28 62 01 */ bl TRK_flush_cache
.L_8000554C:
/* 8000554C 0000254C  3B 9C 00 01 */ addi r28, r28, 0x1
/* 80005550 00002550  3B FF 00 04 */ addi r31, r31, 0x4
/* 80005554 00002554  2C 1C 00 0E */ cmpwi r28, 0xe
/* 80005558 00002558  40 81 FF 80 */ ble .L_800054D8
/* 8000555C 0000255C  BB 61 00 0C */ lmw r27, 0xc(r1)
/* 80005560 00002560  80 01 00 24 */ lwz r0, 0x24(r1)
/* 80005564 00002564  7C 08 03 A6 */ mtlr r0
/* 80005568 00002568  38 21 00 20 */ addi r1, r1, 0x20
/* 8000556C 0000256C  4E 80 00 20 */ blr
.endfn __TRK_reset
