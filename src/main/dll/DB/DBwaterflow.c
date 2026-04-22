#include "ghidra_import.h"
#include "main/dll/DB/DBwaterflow.h"

extern uint FUN_80020078();
extern undefined4 FUN_80037a5c();
extern undefined4 FUN_8003b9ec();
extern undefined4 FUN_801dfa9c();

extern undefined4* DAT_803dd6d4;

void FUN_801dfe50(int param_1)
{
  char in_r8;

  if (in_r8 != '\0') {
    FUN_8003b9ec(param_1);
  }
  return;
}

void FUN_801dfe84(undefined2 *param_1)
{
  uint uVar1;

  *param_1 = 0x2000;
  uVar1 = FUN_80020078(0x75);
  if (uVar1 == 0) {
    (**(code **)(*DAT_803dd6d4 + 0x48))(0,param_1,0xffffffff);
  }
  return;
}

void FUN_801dfee4(undefined2 *param_1)
{
}

void FUN_801dff38(int param_1)
{
  char in_r8;

  if (in_r8 != '\0') {
    FUN_8003b9ec(param_1);
  }
  return;
}

void FUN_801dff70(int param_1)
{
  FUN_80037a5c(param_1,2);
  return;
}

void FUN_801dffc0(int param_1)
{
  char in_r8;

  if (in_r8 != '\0') {
    FUN_8003b9ec(param_1);
  }
  return;
}
