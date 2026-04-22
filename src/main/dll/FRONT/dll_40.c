#include "ghidra_import.h"
#include "main/dll/FRONT/dll_40.h"

extern undefined4 FUN_80244758();

extern undefined4 DAT_803a694c;

void FUN_80118e30(undefined4 param_1)
{
  FUN_80244758((int *)&DAT_803a694c,param_1,1);
  return;
}
