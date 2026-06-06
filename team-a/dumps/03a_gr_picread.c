/* CIV.EXE gr_pic.c family decompile via byte-pattern scan
 * function : GR_PicRead
 * address  : 10b8:079d
 * body size: 779 bytes
 *
 * Located by scanning for instruction `PUSH 0x2300` (= assert
 * string offset for `GR_PicRead()  :  gr_pic.c` in DGROUP).
 *
 * Source: Ghidra DecompInterface (Pcode).
 * Variable names are Ghidra synthetic (uVar1 / iVar2).
 */


undefined2 __cdecl16far FUN_10b8_079d(int param_1,undefined2 param_2,undefined4 param_3)

{
  ulong uVar1;
  uint uVar2;
  int unaff_BP;
  int iVar3;
  int iVar4;
  char *pcVar5;
  char *pcVar6;
  ulong uVar7;
  undefined2 uVar8;
  undefined2 local_34;
  undefined2 local_32;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined2 local_18;
  undefined2 local_16;
  undefined2 local_14;
  undefined2 local_12;
  undefined2 local_10;
  undefined2 local_e;
  undefined2 local_c;
  undefined4 local_a;
  uint local_6;
  char *local_4;
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  local_4 = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
  local_20 = DAT_1420_1f44;
  local_24 = DAT_1420_1f48;
  local_28 = DAT_1420_1f4c;
  local_2c = DAT_1420_1f50;
  FUN_1100_09c6();
  FUN_1148_003c();
  local_6 = FUN_10b8_0aa8(param_1,&local_34);
  if (local_6 == 0) {
    uVar8 = 0x42;
    pcVar5 = (char *)0x1608;
    uVar2 = GLOBALALLOC();
    if (uVar2 == 0) {
      pcVar5 = (char *)s_nothing_1420_1093 + 5;
      FUN_1098_28e4((long)param_1,(char *)s_Memory_error_during_picture_load_1420_231a,
                    (char *)s_Type__Propulsion_Fuel_1420_1416 + 10,uVar8);
    }
    pcVar6 = (char *)0x1608;
    local_a = GLOBALLOCK(pcVar5,uVar2);
    uVar7 = local_a >> 0x10;
    local_6 = FUN_10b8_11bc(param_1,&local_12);
    if (((DAT_1420_17a8 == 1) &&
        (local_1c = CONCAT22(local_1c._2_2_,(undefined2)local_1c), param_1 != 0x89)) &&
       (local_1c = CONCAT22(local_1c._2_2_,(undefined2)local_1c), local_6 != 0)) {
      uVar7 = (ulong)local_6;
      local_1c = GLOBALLOCK();
      pcVar6 = (char *)s____1_if_government_is_Republic_D_1420_10c5 + 3;
      FUN_10c8_0000(local_1c,local_a,local_e,local_c,param_1,uVar7);
      if (DAT_1420_9980 == 1) {
        if (local_6 != 0) {
          pcVar6 = (char *)0x1608;
          GLOBALUNLOCK((char *)s____1_if_government_is_Republic_D_1420_10c5 + 3,local_6);
        }
        pcVar5 = pcVar6;
        if (local_6 != 0) {
          pcVar5 = (char *)0x1608;
          GLOBALFREE(pcVar6,local_6);
        }
        if (uVar2 != 0) {
          GLOBALUNLOCK(pcVar5,uVar2);
          GLOBALFREE(0x1608,uVar2);
        }
        return 1;
      }
      local_14 = 0x11;
      uVar1 = local_a;
      if (local_6 != 0) {
        pcVar6 = (char *)0x1608;
        GLOBALUNLOCK((char *)s____1_if_government_is_Republic_D_1420_10c5 + 3,local_6);
        uVar1 = local_a;
      }
      local_a._2_2_ = (undefined2)(uVar1 >> 0x10);
      local_a._0_2_ = (int)uVar1;
      for (iVar3 = 0; iVar3 < 0x10; iVar3 = iVar3 + 1) {
        *(undefined4 *)((int)local_a + iVar3 * 4) = *(undefined4 *)(iVar3 * 4 + 2);
      }
      *(undefined4 *)((int)local_a + 0x40) = local_24;
      iVar3 = 0x11;
      if (param_1 == 200) {
        *(undefined4 *)((int)local_a + 0x44) = local_28;
        *(undefined4 *)((int)local_a + 0x48) = local_2c;
        local_14 = 0x13;
        iVar3 = 0x13;
      }
      for (; iVar3 < 0x100; iVar3 = iVar3 + 1) {
        *(undefined4 *)((int)local_a + iVar3 * 4) = local_20;
      }
      local_a = uVar1;
      FUN_10b8_0c1c(param_1,local_e,local_c,local_6);
    }
    if (uVar2 != 0) {
      GLOBALUNLOCK(pcVar6,uVar2,uVar7);
    }
  }
  else {
    local_10 = 0;
    local_12 = 0;
    local_e = local_34;
    local_c = local_32;
    uVar8 = 0x42;
    uVar2 = GLOBALALLOC();
    uVar7 = CONCAT22(uVar2,0x1608);
    local_a = GLOBALLOCK(0x1608,uVar2,uVar8);
    uVar8 = (undefined2)(local_a >> 0x10);
    iVar3 = (int)local_a;
    for (iVar4 = 0; iVar4 < 0x10; iVar4 = iVar4 + 1) {
      *(undefined4 *)(iVar3 + iVar4 * 4) = *(undefined4 *)(iVar4 * 4 + 2);
    }
    *(undefined4 *)(iVar3 + 0x40) = local_24;
    iVar4 = 0x11;
    if (param_1 == 200) {
      *(undefined4 *)(iVar3 + 0x44) = local_28;
      *(undefined4 *)(iVar3 + 0x48) = local_2c;
      local_14 = 0x13;
      iVar4 = 0x13;
    }
    for (; iVar4 < 0x100; iVar4 = iVar4 + 1) {
      *(undefined4 *)(iVar3 + iVar4 * 4) = local_20;
    }
    if (uVar2 != 0) {
      uVar7 = (ulong)uVar2;
      GLOBALUNLOCK();
    }
  }
  FUN_10d0_0000((int)param_3,param_3._2_2_,&local_12);
  *(uint *)((int)param_3 + 0x12) = local_6;
  if (*(int *)((int)param_3 + 0x14) != 0) {
    GLOBALFREE((char *)s____1_if_government_is_Republic_D_1420_10c5 + 0xb,
               *(undefined2 *)((int)param_3 + 0x14),uVar7);
  }
  *(uint *)((int)param_3 + 0x14) = uVar2;
  FUN_1098_1104(local_18,local_16);
  return 0;
}

