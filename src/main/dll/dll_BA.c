#include "ghidra_import.h"
#include "main/dll/dll_BA.h"

extern undefined4 DAT_803de19c;

void FUN_80101c10(undefined param_1)
{
  *(undefined *)(DAT_803de19c + 0x139) = param_1;
  return;
}
