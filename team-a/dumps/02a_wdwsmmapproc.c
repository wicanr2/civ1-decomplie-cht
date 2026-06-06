/* CIV.EXE callback decompile
 * function : WDWSMMAPPROC
 * address  : 1210:0000
 * body size: 77 bytes
 *
 * Source: Ghidra DecompInterface (Pcode).
 * NOT a faithful original-source recovery; the original
 * Borland C++ source has been lost. Variable names are
 * Ghidra synthetic (uVar1 / iVar2). Use as a structural
 * guide only.
 */


void __stdcall16far WDWSMMAPPROC(undefined4 param_1,undefined2 param_2,int param_3)

{
  int iVar1;
  int *piVar2;
  
                    /* Segment:    67
                       Offset:     0008c800
                       Length:     11b8
                       Min Alloc:  11b8
                       Flags:      1d50
                           Code
                           Discardable
                           Moveable
                           Preload
                           Impure (Non-shareable)
                        */
  iVar1 = 9;
  piVar2 = (int *)s_Subject__the_1420_0237;
  do {
    if (*piVar2 == param_3) {
                    /* WARNING: Could not recover jumptable at 0x1210002e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (*(code *)piVar2[9])();
      return;
    }
    piVar2 = piVar2 + 1;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  DEFWINDOWPROC((char *)s_Defense_Strength__1420_1204 + 0xc,param_1,param_2,param_3);
  return;
}

