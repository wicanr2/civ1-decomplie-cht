/* CIV.EXE gr_pic.c family decompile via byte-pattern scan
 * function : InvertBitmap
 * address  : 10b8:1489
 * body size: 259 bytes
 *
 * Located by scanning for instruction `PUSH 0x23B2` (= assert
 * string offset for `InvertBitmap()  :  gr_pic.c` in DGROUP).
 *
 * Source: Ghidra DecompInterface (Pcode).
 * Variable names are Ghidra synthetic (uVar1 / iVar2).
 */


void __cdecl16far FUN_10b8_1489(uint param_1,int param_2,uint param_3,uint param_4)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined4 uVar7;
  undefined2 uVar8;
  
  FUN_1148_003c();
  if (1 < param_4) {
    iVar6 = 0;
    uVar1 = FUN_1000_1368();
    iVar2 = (uVar1 + param_1) - param_3;
    iVar4 = (iVar6 + (uint)CARRY2(uVar1,param_1)) * 0x10 + param_2 +
            (uint)(uVar1 + param_1 < param_3) * -0x10;
    uVar8 = 0x42;
    iVar5 = iVar4;
    iVar3 = GLOBALALLOC(0x1000,param_3);
    iVar6 = iVar3;
    uVar8 = GLOBALLOCK(0x1608,iVar3,uVar8);
    uVar7 = CONCAT22(iVar6,iVar5);
    HMEMCPY(0x1608,param_3,0,iVar2,iVar4,uVar8);
    HMEMCPY(0x1608,param_3,0,param_1,param_2,iVar2);
    iVar6 = param_2;
    HMEMCPY(0x1608,param_3,0,uVar8,iVar5,param_1);
    if (iVar3 != 0) {
      GLOBALUNLOCK(0x1608,iVar3,iVar6,iVar4,uVar7);
      GLOBALFREE(0x1608,iVar3);
    }
    FUN_10b8_1489(param_1 + param_3,param_2 + (uint)CARRY2(param_1,param_3) * 0x10,param_3,
                  param_4 - 2);
  }
  return;
}

