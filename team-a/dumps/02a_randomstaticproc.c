/* CIV.EXE callback decompile
 * function : RANDOMSTATICPROC
 * address  : 1058:081a
 * body size: 263 bytes
 *
 * Source: Ghidra DecompInterface (Pcode).
 * NOT a faithful original-source recovery; the original
 * Borland C++ source has been lost. Variable names are
 * Ghidra synthetic (uVar1 / iVar2). Use as a structural
 * guide only.
 */


undefined2 __stdcall16far
RANDOMSTATICPROC(undefined4 param_1,undefined2 param_2,int param_3,undefined2 param_4)

{
  long lVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  
  if (param_3 == 1) {
    lVar1 = *(long *)((int)param_1 + 0x12);
    SETWINDOWWORD();
    SETWINDOWWORD(0x1608,(uint)(lVar1 >> 1) & 1,4);
    SETWINDOWWORD();
    SETWINDOWWORD(0x1608,(uint)(lVar1 >> 3) & 1,2);
  }
  else if (param_3 == 0xf) {
    uVar3 = BEGINPAINT();
    FUN_1058_04f6(param_4,uVar3);
    ENDPAINT();
  }
  else {
    if (param_3 != 0x202) {
      uVar3 = DEFWINDOWPROC();
      return uVar3;
    }
    DAT_1280_0008 = param_4;
    GETPARENT();
    uVar3 = param_4;
    uVar2 = GETWINDOWWORD();
    SENDMESSAGE(0x1608,param_4,uVar2,uVar3);
  }
  return 0;
}

