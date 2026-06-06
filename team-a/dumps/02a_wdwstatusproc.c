/* CIV.EXE callback decompile
 * function : WDWSTATUSPROC
 * address  : 1218:0000
 * body size: 77 bytes
 *
 * Source: Ghidra DecompInterface (Pcode).
 * NOT a faithful original-source recovery; the original
 * Borland C++ source has been lost. Variable names are
 * Ghidra synthetic (uVar1 / iVar2). Use as a structural
 * guide only.
 */


void __stdcall16far WDWSTATUSPROC(undefined4 param_1,undefined2 param_2,int param_3)

{
  int iVar1;
  int *piVar2;
  
                    /* Segment:    68
                       Offset:     0008e200
                       Length:     0b77
                       Min Alloc:  0b77
                       Flags:      1d50
                           Code
                           Discardable
                           Moveable
                           Preload
                           Impure (Non-shareable)
                        */
  iVar1 = 10;
  piVar2 = (int *)0x35e;
  do {
    if (*piVar2 == param_3) {
                    /* WARNING: Could not recover jumptable at 0x1218002e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (*(code *)piVar2[10])();
      return;
    }
    piVar2 = piVar2 + 1;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  DEFWINDOWPROC((char *)s_Movement_Rate__1420_1217 + 1,param_1,param_2,param_3);
  return;
}

