/* CIV.EXE callback decompile
 * function : TIMERPROC
 * address  : 1008:0d68
 * body size: 43 bytes
 *
 * Source: Ghidra DecompInterface (Pcode).
 * NOT a faithful original-source recovery; the original
 * Borland C++ source has been lost. Variable names are
 * Ghidra synthetic (uVar1 / iVar2). Use as a structural
 * guide only.
 */


undefined2 __stdcall16far TIMERPROC(void)

{
  if (DAT_12d8_24f0 != 0) {
    DAT_12d8_24f0 = DAT_12d8_24f0 + -1;
  }
  return 0;
}

