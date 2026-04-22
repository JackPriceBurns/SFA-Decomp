#include "ghidra_import.h"
#include "main/dll/dll_4B.h"

extern undefined4* DAT_803dd720;

void FUN_8011c150(void)
{
  (**(code **)(*DAT_803dd720 + 8))();
  return;
}
