/* CIV.EXE callback decompile
 * function : CIVDIALOGPROC
 * address  : 1098:1838
 * body size: 191 bytes
 *
 * Source: Ghidra DecompInterface (Pcode).
 * NOT a faithful original-source recovery; the original
 * Borland C++ source has been lost. Variable names are
 * Ghidra synthetic (uVar1 / iVar2). Use as a structural
 * guide only.
 */


undefined2 __stdcall16far
CIVDIALOGPROC(undefined4 param_1,undefined2 param_2,int param_3,undefined2 param_4)

{
  undefined2 uVar1;
  undefined2 unaff_CS;
  
  if (param_3 == 0x201) {
    DAT_1420_1d4a = 1;
    DAT_1420_1d4c = 1;
  }
  else {
    if (param_3 != 0x202) {
      uVar1 = DEFDLGPROC(unaff_CS,param_1,param_2,param_3);
      return uVar1;
    }
    DAT_1420_1d4a = 0;
    DAT_1420_1d4c = 0;
  }
  DAT_1420_1d48 = param_4;
  DAT_1420_1d46 = (int)((ulong)param_1 >> 0x10);
  DAT_1420_1d44 = (undefined2)param_1;
  return 0;
}

