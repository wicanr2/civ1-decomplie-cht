/* CIV.EXE callback decompile
 * function : RANDOMUSERPROC
 * address  : 1058:0921
 * body size: 213 bytes
 *
 * Source: Ghidra DecompInterface (Pcode).
 * NOT a faithful original-source recovery; the original
 * Borland C++ source has been lost. Variable names are
 * Ghidra synthetic (uVar1 / iVar2). Use as a structural
 * guide only.
 */


undefined2 __stdcall16far
RANDOMUSERPROC(undefined4 param_1,undefined2 param_2,int param_3,undefined2 param_4)

{
  long lVar1;
  undefined2 uVar2;
  undefined2 unaff_CS;
  
  if (param_3 == 1) {
    lVar1 = *(long *)((int)param_1 + 0x12);
    SETWINDOWWORD();
    SETWINDOWWORD(0x1608,(uint)(lVar1 >> 1) & 1,4);
    SETWINDOWWORD(0x1608,(uint)(lVar1 >> 2) & 1,6);
    SETWINDOWWORD(0x1608,(uint)(lVar1 >> 3) & 1,2);
  }
  else {
    if (param_3 != 0xf) {
      uVar2 = DEFWINDOWPROC(unaff_CS,param_1,param_2,param_3);
      return uVar2;
    }
    uVar2 = BEGINPAINT();
    FUN_1058_0440(param_4,uVar2);
    ENDPAINT();
  }
  return 0;
}

