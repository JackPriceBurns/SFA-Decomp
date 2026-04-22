#include "ghidra_import.h"
#include "main/dll/creator1D6.h"

extern undefined4 FUN_8000b7dc();
extern uint FUN_80020078();
extern undefined4 FUN_800201ac();
extern int FUN_8002ba84();
extern undefined4 FUN_800372f8();
extern undefined4 FUN_801ce430();

extern undefined4 DAT_80327428;
extern undefined4 DAT_80327458;
extern undefined4* DAT_803dd6e8;
extern undefined4* DAT_803dd71c;
extern undefined4* DAT_803dd728;
extern undefined4 DAT_803e5ea0;
extern f32 FLOAT_803e5ee4;
extern f32 FLOAT_803e5eec;
extern f32 FLOAT_803e5ef0;

void FUN_801cfac0(undefined2 *param_1,int param_2,int param_3)
{
}

undefined4 FUN_801cfd5c(void)
{
  int iVar1;

  iVar1 = FUN_8002ba84();
  FUN_8000b7dc(iVar1,0x10);
  return 0;
}

void FUN_801cfd90(void)
{
  FUN_800201ac(0x4e4,1);
  return;
}
