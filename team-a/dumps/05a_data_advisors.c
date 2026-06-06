/* CIV.EXE game-data init decompile (spec 02 §2.1.2 E 段)
 * task     : data_advisors
 * address  : 10e8:2d46
 * body size: 1124 bytes
 *
 * spec 02 §2.1.2 E 段對應假設角色：
 *   data_advisors
 *
 * Source: Ghidra DecompInterface (Pcode).
 * 變數名 Ghidra synthetic (uVar1/iVar2)。
 */


/* WARNING: Stack frame is not setup normally: Input value of stackpointer is not used */

void __cdecl16far FUN_10e8_2d46(void)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  undefined2 *puVar4;
  undefined *puVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  undefined *puVar8;
  undefined *puVar9;
  undefined4 *puVar10;
  undefined *puVar11;
  undefined *puVar12;
  undefined2 *puVar13;
  undefined4 *puVar14;
  undefined4 in_ESP;
  undefined2 uVar17;
  undefined4 *puVar15;
  undefined4 *puVar16;
  int unaff_BP;
  undefined2 unaff_SI;
  int iVar18;
  undefined2 unaff_DI;
  int iVar19;
  undefined2 unaff_CS;
  char *pcVar20;
  undefined2 unaff_SS;
  undefined4 uVar21;
  
  iVar3 = (int)in_ESP;
  uVar17 = (undefined2)((ulong)in_ESP >> 0x10);
  *(int *)(iVar3 + -2) = unaff_BP + 1;
  *(undefined2 *)(iVar3 + -4) = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
  *(undefined2 *)(iVar3 + -0x6a) = unaff_SI;
  *(undefined2 *)(iVar3 + -0x6c) = unaff_DI;
  *(undefined2 *)(iVar3 + -0x6e) = unaff_SS;
  *(int *)(iVar3 + -0x70) = iVar3 + -0x28;
  *(undefined2 *)(iVar3 + -0x72) = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
  *(undefined2 *)(iVar3 + -0x74) = 0x2a02;
  *(undefined2 *)(iVar3 + -0x76) = unaff_CS;
  puVar4 = (undefined2 *)(iVar3 + -0x78);
  *(undefined2 *)(iVar3 + -0x78) = (char *)s_NewPtr_____mac_c_1420_2d65 + 1;
  FUN_1000_137f();
  puVar4[5] = unaff_SS;
  puVar4[4] = iVar3 + -0x68;
  puVar4[3] = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
  puVar4[2] = 0x2a12;
  puVar4[1] = 0x1000;
  *puVar4 = (char *)s_NewPtr_____mac_c_1420_2d65 + 0x12;
  FUN_1000_137f();
  *(int *)((int)puVar4 + 10) = DAT_1420_b388;
  *(undefined2 *)((int)puVar4 + 8) = 0x1000;
  puVar5 = (undefined *)((int)puVar4 + 6);
  *(undefined2 *)((int)puVar4 + 6) = (char *)s_DisposPtr_____mac_c_1420_2d78 + 0xe;
  uVar21 = GLOBALLOCK();
  puVar14 = (undefined4 *)(puVar5 + 4);
  *(undefined2 *)(iVar3 + -10) = (int)((ulong)uVar21 >> 0x10);
  *(undefined2 *)(iVar3 + -0xc) = (int)uVar21;
  for (iVar19 = 0; iVar19 < 8; iVar19 = iVar19 + 1) {
    *(undefined4 *)(iVar3 + -0x14) = 1000000;
    *(undefined2 *)(iVar3 + -8) = 0;
    for (iVar18 = 0; iVar18 < 0x40; iVar18 = iVar18 + 1) {
      *(undefined2 *)((int)puVar14 + -2) = 0x12d0;
      *(int *)((int)puVar14 + -4) = iVar19 * 4 + 0x66;
      iVar2 = *(int *)(iVar3 + -0xc) + iVar18 * 4 + 4;
      *(undefined2 *)((int)puVar14 + -6) = *(undefined2 *)(iVar3 + -10);
      *(int *)((int)puVar14 + -8) = iVar2;
      *(undefined2 *)((int)puVar14 + -10) = 0x1608;
      puVar6 = (undefined4 *)((int)puVar14 + -0xc);
      *(undefined2 *)((int)puVar14 + -0xc) = 0x2dc2;
      uVar1 = FUN_10e8_2cc1();
      *(int *)((int)puVar6 + 2) = iVar2;
      puVar15 = (undefined4 *)CONCAT22(uVar17,puVar6);
      *(undefined2 *)puVar6 = uVar1;
      puVar16 = puVar15 + 1;
      uVar17 = (undefined2)((ulong)puVar16 >> 0x10);
      puVar14 = (undefined4 *)puVar16 + 2;
      *(undefined4 *)(iVar3 + -0x10) = *puVar15;
      if (*(ulong *)(iVar3 + -0x10) < *(ulong *)(iVar3 + -0x14)) {
        *(undefined4 *)(iVar3 + -0x14) = *(undefined4 *)(iVar3 + -0x10);
        *(int *)(iVar3 + -8) = iVar18;
      }
    }
    ((char *)s_FIRSTMOVE_1420_45fc + 2)[*(byte *)(iVar3 + iVar19 + -0x28)] =
         *(char *)(iVar3 + -8) + '\v';
  }
  for (iVar19 = 2; iVar19 < 10; iVar19 = iVar19 + 1) {
    *(undefined4 *)(iVar3 + -0x14) = 1000000;
    *(undefined2 *)(iVar3 + -8) = 0;
    for (iVar18 = 0; iVar18 < 0x40; iVar18 = iVar18 + 1) {
      *(undefined2 *)((int)puVar14 + -2) = 0x12d0;
      *(int *)((int)puVar14 + -4) = iVar19 * 4 + 0x8e;
      iVar2 = *(int *)(iVar3 + -0xc) + iVar18 * 4 + 4;
      *(undefined2 *)((int)puVar14 + -6) = *(undefined2 *)(iVar3 + -10);
      *(int *)((int)puVar14 + -8) = iVar2;
      *(undefined2 *)((int)puVar14 + -10) = 0x1608;
      puVar7 = (undefined4 *)((int)puVar14 + -0xc);
      *(undefined2 *)((int)puVar14 + -0xc) = (char *)s_Climate_1420_2e34 + 7;
      uVar1 = FUN_10e8_2cc1();
      *(int *)((int)puVar7 + 2) = iVar2;
      puVar15 = (undefined4 *)CONCAT22(uVar17,puVar7);
      *(undefined2 *)puVar7 = uVar1;
      puVar16 = puVar15 + 1;
      uVar17 = (undefined2)((ulong)puVar16 >> 0x10);
      puVar14 = (undefined4 *)puVar16 + 2;
      *(undefined4 *)(iVar3 + -0x10) = *puVar15;
      if (*(ulong *)(iVar3 + -0x10) < *(ulong *)(iVar3 + -0x14)) {
        *(undefined4 *)(iVar3 + -0x14) = *(undefined4 *)(iVar3 + -0x10);
        *(int *)(iVar3 + -8) = iVar18;
      }
    }
    ((char *)s_PILLAGE_1420_4606)[*(byte *)(iVar3 + iVar19 + -0x22)] = *(char *)(iVar3 + -8) + '\v';
  }
  if (DAT_1420_b388 != 0) {
    *(int *)((int)puVar14 + -2) = DAT_1420_b388;
    *(undefined2 *)((int)puVar14 + -4) = 0x1608;
    puVar8 = (undefined *)((int)puVar14 + -6);
    *(undefined2 *)((int)puVar14 + -6) = 0x2e99;
    GLOBALUNLOCK();
    puVar14 = (undefined4 *)(puVar8 + 4);
  }
  DAT_12d8_45be = 0;
  DAT_12d8_45c2 = 1;
  DAT_12d8_45c0 = 2;
  DAT_12d8_45c4 = 3;
  DAT_12d8_45bf = 4;
  DAT_12d8_45c3 = 5;
  DAT_12d8_45c1 = 6;
  DAT_12d8_45c5 = 7;
  DAT_12d8_45c6 = 0xf8;
  DAT_12d8_45ca = 0xf9;
  DAT_12d8_45c8 = 0xfa;
  DAT_12d8_45cc = 0xfb;
  DAT_12d8_45c7 = 0xfc;
  DAT_12d8_45cb = 0xfd;
  DAT_12d8_45c9 = 0xfe;
  DAT_12d8_45cd = 0xff;
  if (DAT_1420_17a8 == 0) {
    *(int *)((int)puVar14 + -2) = DAT_1420_b388;
    *(undefined2 *)((int)puVar14 + -4) = 0x1608;
    puVar9 = (undefined *)((int)puVar14 + -6);
    *(undefined2 *)((int)puVar14 + -6) = (char *)s_All_Files_______1420_2f63 + 5;
    uVar21 = GLOBALLOCK();
    puVar14 = (undefined4 *)(puVar9 + 4);
    *(undefined2 *)(iVar3 + -10) = (int)((ulong)uVar21 >> 0x10);
    *(undefined2 *)(iVar3 + -0xc) = (int)uVar21;
    for (iVar19 = 0; iVar19 < 0x10; iVar19 = iVar19 + 1) {
      *(undefined4 *)(iVar3 + -0x14) = 1000000;
      *(undefined2 *)(iVar3 + -8) = 0;
      for (iVar18 = 0; iVar18 < 0x40; iVar18 = iVar18 + 1) {
        *(undefined2 *)((int)puVar14 + -2) = unaff_SS;
        *(int *)((int)puVar14 + -4) = iVar19 * 4 + iVar3 + -0x68;
        iVar2 = *(int *)(iVar3 + -0xc) + iVar18 * 4 + 4;
        *(undefined2 *)((int)puVar14 + -6) = *(undefined2 *)(iVar3 + -10);
        *(int *)((int)puVar14 + -8) = iVar2;
        *(undefined2 *)((int)puVar14 + -10) = 0x1608;
        puVar10 = (undefined4 *)((int)puVar14 + -0xc);
        *(undefined2 *)((int)puVar14 + -0xc) = 0x2fa4;
        uVar1 = FUN_10e8_2cc1();
        *(int *)((int)puVar10 + 2) = iVar2;
        puVar15 = (undefined4 *)CONCAT22(uVar17,puVar10);
        *(undefined2 *)puVar10 = uVar1;
        puVar16 = puVar15 + 1;
        uVar17 = (undefined2)((ulong)puVar16 >> 0x10);
        puVar14 = (undefined4 *)puVar16 + 2;
        *(undefined4 *)(iVar3 + -0x10) = *puVar15;
        if (*(ulong *)(iVar3 + -0x10) < *(ulong *)(iVar3 + -0x14)) {
          *(undefined4 *)(iVar3 + -0x14) = *(undefined4 *)(iVar3 + -0x10);
          *(int *)(iVar3 + -8) = iVar18;
        }
      }
      ((char *)s_SetCivFont_selecting_font_into_g_1420_45cb + 0x23)[iVar19] =
           *(char *)(iVar3 + -8) + '\v';
    }
    if (DAT_1420_b388 != 0) {
      *(int *)((int)puVar14 + -2) = DAT_1420_b388;
      *(undefined2 *)((int)puVar14 + -4) = 0x1608;
      puVar11 = (undefined *)((int)puVar14 + -6);
      *(undefined2 *)((int)puVar14 + -6) = 0x2ffc;
      GLOBALUNLOCK();
      puVar14 = (undefined4 *)(puVar11 + 4);
    }
  }
  else {
    DAT_12d8_45ee = 0xff;
    DAT_12d8_45ef = 0xfb;
    DAT_12d8_45f0 = 0xf9;
    DAT_12d8_45f1 = 1;
    DAT_12d8_45f2 = 5;
    DAT_12d8_45f3 = 4;
    DAT_12d8_45f4 = 0xfc;
    DAT_12d8_45f5 = 6;
    DAT_12d8_45f6 = 0xfa;
    DAT_12d8_45f7 = 2;
    DAT_12d8_45f8 = 3;
    DAT_12d8_45f9 = 0xfb;
    DAT_12d8_45fa = 7;
    DAT_12d8_45fb = 0xf8;
    DAT_12d8_45fc = 0xf8;
    DAT_12d8_45fd = 0;
  }
  pcVar20 = (char *)0x1608;
  *(undefined2 *)(iVar3 + -6) = 0;
  while (*(int *)(iVar3 + -6) < 8) {
    *(undefined2 *)((int)puVar14 + -2) = pcVar20;
    pcVar20 = (char *)s_nothing_1420_1093 + 5;
    puVar12 = (undefined *)((int)puVar14 + -4);
    *(undefined2 *)((int)puVar14 + -4) = (char *)s_AIRNUKE_wav_1420_312b;
    FUN_1098_0157();
    puVar14 = (undefined4 *)(puVar12 + 4);
    iVar19 = *(int *)(iVar3 + -6);
    *(undefined2 *)(iVar3 + -0x16) = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
    *(int *)(iVar3 + -0x18) = iVar19 * 0x40 + -0x6c30;
    for (iVar19 = 0; iVar19 < 0x1c; iVar19 = iVar19 + 1) {
      *(undefined2 *)((int)puVar14 + -2) =
           (int)CONCAT31(0x12,((char *)s_SetCivFont_selecting_font_into_g_1420_45cb + 0x13)
                              [*(int *)(iVar3 + -6)]);
      *(undefined2 *)((int)puVar14 + -4) = (int)CONCAT31(0x12,DAT_12d8_44fa);
      *(undefined2 *)((int)puVar14 + -6) = *(undefined2 *)*(undefined4 *)(iVar3 + -0x18);
      *(undefined2 *)((int)puVar14 + -8) = pcVar20;
      puVar13 = (undefined2 *)((int)puVar14 + -10);
      *(undefined2 *)((int)puVar14 + -10) = (char *)s_S_BEEP_wav_1420_315a + 0xb;
      FUN_11a8_022f();
      puVar13[4] = (int)CONCAT31(0x12,((char *)s_SetCivFont_selecting_font_into_g_1420_45cb + 3)
                                      [*(int *)(iVar3 + -6)]);
      puVar13[3] = (int)CONCAT31(0x12,DAT_12d8_44f9);
      puVar13[2] = *(undefined2 *)*(undefined4 *)(iVar3 + -0x18);
      puVar13[1] = (char *)s_shields__1420_11a7 + 1;
      pcVar20 = (char *)s_shields__1420_11a7 + 1;
      *puVar13 = (char *)s_Which_section_shall_we_improve__1420_317d + 0xe;
      FUN_11a8_022f();
      puVar14 = (undefined4 *)((int)puVar13 + 10);
      *(int *)(iVar3 + -0x18) = *(int *)(iVar3 + -0x18) + 2;
    }
    *(int *)(iVar3 + -6) = *(int *)(iVar3 + -6) + 1;
  }
  return;
}

