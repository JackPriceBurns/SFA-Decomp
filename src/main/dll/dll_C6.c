#include "ghidra_import.h"
#include "main/dll/dll_C6.h"

extern undefined4 FUN_80100d40();
extern int FUN_80134f70();
extern undefined4 countLeadingZeros();

extern undefined4 DAT_803dc5f0;
extern undefined4 DAT_803de190;
extern undefined4 DAT_803de19c;

void FUN_801023a8(void)
{
  int iVar1;

  iVar1 = FUN_80134f70();
  if (iVar1 == 0) {
    DAT_803dc5f0 = 0xffff;
    countLeadingZeros(0x49 - DAT_803de190);
    FUN_80100d40();
    *(undefined4 *)(DAT_803de19c + 0x120) = 0;
  }
  return;
}
