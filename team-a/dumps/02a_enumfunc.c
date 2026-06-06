/* CIV.EXE callback decompile
 * function : ENUMFUNC
 * address  : 1008:0e12
 * body size: 135 bytes
 *
 * Source: Ghidra DecompInterface (Pcode).
 * NOT a faithful original-source recovery; the original
 * Borland C++ source has been lost. Variable names are
 * Ghidra synthetic (uVar1 / iVar2). Use as a structural
 * guide only.
 */


undefined2 __stdcall16far
ENUMFUNC(undefined2 param_1_00,undefined2 param_2_00,uint param_1,undefined2 param_4,
        undefined2 param_5,undefined4 param_2)

{
  int iVar1;
  undefined2 uVar2;
  int local_8;
  undefined2 uStack_6;
  
  if ((param_1 & 1) != 0) {
    uVar2 = 3;
    local_8 = (int)param_2;
    uStack_6 = (undefined2)((ulong)param_2 >> 0x10);
    iVar1 = FUN_1000_3744(local_8 + 0x12,uStack_6,0x11b,
                          (char *)s_Type__Propulsion_Fuel_1420_1416 + 10,3);
    if (iVar1 == 0) {
      *(undefined *)(local_8 + 0xf) = 0xf;
      uVar2 = CREATEFONTINDIRECT(0x1000,local_8);
      ((undefined2 *)&DAT_1420_5484)[DAT_12d8_0022] = uVar2;
      DAT_12d8_0022 = DAT_12d8_0022 + 1;
      uVar2 = uStack_6;
    }
    FUN_1008_09d3(uVar2);
  }
  return 1;
}

