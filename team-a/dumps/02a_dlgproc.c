/* CIV.EXE callback decompile
 * function : DLGPROC
 * address  : 1098:16e2
 * body size: 70 bytes
 *
 * Source: Ghidra DecompInterface (Pcode).
 * NOT a faithful original-source recovery; the original
 * Borland C++ source has been lost. Variable names are
 * Ghidra synthetic (uVar1 / iVar2). Use as a structural
 * guide only.
 */


undefined2 __stdcall16far DLGPROC(void)

{
  undefined2 uVar1;
  int iVar2;
  int *piVar3;
  int param_2;
  
  iVar2 = 6;
  piVar3 = (int *)0x1820;
  do {
    if (*piVar3 == param_2) {
                    /* WARNING: Could not recover jumptable at 0x10981713. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      uVar1 = (*(code *)piVar3[6])();
      return uVar1;
    }
    piVar3 = piVar3 + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return 0;
}

