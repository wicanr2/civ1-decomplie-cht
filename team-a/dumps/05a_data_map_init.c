/* CIV.EXE game-data init decompile (spec 02 §2.1.2 E 段)
 * task     : data_map_init
 * address  : 11e0:0000
 * body size: 545 bytes
 *
 * spec 02 §2.1.2 E 段對應假設角色：
 *   data_map_init
 *
 * Source: Ghidra DecompInterface (Pcode).
 * 變數名 Ghidra synthetic (uVar1/iVar2)。
 */


/* WARNING: Stack frame is not setup normally: Input value of stackpointer is not used */

void __cdecl16far FUN_11e0_0000(void)

{
  char cVar1;
  char *pcVar2;
  int iVar3;
  undefined *puVar4;
  undefined2 *puVar5;
  undefined2 *puVar6;
  undefined4 *puVar7;
  undefined4 *puVar8;
  undefined2 *puVar9;
  undefined2 *puVar10;
  undefined2 *puVar11;
  undefined *puVar12;
  undefined2 *puVar13;
  undefined2 *puVar14;
  int *piVar15;
  int *piVar16;
  int *piVar17;
  undefined *puVar18;
  undefined2 *puVar19;
  undefined2 *puVar20;
  undefined *puVar21;
  undefined *puVar22;
  undefined *puVar23;
  undefined2 *puVar24;
  undefined4 in_ESP;
  undefined2 uVar27;
  undefined4 *puVar25;
  int *piVar26;
  int unaff_BP;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  
                    /* Segment:    61
                       Offset:     0007f600
                       Length:     0c84
                       Min Alloc:  0c84
                       Flags:      1d50
                           Code
                           Discardable
                           Moveable
                           Preload
                           Impure (Non-shareable)
                        */
  iVar3 = (int)in_ESP;
  uVar27 = (undefined2)((ulong)in_ESP >> 0x10);
  *(int *)(iVar3 + -2) = unaff_BP + 1;
  *(undefined2 *)(iVar3 + -4) = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
  *(undefined2 *)(iVar3 + -0x1c) = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
  *(undefined2 *)(iVar3 + -0x1e) = (char *)s_CIVDIALOG_1420_4342;
  *(undefined2 *)(iVar3 + -0x20) = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
  *(undefined2 *)(iVar3 + -0x22) = 0x434c;
  *(undefined2 *)(iVar3 + -0x24) = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
  *(undefined2 *)(iVar3 + -0x26) = 0x434e;
  *(undefined2 *)(iVar3 + -0x28) = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
  *(undefined2 *)(iVar3 + -0x2a) = (char *)s_CIVTIMES24_1420_4350;
  *(undefined4 *)(iVar3 + -0x1a) = 0;
  *(undefined2 *)(iVar3 + -0x2c) = unaff_SS;
  *(int *)(iVar3 + -0x2e) = iVar3 + -8;
  *(undefined2 *)(iVar3 + -0x30) = 0x41;
  *(undefined2 *)(iVar3 + -0x32) = 0x94;
  *(undefined2 *)(iVar3 + -0x34) = unaff_CS;
  puVar4 = (undefined *)(iVar3 + -0x36);
  *(undefined2 *)(iVar3 + -0x36) = 0x44;
  cVar1 = FUN_1098_124f();
  *(int *)(iVar3 + -0xe) = (int)cVar1;
  *(undefined2 *)(puVar4 + 10) = *(undefined2 *)(iVar3 + -6);
  *(undefined2 *)(puVar4 + 8) = *(undefined2 *)(iVar3 + -8);
  *(char **)(puVar4 + 6) = (char *)s_nothing_1420_1093 + 5;
  puVar5 = (undefined2 *)(puVar4 + 4);
  *(undefined2 *)(puVar4 + 4) = 0x56;
  FUN_1098_1104();
  puVar5[3] = 0;
  puVar5[2] = 5;
  puVar5[1] = 0;
  *puVar5 = (undefined2 *)&DAT_1420_12b0;
  puVar5[-1] = 0x5a;
  puVar5[-2] = (char *)s_nothing_1420_1093 + 5;
  puVar6 = puVar5 + -3;
  puVar5[-3] = 0x6a;
  FUN_1090_0376();
  puVar6[6] = unaff_SS;
  puVar6[5] = iVar3 + -0xc;
  puVar6[4] = (char *)s_Cost__1420_11df + 1;
  puVar6[3] = 0x221;
  puVar6[2] = (char *)s_Confirm_spaceship_launch____1420_13b6 + 0x1a;
  puVar6[1] = 0;
  *puVar6 = 0;
  puVar25 = (undefined4 *)((undefined2 *)CONCAT22(uVar27,puVar6) + -2);
  *puVar25 = 0;
  puVar7 = (undefined4 *)puVar25;
  uVar27 = (undefined2)((ulong)puVar25 >> 0x10);
  *(undefined2 *)((int)puVar7 + -2) = *(undefined2 *)(iVar3 + -6);
  *(undefined2 *)(puVar7 + -1) = *(undefined2 *)(iVar3 + -8);
  *(undefined2 *)((int)puVar7 + -6) = (char *)s___with_Roads__1420_1085 + 0xb;
  puVar8 = puVar7 + -2;
  *(undefined2 *)(puVar7 + -2) = 0xa5;
  FUN_10b0_008a();
  DAT_13e0_001e = 0;
  *(undefined2 *)((int)puVar8 + 0x18) = 0;
  *(undefined2 *)((int)puVar8 + 0x16) = 3;
  *(undefined2 *)((int)puVar8 + 0x14) = (char *)s____1_if_government_is_Despotism__1420_109b + 0x15;
  puVar9 = (undefined2 *)((int)puVar8 + 0x12);
  *(undefined2 *)((int)puVar8 + 0x12) = 0xbd;
  FUN_1150_00c9();
  puVar9[3] = *(undefined2 *)*(undefined4 *)(iVar3 + -8);
  puVar9[2] = unaff_SS;
  puVar9[1] = iVar3 + -0x16;
  *puVar9 = (char *)s_improvement_1420_1147 + 9;
  puVar10 = puVar9 + -1;
  puVar9[-1] = 0xd0;
  GETCLIENTRECT();
  puVar10[3] = *(undefined2 *)((int)*(undefined4 *)(iVar3 + -8) + 2);
  puVar10[2] = unaff_SS;
  puVar10[1] = iVar3 + -0x16;
  *puVar10 = DAT_1420_17a2;
  puVar10[-1] = 0x1608;
  puVar11 = puVar10 + -2;
  puVar10[-2] = 0xeb;
  FILLRECT();
  *(undefined2 *)((int)puVar11 + 8) = 0x83;
  *(undefined2 *)((int)puVar11 + 6) = 0x1608;
  pcVar2 = (char *)s_nothing_1420_1093 + 5;
  puVar12 = (undefined *)((int)puVar11 + 4);
  *(undefined2 *)((int)puVar11 + 4) = 0xf3;
  FUN_1098_096c();
  puVar13 = (undefined2 *)(puVar12 + 6);
  do {
    *(undefined2 *)((int)puVar13 + -2) = *(undefined2 *)(iVar3 + -10);
    *(undefined2 *)((int)puVar13 + -4) = *(undefined2 *)(iVar3 + -0xc);
    *(undefined2 *)((int)puVar13 + -6) = pcVar2;
    pcVar2 = (char *)s____1_if_government_is_Despotism__1420_109b + 0x15;
    puVar14 = (undefined2 *)((int)puVar13 + -8);
    *(undefined2 *)((int)puVar13 + -8) = 0x101;
    cVar1 = FUN_10b0_0461();
    puVar13 = puVar14 + 4;
  } while (cVar1 == '\0');
  puVar14[3] = *(undefined2 *)(iVar3 + -10);
  puVar14[2] = *(undefined2 *)(iVar3 + -0xc);
  puVar14[1] = (char *)s____1_if_government_is_Despotism__1420_109b + 0x15;
  *puVar14 = 0x114;
  FUN_10b0_0287();
  puVar14[3] = 0;
  puVar14[2] = 1;
  puVar14[1] = (char *)s____1_if_government_is_Despotism__1420_109b + 0x15;
  *puVar14 = 0x120;
  FUN_1150_00c9();
  if (((DAT_12d8_44e2 == 0) || (DAT_12d8_44e2 == 2)) || (DAT_12d8_44e2 == 3)) {
    DAT_13e0_001e = 0;
    puVar14[3] = (char *)s_improvement_1420_1147 + 9;
    piVar15 = puVar14 + 2;
    puVar14[2] = 0x15e;
    FUN_11f0_4ea0();
    piVar15[1] = unaff_SS;
    *piVar15 = iVar3 + -0xc;
    piVar15[-1] = (int)((char *)s_Cost__1420_11df + 1);
    piVar15[-2] = 0x91e;
    if (DAT_12d8_28fc == 0) {
      pcVar2 = (char *)s___chance_of_success__No_Launch_L_1420_13d2 + 6;
    }
    else {
      pcVar2 = (char *)s___chance_of_success__No_Launch_L_1420_13d2 + 0xe;
    }
    piVar15[-3] = (int)pcVar2;
    piVar15[-4] = 0;
    piVar15[-5] = 0;
    piVar26 = (int *)CONCAT22(uVar27,piVar15 + -5) + -2;
    piVar26[0] = 0;
    piVar26[1] = 0;
    piVar16 = (int *)piVar26;
    piVar16[-1] = *(undefined2 *)(iVar3 + -6);
    piVar16[-2] = *(undefined2 *)(iVar3 + -8);
    piVar16[-3] = (int)((char *)s_resources__1420_11e6 + 10);
    piVar17 = piVar16 + -4;
    piVar16[-4] = 0x196;
    FUN_10b0_008a();
    *(undefined4 *)((int)piVar17 + 0x16) = 1000;
    *(undefined2 *)((int)piVar17 + 0x14) = *(undefined2 *)(iVar3 + -10);
    *(undefined2 *)((int)piVar17 + 0x12) = *(undefined2 *)(iVar3 + -0xc);
    *(undefined2 *)((int)piVar17 + 0x10) =
         (char *)s____1_if_government_is_Despotism__1420_109b + 0x15;
    puVar18 = (undefined *)((int)piVar17 + 0xe);
    *(undefined2 *)((int)piVar17 + 0xe) = 0x1aa;
    FUN_10b0_005b();
    *(char **)(puVar18 + 10) = (char *)s____1_if_government_is_Despotism__1420_109b + 0x15;
    puVar19 = (undefined2 *)(puVar18 + 8);
    *(undefined2 *)(puVar18 + 8) = 0x1b2;
    FUN_1108_01bc();
    puVar19[1] = 0x81;
    *puVar19 = (char *)s_Defense_bonus____1420_1108;
    pcVar2 = (char *)s_nothing_1420_1093 + 5;
    puVar20 = puVar19 + -1;
    puVar19[-1] = 0x1ba;
    FUN_1098_096c();
    puVar21 = (undefined *)((int)puVar20 + 6);
    do {
      puVar23 = puVar21;
      if (DAT_13e0_001e != 0) break;
      *(undefined2 *)(puVar21 + -2) = *(undefined2 *)(iVar3 + -10);
      *(undefined2 *)(puVar21 + -4) = *(undefined2 *)(iVar3 + -0xc);
      *(char **)(puVar21 + -6) = pcVar2;
      pcVar2 = (char *)s____1_if_government_is_Despotism__1420_109b + 0x15;
      puVar22 = puVar21 + -8;
      *(undefined2 *)(puVar21 + -8) = 0x1d5;
      cVar1 = FUN_10b0_0461();
      puVar23 = puVar22 + 8;
      puVar21 = puVar22 + 8;
    } while (cVar1 == '\0');
    *(undefined2 *)(puVar23 + -2) = *(undefined2 *)(iVar3 + -10);
    *(undefined2 *)(puVar23 + -4) = *(undefined2 *)(iVar3 + -0xc);
    *(char **)(puVar23 + -6) = pcVar2;
    puVar24 = (undefined2 *)(puVar23 + -8);
    *(undefined2 *)(puVar23 + -8) = 0x1e8;
    FUN_10b0_0287();
    puVar24[3] = 0;
    puVar24[2] = 1;
    puVar24[1] = (char *)s____1_if_government_is_Despotism__1420_109b + 0x15;
    *puVar24 = 500;
    FUN_1150_00c9();
    puVar24[3] = *(undefined2 *)(iVar3 + -6);
    puVar24[2] = *(undefined2 *)(iVar3 + -8);
    puVar24[1] = (char *)s_improvement_1420_1147 + 9;
    *puVar24 = 0x202;
    FUN_1098_1612();
    *(undefined2 *)((int)puVar24 + 6) = (char *)s_nothing_1420_1093 + 5;
    *(undefined2 *)((int)puVar24 + 4) = 0x20a;
    FUN_10e8_0000();
  }
  else {
    puVar14[3] = *(undefined2 *)(iVar3 + -6);
    puVar14[2] = *(undefined2 *)(iVar3 + -8);
    puVar14[1] = (char *)s_improvement_1420_1147 + 9;
    *puVar14 = 0x217;
    FUN_1098_1612();
  }
  return;
}

