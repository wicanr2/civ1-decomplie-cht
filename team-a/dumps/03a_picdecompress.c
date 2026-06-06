/* CIV.EXE gr_pic.c family decompile via byte-pattern scan
 * function : PicDecompress
 * address  : 10b8:158c
 * body size: 1474 bytes
 *
 * Located by scanning for instruction `PUSH 0x23CE` (= assert
 * string offset for `PicDecompress()  :  gr_pic.c` in DGROUP).
 *
 * Source: Ghidra DecompInterface (Pcode).
 * Variable names are Ghidra synthetic (uVar1 / iVar2).
 */


/* WARNING: Stack frame is not setup normally: Input value of stackpointer is not used */

undefined __cdecl16far FUN_10b8_158c(void)

{
  uint *puVar1;
  int *piVar2;
  undefined *puVar3;
  undefined *puVar4;
  char cVar5;
  undefined uVar6;
  undefined *puVar7;
  undefined2 uVar8;
  int iVar9;
  uint uVar10;
  uint uVar11;
  undefined2 in_DX;
  int iVar12;
  int iVar13;
  ulong uVar14;
  int iVar15;
  undefined2 *puVar16;
  undefined2 *puVar17;
  undefined4 *puVar18;
  undefined *puVar20;
  undefined4 *puVar21;
  undefined4 *puVar22;
  undefined4 *puVar23;
  undefined4 *puVar24;
  undefined4 in_ESP;
  undefined4 *puVar26;
  undefined4 *puVar27;
  int unaff_BP;
  undefined *puVar28;
  undefined *unaff_SI;
  int iVar29;
  undefined *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_CS;
  char *pcVar30;
  undefined2 unaff_SS;
  undefined2 uVar31;
  undefined4 *puVar19;
  undefined2 *puVar25;
  
  iVar15 = (int)in_ESP;
  uVar31 = (undefined2)((ulong)in_ESP >> 0x10);
  puVar28 = (undefined *)(iVar15 + -2);
  *(int *)(iVar15 + -2) = unaff_BP + 1;
  *(undefined2 *)(iVar15 + -4) = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
  *(undefined2 *)(iVar15 + -0x52) = unaff_SI;
  *(undefined2 *)(iVar15 + -0x54) = unaff_DI;
  *(undefined2 *)(iVar15 + -0x56) = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
  *(undefined2 *)(iVar15 + -0x58) = (char *)s_PicDecompress_____gr_pic_c_1420_23ce;
  *(undefined2 *)(iVar15 + -0x5a) = unaff_CS;
  puVar16 = (undefined2 *)(iVar15 + -0x5c);
  *(undefined2 *)(iVar15 + -0x5c) = (char *)s_The_Top_Five_Cities_in_the_World_1420_159c + 8;
  FUN_1148_003c();
  *(undefined *)(iVar15 + -6) = *(undefined *)(iVar15 + 10);
  *(undefined4 *)(iVar15 + -0x34) = *(undefined4 *)(iVar15 + 4);
  *(undefined4 *)(iVar15 + -0x2c) = *(undefined4 *)(iVar15 + 0x12);
  *(undefined4 *)(iVar15 + -0x24) = *(undefined4 *)(iVar15 + -0x34);
  puVar16[3] = 0x42;
  *(undefined4 *)(puVar16 + 1) = 0x6000;
  *puVar16 = (char *)s_improvement_1420_1147 + 1;
  pcVar30 = (char *)0x1608;
  puVar17 = puVar16 + -1;
  puVar16[-1] = (char *)s_plans_retirement_in_20_years__1420_15d0 + 2;
  uVar8 = GLOBALALLOC();
  puVar20 = (undefined *)((int)puVar17 + 8);
  *(undefined2 *)(iVar15 + -0x36) = uVar8;
  if (*(int *)(iVar15 + -0x36) == 0) {
    *(undefined2 *)((int)puVar17 + 6) = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
    *(undefined2 *)((int)puVar17 + 4) = (char *)s_Memory_error_during_allocation_o_1420_23eb;
    puVar26 = (undefined4 *)((undefined *)CONCAT22(uVar31,(undefined *)((int)puVar17 + 4)) + -4);
    *puVar26 = 0;
    puVar18 = (undefined4 *)puVar26;
    uVar31 = (undefined2)((ulong)puVar26 >> 0x10);
    *(undefined2 *)((int)puVar18 + -2) = 0x1608;
    pcVar30 = (char *)s_nothing_1420_1093 + 5;
    puVar19 = puVar18 + -1;
    *(char **)(puVar18 + -1) = (char *)s_plans_retirement_in_20_years__1420_15d0 + 0x17;
    FUN_1098_28e4();
    puVar20 = (undefined *)((int)puVar19 + 0xc);
  }
  *(undefined2 *)(puVar20 + -2) = *(undefined2 *)(iVar15 + -0x36);
  *(char **)(puVar20 + -4) = pcVar30;
  puVar21 = (undefined4 *)(puVar20 + -6);
  *(undefined2 *)(puVar20 + -6) = 0x15f2;
  uVar8 = GLOBALLOCK();
  *(undefined2 *)(iVar15 + -0x38) = in_DX;
  *(undefined2 *)(iVar15 + -0x3a) = uVar8;
  iVar9 = *(int *)(iVar15 + -0x3a);
  *(undefined2 *)(iVar15 + -0x3c) = *(undefined2 *)(iVar15 + -0x38);
  *(int *)(iVar15 + -0x3e) = iVar9 + 0x2000;
  iVar9 = *(int *)(iVar15 + -0x3a);
  *(undefined2 *)(iVar15 + -0x40) = *(undefined2 *)(iVar15 + -0x38);
  *(int *)(iVar15 + -0x42) = iVar9 + 0x4000;
  *(int *)(iVar15 + -0x4c) = *(int *)(iVar15 + 0xc) - *(int *)(iVar15 + 0xe);
  *(undefined2 *)(iVar15 + -0x4e) = 0;
  if (((*(long *)(iVar15 + -0x42) == 0) || (*(long *)(iVar15 + -0x3a) == 0)) ||
     (*(long *)(iVar15 + -0x3e) == 0)) {
    return 1;
  }
  iVar9 = 2 << (*(byte *)(iVar15 + 10) & 0x1f);
  *(int *)(iVar15 + -0xc) = iVar9;
  *(int *)(iVar15 + -0x1c) = iVar9;
  *(int *)(iVar15 + -0x1e) = *(int *)(iVar15 + -0x1c) + 2;
  cVar5 = *(char *)(iVar15 + -6);
  *(char *)(iVar15 + -6) = cVar5 + '\x01';
  *(char *)(iVar15 + -7) = cVar5 + '\x01';
  iVar9 = (2 << (*(byte *)(iVar15 + -7) & 0x1f)) + -2;
  *(int *)(iVar15 + -0x1a) = iVar9;
  *(int *)(iVar15 + -0x10) = iVar9;
  *(int *)(iVar15 + -10) = *(int *)(iVar15 + -0x1e) + 2;
  *(undefined4 *)(iVar15 + -0x14) = 0;
  *(undefined *)(iVar15 + -5) = 0;
  iVar9 = 0;
  *(undefined2 *)((int)puVar21 + 2) = 0x1608;
  uVar8 = 0x1000;
  *(char **)puVar21 = (char *)s_Travellers_report__1420_168d + 0xb;
  uVar10 = FUN_1000_1368();
  uVar11 = *(uint *)(iVar15 + 0x12);
  *(int *)(iVar15 + -0x26) =
       (iVar9 + (uint)CARRY2(uVar10,*(uint *)(iVar15 + 0x12))) * 0x10 + *(int *)(iVar15 + 0x14);
  *(int *)(iVar15 + -0x28) = uVar10 + uVar11;
  *(undefined2 *)(iVar15 + -0xe) = 0;
  while (*(uint *)(iVar15 + -0xe) < *(uint *)(iVar15 + -10)) {
    *(undefined2 *)((int)*(undefined4 *)(iVar15 + -0x3a) + *(int *)(iVar15 + -0xe) * 2) = 0xffff;
    *(int *)((int)*(undefined4 *)(iVar15 + -0x3e) + *(int *)(iVar15 + -0xe) * 2) =
         *(int *)(iVar15 + -0xe) + (uint)*(byte *)(iVar15 + 8) + 10;
    unaff_ES = (undefined2)((ulong)*(undefined4 *)(iVar15 + -0x42) >> 0x10);
    *(undefined2 *)((int)*(undefined4 *)(iVar15 + -0x42) + *(int *)(iVar15 + -0xe) * 2) = 1;
    *(int *)(iVar15 + -0xe) = *(int *)(iVar15 + -0xe) + 1;
  }
  *(undefined2 *)((int)puVar21 + 2) = unaff_ES;
  puVar26 = (undefined4 *)CONCAT22(uVar31,puVar21);
  *(char **)puVar21 = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
  iVar9 = *(int *)(iVar15 + -0x38);
  *(int *)(iVar15 + -0x44) = iVar9;
  *(undefined2 *)(iVar15 + -0x46) = *(undefined2 *)(iVar15 + -0x3a);
  *(int *)(iVar15 + -0x48) = *(int *)(iVar15 + -0x3e) + (*(int *)(iVar15 + -0x3c) - iVar9) * 0x10;
  *(int *)(iVar15 + -0x4a) = *(int *)(iVar15 + -0x42) + (*(int *)(iVar15 + -0x40) - iVar9) * 0x10;
  do {
    if (*(uint *)(puVar28 + 0x10) <= *(uint *)(puVar28 + -0x4c)) {
      *(uint *)(puVar28 + -0x1e) = *(uint *)(puVar28 + -0x4c) - *(int *)(puVar28 + 0x10);
      *(undefined4 *)(puVar28 + -0x2e) = *(undefined4 *)(puVar28 + -0x2a);
      uVar11 = *(uint *)(puVar28 + -0x4a);
      uVar10 = *(uint *)(puVar28 + -0x2e);
      *(uint *)(puVar28 + -0x28) =
           (uint)CARRY2(uVar11,*(uint *)(puVar28 + -0x2e)) * 0x10 + *(int *)(puVar28 + -0x2c);
      *(uint *)(puVar28 + -0x2a) = uVar11 + uVar10;
      puVar7 = *(undefined **)(puVar28 + -0x2e);
      unaff_SI = (undefined *)puVar7;
      unaff_DI = *(undefined **)(puVar28 + -0x2a);
      for (iVar9 = *(int *)(puVar28 + -0x1e); iVar9 != 0; iVar9 = iVar9 + -1) {
        puVar4 = unaff_DI;
        unaff_DI = unaff_DI + -1;
        puVar3 = unaff_SI;
        unaff_SI = unaff_SI + -1;
        *puVar4 = *puVar3;
      }
      *(undefined2 *)(puVar28 + -0x4c) = *(undefined2 *)(puVar28 + -0x1e);
    }
    *(undefined2 *)(puVar28 + -0xc) = *(undefined2 *)(puVar28 + -0x12);
    if ((byte)puVar28[-3] < (byte)puVar28[-5]) {
      do {
        uVar11 = *(uint *)(puVar28 + -0x22);
        uVar10 = *(uint *)(puVar28 + -0x32);
        puVar22 = (undefined4 *)puVar26;
        *(int *)((int)puVar22 + -2) =
             ((*(uint *)(puVar28 + -0x20) >> 3) - (*(uint *)(puVar28 + -0x30) >> 3)) -
             (uint)(uVar11 < *(uint *)(puVar28 + -0x32));
        puVar27 = (undefined4 *)CONCAT22((int)((ulong)puVar26 >> 0x10),puVar22 + -1);
        *(uint *)(puVar22 + -1) = uVar11 - uVar10;
        puVar26 = puVar27 + 1;
        *(int *)(puVar28 + -0x1e) = (int)*puVar27;
        if (*(int *)(puVar28 + -0x1e) == 0) {
          *(uint *)(puVar28 + -0x1e) = **(byte **)(puVar28 + -0x32) + 1;
          puVar1 = (uint *)(puVar28 + -0x32);
          uVar11 = *puVar1;
          *puVar1 = *puVar1 + 1;
          *(uint *)(puVar28 + -0x30) = *(int *)(puVar28 + -0x30) + (uint)(0xfffe < uVar11) * 0x10;
          uVar10 = *(uint *)(puVar28 + -0x1e);
          puVar1 = (uint *)(puVar28 + -0x22);
          uVar11 = *puVar1;
          *puVar1 = *puVar1 + uVar10;
          *(uint *)(puVar28 + -0x20) =
               *(int *)(puVar28 + -0x20) + (uint)CARRY2(uVar11,uVar10) * 0x10;
        }
        uVar6 = **(undefined **)(puVar28 + -0x32);
        puVar1 = (uint *)(puVar28 + -0x32);
        uVar11 = *puVar1;
        *puVar1 = *puVar1 + 1;
        *(uint *)(puVar28 + -0x30) = *(int *)(puVar28 + -0x30) + (uint)(0xfffe < uVar11) * 0x10;
        puVar28[-0x16] = uVar6;
        puVar28[-0x15] = 0;
        *(undefined2 *)(puVar28 + -0x14) = 0;
        *(ulong *)(puVar28 + -0x12) =
             *(long *)(puVar28 + -0x16) << (puVar28[-3] & 0x1f) | *(ulong *)(puVar28 + -0x12);
        *(undefined2 *)(puVar28 + -0xc) = *(undefined2 *)(puVar28 + -0x12);
        puVar28[-3] = puVar28[-3] + '\b';
      } while ((byte)puVar28[-3] < (byte)puVar28[-5]);
    }
    uVar11 = *(int *)(puVar28 + -0xc) << 1 & *(uint *)(puVar28 + -0xe);
    *(uint *)(puVar28 + -0xc) = uVar11;
    puVar23 = (undefined4 *)puVar26;
    if (*(uint *)(puVar28 + -8) < uVar11) {
      puVar28[-0x4d] = 1;
      goto LAB_10b8_1b1f;
    }
    *(ulong *)(puVar28 + -0x12) = *(ulong *)(puVar28 + -0x12) >> (puVar28[-5] & 0x1f);
    puVar28[-3] = puVar28[-3] - puVar28[-5];
    if (*(int *)(puVar28 + -0xc) == *(int *)(puVar28 + -0x1c)) {
      puVar28[-0x4d] = 2;
LAB_10b8_1b1f:
      puVar24 = puVar23 + 1;
      if (*(int *)(puVar28 + -0x34) != 0) {
        *(undefined2 *)((int)puVar23 + 2) = *(undefined2 *)(puVar28 + -0x34);
        *(undefined2 *)puVar23 = 0x1000;
        uVar8 = 0x1608;
        puVar25 = (undefined2 *)((int)puVar23 + -2);
        *(undefined2 *)((int)puVar23 + -2) = 0x1b2f;
        GLOBALUNLOCK();
        puVar24 = (undefined4 *)((int)puVar25 + 4);
      }
      if (*(int *)(puVar28 + -0x34) != 0) {
        *(undefined2 *)((int)puVar24 + -2) = *(undefined2 *)(puVar28 + -0x34);
        *(undefined2 *)((int)puVar24 + -4) = uVar8;
        *(undefined2 *)((int)puVar24 + -6) = 0x1b3d;
        GLOBALFREE();
      }
      return puVar28[-0x4d];
    }
    if (*(int *)(puVar28 + -0xc) == *(int *)(puVar28 + -0x1a)) {
      puVar28[-5] = puVar28[-4];
      *(undefined2 *)(puVar28 + -0xe) = *(undefined2 *)(puVar28 + -0x18);
      *(undefined2 *)(puVar28 + -10) = *(undefined2 *)(puVar28 + -0x1a);
      *(int *)(puVar28 + -8) = *(int *)(puVar28 + -0x1c) + 2;
    }
    else if (*(int *)(puVar28 + -10) == *(int *)(puVar28 + -0x1a)) {
      unaff_DI = (undefined *)*(undefined **)(puVar28 + -0x40);
      *(undefined2 *)(puVar28 + -0x1e) = *(undefined2 *)(unaff_DI + *(int *)(puVar28 + -0xc));
      uVar10 = *(uint *)(puVar28 + -0x1e);
      puVar1 = (uint *)(puVar28 + -0x2a);
      uVar11 = *puVar1;
      *puVar1 = *puVar1 + uVar10;
      *(uint *)(puVar28 + -0x28) = *(int *)(puVar28 + -0x28) + (uint)CARRY2(uVar11,uVar10) * 0x10;
      *(int *)(puVar28 + -0x4c) = *(int *)(puVar28 + -0x4c) + *(int *)(puVar28 + -0x1e);
      *(undefined4 *)(puVar28 + -0x2e) = *(undefined4 *)(puVar28 + -0x2a);
      uVar11 = *(uint *)(puVar28 + -0x26);
      if ((*(uint *)(puVar28 + -0x24) <= *(uint *)(puVar28 + -0x28)) &&
         ((*(uint *)(puVar28 + -0x24) < *(uint *)(puVar28 + -0x28) ||
          (uVar11 < *(uint *)(puVar28 + -0x2a))))) {
        puVar28[-0x4d] = 3;
        goto LAB_10b8_1b1f;
      }
      iVar9 = *(int *)(puVar28 + -0xc);
      *(int *)(puVar28 + -10) = iVar9;
      uVar31 = (undefined2)(*(ulong *)(puVar28 + -0x38) >> 0x10);
      uVar14 = *(ulong *)(puVar28 + -0x38) & 0xffff;
      iVar29 = *(int *)(puVar28 + -0x46);
      do {
        *(int *)((int)puVar23 + -2) = iVar9;
        *(int *)(puVar23 + -1) = iVar29;
        *(undefined2 *)((int)puVar23 + -6) = (int)uVar14;
        *(uint *)(puVar23 + -2) = uVar11;
        *(undefined2 *)((int)puVar23 + -10) = puVar23;
        *(undefined **)(puVar23 + -3) = puVar28;
        *(undefined2 *)((int)puVar23 + -0xe) = unaff_SI;
        *(undefined **)(puVar23 + -4) = unaff_DI;
        piVar2 = (int *)(puVar28 + -0x2e);
        iVar9 = *piVar2;
        *piVar2 = *piVar2 + -1;
        *(uint *)(puVar28 + -0x2c) = *(int *)(puVar28 + -0x2c) + (uint)(iVar9 == 0) * -0x10;
        puVar28 = *(undefined **)(puVar23 + -3);
        unaff_SI = (undefined *)*(undefined2 *)((int)puVar23 + -6);
        iVar29 = *(int *)(puVar23 + -1);
        puVar7 = *(undefined **)(puVar28 + -0x2e);
        unaff_ES = (undefined2)((ulong)puVar7 >> 0x10);
        unaff_DI = (undefined *)puVar7;
        iVar9 = *(int *)(puVar28 + -0xc);
        *puVar7 = *(undefined *)(iVar9 + iVar29);
        if (*(int *)(unaff_SI + iVar9) == -1) break;
        *(int *)((int)puVar23 + -2) = *(int *)(unaff_SI + iVar9);
        *(int *)(puVar23 + -1) = iVar29;
        *(undefined2 *)((int)puVar23 + -6) = unaff_SI;
        *(int *)(puVar23 + -2) = iVar9;
        *(undefined2 *)((int)puVar23 + -10) = puVar23;
        *(undefined **)(puVar23 + -3) = puVar28;
        *(undefined2 *)((int)puVar23 + -0xe) = unaff_SI;
        *(undefined **)(puVar23 + -4) = unaff_DI;
        piVar2 = (int *)(puVar28 + -0x2e);
        iVar9 = *piVar2;
        *piVar2 = *piVar2 + -1;
        *(uint *)(puVar28 + -0x2c) = *(int *)(puVar28 + -0x2c) + (uint)(iVar9 == 0) * -0x10;
        puVar28 = *(undefined **)(puVar23 + -3);
        unaff_SI = (undefined *)*(undefined2 *)((int)puVar23 + -6);
        uVar14 = ZEXT24(unaff_SI);
        iVar29 = *(int *)(puVar23 + -1);
        uVar11 = *(uint *)((int)puVar23 + -2);
        puVar7 = *(undefined **)(puVar28 + -0x2e);
        unaff_ES = (undefined2)((ulong)puVar7 >> 0x10);
        unaff_DI = (undefined *)puVar7;
        *puVar7 = *(undefined *)(uVar11 + iVar29);
        iVar9 = *(int *)(unaff_SI + uVar11);
        *(int *)(puVar28 + -0xc) = iVar9;
      } while (iVar9 != -1);
    }
    else {
      uVar31 = (undefined2)((ulong)*(undefined4 *)(puVar28 + -0x38) >> 0x10);
      iVar9 = *(int *)(puVar28 + -10);
      iVar29 = *(int *)(puVar28 + -8);
      *(int *)(iVar29 + (int)*(undefined4 *)(puVar28 + -0x38)) = iVar9;
      *(int *)((int)puVar23 + -2) = iVar29;
      iVar29 = *(int *)(puVar28 + -0x48);
      iVar13 = *(int *)((int)puVar23 + -2);
      *(undefined2 *)(iVar13 + iVar29) = *(undefined2 *)(iVar9 + iVar29);
      piVar2 = (int *)(iVar13 + iVar29);
      *piVar2 = *piVar2 + 1;
      if (*(uint *)(puVar28 + -0xc) < *(uint *)(puVar28 + -8)) {
        iVar9 = (int)*(undefined4 *)(puVar28 + -0x40);
        *(undefined2 *)(puVar28 + -0x1e) = *(undefined2 *)(*(int *)(puVar28 + -0xc) + iVar9);
        uVar10 = *(uint *)(puVar28 + -0x1e);
        puVar1 = (uint *)(puVar28 + -0x2a);
        uVar11 = *puVar1;
        *puVar1 = *puVar1 + uVar10;
        *(uint *)(puVar28 + -0x28) = *(int *)(puVar28 + -0x28) + (uint)CARRY2(uVar11,uVar10) * 0x10;
        *(int *)(puVar28 + -0x4c) = *(int *)(puVar28 + -0x4c) + *(int *)(puVar28 + -0x1e);
        *(undefined4 *)(puVar28 + -0x2e) = *(undefined4 *)(puVar28 + -0x2a);
        uVar11 = *(uint *)(puVar28 + -0x26);
        if ((*(uint *)(puVar28 + -0x24) <= *(uint *)(puVar28 + -0x28)) &&
           ((*(uint *)(puVar28 + -0x24) < *(uint *)(puVar28 + -0x28) ||
            (uVar11 < *(uint *)(puVar28 + -0x2a))))) {
          puVar28[-0x4d] = 4;
          goto LAB_10b8_1b1f;
        }
        iVar29 = *(int *)(puVar28 + -0xc);
        *(int *)(puVar28 + -10) = iVar29;
        uVar31 = (undefined2)((ulong)*(undefined4 *)(puVar28 + -0x38) >> 0x10);
        iVar13 = *(int *)(puVar28 + -0x46);
        iVar12 = (int)*(undefined4 *)(puVar28 + -0x38);
        do {
          *(int *)((int)puVar23 + -2) = iVar29;
          *(int *)(puVar23 + -1) = iVar13;
          *(int *)((int)puVar23 + -6) = iVar12;
          *(uint *)(puVar23 + -2) = uVar11;
          *(undefined2 *)((int)puVar23 + -10) = puVar23;
          *(undefined **)(puVar23 + -3) = puVar28;
          *(int *)((int)puVar23 + -0xe) = iVar9;
          *(undefined **)(puVar23 + -4) = unaff_DI;
          piVar2 = (int *)(puVar28 + -0x2e);
          iVar9 = *piVar2;
          *piVar2 = *piVar2 + -1;
          *(uint *)(puVar28 + -0x2c) = *(int *)(puVar28 + -0x2c) + (uint)(iVar9 == 0) * -0x10;
          puVar28 = *(undefined **)(puVar23 + -3);
          iVar13 = *(int *)((int)puVar23 + -6);
          iVar29 = *(int *)(puVar23 + -1);
          puVar7 = *(undefined **)(puVar28 + -0x2e);
          unaff_ES = (undefined2)((ulong)puVar7 >> 0x10);
          unaff_DI = (undefined *)puVar7;
          iVar9 = *(int *)(puVar28 + -0xc);
          *puVar7 = *(undefined *)(iVar9 + iVar29);
          if (*(int *)(iVar9 + iVar13) == -1) break;
          *(int *)((int)puVar23 + -2) = *(int *)(iVar9 + iVar13);
          *(int *)(puVar23 + -1) = iVar29;
          *(int *)((int)puVar23 + -6) = iVar13;
          *(int *)(puVar23 + -2) = iVar9;
          *(undefined2 *)((int)puVar23 + -10) = puVar23;
          *(undefined **)(puVar23 + -3) = puVar28;
          *(int *)((int)puVar23 + -0xe) = iVar13;
          *(undefined **)(puVar23 + -4) = unaff_DI;
          piVar2 = (int *)(puVar28 + -0x2e);
          iVar9 = *piVar2;
          *piVar2 = *piVar2 + -1;
          *(uint *)(puVar28 + -0x2c) = *(int *)(puVar28 + -0x2c) + (uint)(iVar9 == 0) * -0x10;
          puVar28 = *(undefined **)(puVar23 + -3);
          iVar9 = *(int *)((int)puVar23 + -6);
          iVar13 = *(int *)(puVar23 + -1);
          uVar11 = *(uint *)((int)puVar23 + -2);
          puVar7 = *(undefined **)(puVar28 + -0x2e);
          unaff_ES = (undefined2)((ulong)puVar7 >> 0x10);
          unaff_DI = (undefined *)puVar7;
          *puVar7 = *(undefined *)(uVar11 + iVar13);
          iVar29 = *(int *)(uVar11 + iVar9);
          *(int *)(puVar28 + -0xc) = iVar29;
          iVar12 = iVar9;
        } while (iVar29 != -1);
      }
      else {
        iVar29 = (int)*(undefined4 *)(puVar28 + -0x40);
        *(undefined2 *)(puVar28 + -0x1e) = *(undefined2 *)(*(int *)(puVar28 + -10) + iVar29);
        iVar9 = *(int *)(puVar28 + -0x1e);
        puVar1 = (uint *)(puVar28 + -0x2a);
        uVar11 = *puVar1;
        *puVar1 = *puVar1 + iVar9 + 1U;
        *(uint *)(puVar28 + -0x28) =
             *(int *)(puVar28 + -0x28) + (uint)CARRY2(uVar11,iVar9 + 1U) * 0x10;
        *(int *)(puVar28 + -0x4c) = *(int *)(puVar28 + -0x4c) + *(int *)(puVar28 + -0x1e) + 1;
        iVar9 = *(int *)(puVar28 + -0x2a);
        *(uint *)(puVar28 + -0x2c) =
             ((*(int *)(puVar28 + -0x2a) != 0) - 1) * 0x10 + *(int *)(puVar28 + -0x28);
        *(int *)(puVar28 + -0x2e) = iVar9 + -1;
        uVar11 = *(uint *)(puVar28 + -0x26);
        uVar10 = *(uint *)(puVar28 + -0x2a);
        if ((*(uint *)(puVar28 + -0x24) <= *(uint *)(puVar28 + -0x28)) &&
           ((*(uint *)(puVar28 + -0x24) < *(uint *)(puVar28 + -0x28) || (uVar11 < uVar10)))) {
          puVar28[-0x4d] = 5;
          goto LAB_10b8_1b1f;
        }
        uVar31 = (undefined2)((ulong)*(undefined4 *)(puVar28 + -0x38) >> 0x10);
        iVar9 = *(int *)(puVar28 + -0x46);
        iVar13 = (int)*(undefined4 *)(puVar28 + -0x38);
        do {
          *(uint *)((int)puVar23 + -2) = uVar10;
          *(int *)(puVar23 + -1) = iVar9;
          *(int *)((int)puVar23 + -6) = iVar13;
          *(uint *)(puVar23 + -2) = uVar11;
          *(undefined2 *)((int)puVar23 + -10) = puVar23;
          *(undefined **)(puVar23 + -3) = puVar28;
          *(int *)((int)puVar23 + -0xe) = iVar29;
          *(undefined **)(puVar23 + -4) = unaff_DI;
          piVar2 = (int *)(puVar28 + -0x2e);
          iVar9 = *piVar2;
          *piVar2 = *piVar2 + -1;
          *(uint *)(puVar28 + -0x2c) = *(int *)(puVar28 + -0x2c) + (uint)(iVar9 == 0) * -0x10;
          puVar28 = *(undefined **)(puVar23 + -3);
          iVar13 = *(int *)((int)puVar23 + -6);
          iVar29 = *(int *)(puVar23 + -1);
          puVar7 = *(undefined **)(puVar28 + -0x2e);
          unaff_ES = (undefined2)((ulong)puVar7 >> 0x10);
          unaff_DI = (undefined *)puVar7;
          iVar9 = *(int *)(puVar28 + -10);
          *puVar7 = *(undefined *)(iVar9 + iVar29);
          if (*(int *)(iVar9 + iVar13) == -1) break;
          *(int *)((int)puVar23 + -2) = *(int *)(iVar9 + iVar13);
          *(int *)(puVar23 + -1) = iVar29;
          *(int *)((int)puVar23 + -6) = iVar13;
          *(int *)(puVar23 + -2) = iVar9;
          *(undefined2 *)((int)puVar23 + -10) = puVar23;
          *(undefined **)(puVar23 + -3) = puVar28;
          *(int *)((int)puVar23 + -0xe) = iVar13;
          *(undefined **)(puVar23 + -4) = unaff_DI;
          piVar2 = (int *)(puVar28 + -0x2e);
          iVar9 = *piVar2;
          *piVar2 = *piVar2 + -1;
          *(uint *)(puVar28 + -0x2c) = *(int *)(puVar28 + -0x2c) + (uint)(iVar9 == 0) * -0x10;
          puVar28 = *(undefined **)(puVar23 + -3);
          iVar29 = *(int *)((int)puVar23 + -6);
          iVar9 = *(int *)(puVar23 + -1);
          uVar11 = *(uint *)((int)puVar23 + -2);
          puVar7 = *(undefined **)(puVar28 + -0x2e);
          unaff_ES = (undefined2)((ulong)puVar7 >> 0x10);
          unaff_DI = (undefined *)puVar7;
          *puVar7 = *(undefined *)(uVar11 + iVar9);
          uVar10 = *(uint *)(uVar11 + iVar29);
          *(uint *)(puVar28 + -10) = uVar10;
          iVar13 = iVar29;
        } while (uVar10 != 0xffff);
        *(undefined2 *)(puVar28 + -10) = *(undefined2 *)(puVar28 + -0xc);
        *(undefined *)((int)*(undefined4 *)(puVar28 + -0x2a) + -1) = *unaff_DI;
      }
      unaff_SI = (undefined *)*(undefined **)(puVar28 + -0x3c);
      unaff_SI[*(int *)(puVar28 + -8)] = *unaff_DI;
      iVar9 = *(int *)(puVar28 + -8);
      *(uint *)(puVar28 + -8) = iVar9 + 2U;
      if ((*(uint *)(puVar28 + -0xe) < iVar9 + 2U) && (puVar28[-5] != '\f')) {
        puVar28[-5] = puVar28[-5] + '\x01';
        *(int *)(puVar28 + -0xe) = *(int *)(puVar28 + -0xe) * 2 + 2;
      }
    }
  } while( true );
}

