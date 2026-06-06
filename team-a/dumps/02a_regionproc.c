/* CIV.EXE callback decompile
 * function : REGIONPROC
 * address  : 1058:09f6
 * body size: 79 bytes
 *
 * Source: Ghidra DecompInterface (Pcode).
 * NOT a faithful original-source recovery; the original
 * Borland C++ source has been lost. Variable names are
 * Ghidra synthetic (uVar1 / iVar2). Use as a structural
 * guide only.
 */


void __stdcall16far REGIONPROC(undefined4 param_1,undefined2 param_2,int param_3)

{
  int iVar1;
  int *piVar2;
  
  iVar1 = 5;
  piVar2 = (int *)((char *)s_Screen_Dimensions__pixels___1420_0b1b + 0x1e);
  do {
    if (*piVar2 == param_3) {
                    /* WARNING: Could not recover jumptable at 0x10580a24. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (*(code *)piVar2[5])();
      return;
    }
    piVar2 = piVar2 + 1;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  DEFWINDOWPROC(0x1058,param_1,param_2,param_3);
  return;
}

