/* CIV.EXE callback decompile
 * function : RANDOMRADIOPROC
 * address  : 1058:06ab
 * body size: 79 bytes
 *
 * Source: Ghidra DecompInterface (Pcode).
 * NOT a faithful original-source recovery; the original
 * Borland C++ source has been lost. Variable names are
 * Ghidra synthetic (uVar1 / iVar2). Use as a structural
 * guide only.
 */


void __stdcall16far RANDOMRADIOPROC(undefined4 param_1,undefined2 param_2,int param_3)

{
  int iVar1;
  int *piVar2;
  
  iVar1 = 4;
  piVar2 = (int *)((char *)s_destroyed__1420_0802 + 8);
  do {
    if (*piVar2 == param_3) {
                    /* WARNING: Could not recover jumptable at 0x105806d9. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (*(code *)piVar2[4])();
      return;
    }
    piVar2 = piVar2 + 1;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  DEFWINDOWPROC(0x1058,param_1,param_2,param_3);
  return;
}

