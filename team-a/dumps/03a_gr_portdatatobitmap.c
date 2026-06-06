/* CIV.EXE gr_pic.c family decompile via byte-pattern scan
 * function : GR_PortDataToBitmap
 * address  : 10b8:0e36
 * body size: 902 bytes
 *
 * Located by scanning for instruction `PUSH 0x234A` (= assert
 * string offset for `GR_PortDataToBitmap()  :  gr_pic.c` in DGROUP).
 *
 * Source: Ghidra DecompInterface (Pcode).
 * Variable names are Ghidra synthetic (uVar1 / iVar2).
 */


void __cdecl16far FUN_10b8_0e36(undefined4 param_1,int param_2)

{
  uint uVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  undefined2 uVar7;
  undefined2 uVar8;
  int in_DX;
  uint uVar9;
  long lVar10;
  int iVar11;
  undefined2 uVar12;
  char *pcVar13;
  bool bVar14;
  ulong uVar15;
  undefined2 uVar16;
  undefined2 uVar17;
  undefined4 uVar18;
  int local_1e;
  int local_1c;
  int local_16;
  uint local_e;
  int local_c;
  
  local_c = in_DX;
  FUN_1148_003c((char *)s_GR_PortDataToBitmap_____gr_pic_c_1420_234a,
                (char *)s_Type__Propulsion_Fuel_1420_1416 + 10);
  uVar12 = (undefined2)((ulong)param_1 >> 0x10);
  iVar11 = (int)param_1;
  local_e = GLOBALLOCK((char *)s_improvement_1420_1147 + 1,*(undefined2 *)(iVar11 + 0x12));
  if (*(int *)(iVar11 + 0x10) / 2 << 1 == *(int *)(iVar11 + 0x10)) {
    uVar18 = CONCAT22(local_e,*(undefined2 *)(iVar11 + 0x18));
    uVar2 = GETDEVICECAPS(0x1608,0xc);
    uVar16 = *(undefined2 *)(iVar11 + 0x18);
    uVar3 = GETDEVICECAPS(0x1608,0xe);
    pcVar13 = (char *)s____1_if_government_is_Republic_D_1420_10c5 + 0xb;
    iVar4 = FUN_10d0_0cfa(*(undefined2 *)(iVar11 + 0x14),1,*(undefined2 *)(iVar11 + 0x10),
                          *(undefined2 *)(iVar11 + 0xe),uVar3,uVar16,uVar2,uVar18,local_c);
  }
  else if (DAT_1420_17a6 == 1) {
    uVar3 = 0x42;
    lVar10 = (long)*(int *)(iVar11 + 0xe);
    uVar16 = 0xefb;
    iVar5 = GLOBALALLOC(0x1608,((*(int *)(iVar11 + 0x10) + 1) / 2 << 1) * lVar10);
    uVar9 = (uint)lVar10;
    uVar2 = 0xf03;
    iVar4 = GLOBALLOCK(0x1608,iVar5,uVar3);
    local_16 = iVar4;
    for (local_1e = 0; local_1e < *(int *)(iVar11 + 0xe); local_1e = local_1e + 1) {
      HMEMCPY(0x1608,(long)*(int *)(iVar11 + 0x10),local_e,local_c,local_16);
      local_16 = local_16 + ((*(int *)(iVar11 + 0x10) + 1) / 2) * 2;
      uVar1 = *(uint *)(iVar11 + 0x10);
      bVar14 = CARRY2(local_e,uVar1);
      local_e = local_e + uVar1;
      local_c = local_c + (((int)uVar1 >> 0xf) + (uint)bVar14) * 0x10;
    }
    uVar15 = (ulong)uVar9;
    uVar3 = *(undefined2 *)(iVar11 + 0x18);
    uVar7 = GETDEVICECAPS();
    uVar6 = *(undefined2 *)(iVar11 + 0x18);
    uVar8 = GETDEVICECAPS(0x1608,0xe);
    pcVar13 = (char *)s____1_if_government_is_Republic_D_1420_10c5 + 0xb;
    iVar4 = FUN_10d0_0cfa(*(undefined2 *)(iVar11 + 0x14),1,(*(int *)(iVar11 + 0x10) + 1) / 2 << 1,
                          *(undefined2 *)(iVar11 + 0xe),uVar8,uVar6,uVar7,uVar3,iVar4,uVar15,uVar16,
                          uVar2);
    if (iVar5 != 0) {
      GLOBALUNLOCK((char *)s____1_if_government_is_Republic_D_1420_10c5 + 0xb,iVar5);
      pcVar13 = (char *)0x1608;
      GLOBALFREE();
    }
  }
  else {
    uVar16 = *(undefined2 *)(iVar11 + 0x18);
    uVar2 = CREATECOMPATIBLEDC();
    uVar3 = CREATECOMPATIBLEDC(0x1608,*(undefined2 *)(iVar11 + 0x18),uVar16);
    iVar4 = FUN_10d0_0cfa(*(undefined2 *)(iVar11 + 0x14),0,(*(int *)(iVar11 + 0x10) + 1) / 2 << 1,
                          *(undefined2 *)(iVar11 + 0xe),1,8,0,0);
    uVar6 = FUN_10d0_0cfa(*(undefined2 *)(iVar11 + 0x14),0,(*(int *)(iVar11 + 0x10) + 1) / 2 << 1,1,
                          1,8,0,0);
    uVar7 = SELECTOBJECT((char *)s____1_if_government_is_Republic_D_1420_10c5 + 0xb,uVar6);
    uVar16 = uVar3;
    uVar8 = SELECTOBJECT();
    for (local_1c = 0; local_1c < *(int *)(iVar11 + 0xe); local_1c = local_1c + 1) {
      SETBITMAPBITS(0x1608,local_e,local_c,(long)*(int *)(iVar11 + 0xc));
      uVar9 = *(uint *)(iVar11 + 0x10);
      bVar14 = CARRY2(local_e,uVar9);
      local_e = local_e + uVar9;
      local_c = local_c + (((int)uVar9 >> 0xf) + (uint)bVar14) * 0x10;
      BITBLT(0x1608,0xcc0020,0,0,uVar2,1,*(undefined2 *)(iVar11 + 0x10),local_1c,0);
    }
    uVar17 = uVar2;
    SELECTOBJECT(0x1608,uVar7);
    uVar7 = uVar3;
    SELECTOBJECT(0x1608,uVar8);
    DELETEOBJECT(0x1608,uVar6,uVar7,uVar17,uVar16);
    DELETEDC(0x1608,uVar2);
    pcVar13 = (char *)0x1608;
    DELETEDC(0x1608,uVar3);
  }
  if (*(int *)(iVar11 + 0x12) != 0) {
    pcVar13 = (char *)0x1608;
    GLOBALUNLOCK();
  }
  if (iVar4 != 0) {
    uVar16 = *(undefined2 *)(iVar11 + 0x18);
    SELECTOBJECT(pcVar13,iVar4);
    if (*(int *)(iVar11 + 0x1a) != 0) {
      DELETEOBJECT(0x1608,*(undefined2 *)(iVar11 + 0x1a),uVar16);
    }
    pcVar13 = (char *)0x1608;
    *(int *)(iVar11 + 0x1a) = iVar4;
  }
  if (param_2 != 0) {
    if (*(int *)(iVar11 + 0x12) != 0) {
      pcVar13 = (char *)0x1608;
      GLOBALFREE();
    }
    *(undefined2 *)(iVar11 + 0x12) = 0;
  }
  SETRECT(pcVar13,*(undefined2 *)(iVar11 + 0xe),*(undefined2 *)(iVar11 + 0xc),0,0,iVar11 + 0x20);
  return;
}

