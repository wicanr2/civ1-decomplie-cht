/* CIV.EXE callback decompile
 * function : WDWMAPPROC
 * address  : 1208:0054
 * body size: 203 bytes
 *
 * Source: Ghidra DecompInterface (Pcode).
 * NOT a faithful original-source recovery; the original
 * Borland C++ source has been lost. Variable names are
 * Ghidra synthetic (uVar1 / iVar2). Use as a structural
 * guide only.
 */


undefined2 __stdcall16far WDWMAPPROC(undefined4 param_1,undefined2 param_2,int param_3,int param_4)

{
  undefined2 uVar1;
  int iVar2;
  int *piVar3;
  char *unaff_CS;
  
  DAT_1420_4860 = DAT_1420_4860 + 1;
  if (DAT_12b0_0000 == 0) {
    iVar2 = 0x16;
    piVar3 = (int *)((char *)s_Nuclear_Catastrophe_in_1420_062e + 6);
    do {
      unaff_CS = (char *)s_Defense_Strength__1420_1204 + 4;
      if (*piVar3 == param_3) {
                    /* WARNING: Could not recover jumptable at 0x12080102. Too many branches */
                    /* WARNING: Treating indirect jump as call */
        uVar1 = (*(code *)piVar3[0x16])();
        return uVar1;
      }
      piVar3 = piVar3 + 1;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
LAB_1208_0619:
    uVar1 = DEFWINDOWPROC(unaff_CS,param_1,param_2,param_3);
  }
  else {
    if (DAT_12b0_0004 == param_4) {
      if (param_3 == 5) {
        DAT_12b0_0002 = 1;
        DAT_1420_994c = param_4;
        DAT_1420_994e = param_3;
        DAT_1420_9950 = param_2;
        DAT_1420_9952 = param_1;
      }
      else if (param_3 != 0x10) goto LAB_1208_0619;
    }
    uVar1 = 0;
  }
  return uVar1;
}

