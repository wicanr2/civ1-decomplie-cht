/* CIV.EXE startup chain walk
 *
 * Walked the call graph forward from 'entry' looking for
 * the first function that calls REGISTERCLASS, LOADMENU,
 * or LOADACCELERATORS. Such a function is the application's
 * WinMain (or equivalent).
 */

// WinMain candidate: FUN_1008_0000 @ 1008:0000


/* WARNING: Stack frame is not setup normally: Input value of stackpointer is not used */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined2 __stdcall16far FUN_1008_0000(void)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  int iVar4;
  undefined2 *puVar5;
  undefined2 *puVar6;
  undefined2 *puVar7;
  undefined2 *puVar8;
  undefined2 *puVar9;
  undefined2 *puVar10;
  undefined2 *puVar11;
  undefined4 *puVar12;
  undefined4 *puVar13;
  undefined4 *puVar14;
  undefined2 *puVar16;
  undefined2 *puVar17;
  undefined2 *puVar18;
  undefined2 *puVar19;
  undefined2 *puVar20;
  undefined2 *puVar21;
  undefined2 *puVar22;
  undefined *puVar23;
  undefined2 *puVar24;
  undefined2 *puVar25;
  undefined2 *puVar26;
  undefined2 *puVar27;
  undefined2 *puVar28;
  undefined2 *puVar29;
  undefined2 *puVar30;
  undefined2 *puVar31;
  undefined2 *puVar32;
  undefined *puVar33;
  undefined *puVar35;
  undefined4 in_ESP;
  undefined2 uVar37;
  undefined4 *puVar36;
  int unaff_BP;
  undefined2 unaff_SI;
  undefined2 unaff_DI;
  undefined2 unaff_CS;
  char *pcVar38;
  undefined2 unaff_SS;
  undefined4 uVar39;
  undefined4 *puVar15;
  undefined2 *puVar34;
  
                    /* Segment:    2
                       Offset:     00006200
                       Length:     10ad
                       Min Alloc:  10ad
                       Flags:      1d50
                           Code
                           Discardable
                           Moveable
                           Preload
                           Impure (Non-shareable)
                        */
  iVar4 = (int)in_ESP;
  uVar37 = (undefined2)((ulong)in_ESP >> 0x10);
  *(int *)(iVar4 + -2) = unaff_BP + 1;
  *(undefined2 *)(iVar4 + -4) = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
  *(undefined2 *)(iVar4 + -0x11a) = unaff_SI;
  *(undefined2 *)(iVar4 + -0x11c) = unaff_DI;
  uVar3 = *(undefined2 *)(iVar4 + 0xc);
  iVar1 = *(int *)(iVar4 + 10);
  *(undefined2 *)(iVar4 + -0x11e) = unaff_CS;
  puVar5 = (undefined2 *)(iVar4 + -0x120);
  *(undefined2 *)(iVar4 + -0x120) = 0x1b;
  FUN_11e8_0000();
  DAT_1228_0002 = 0;
  puVar5[1] = (char *)s_resources__1420_11e6 + 2;
  *puVar5 = 0x2c;
  DAT_1228_0002 = FUN_1000_11bb();
  pcVar38 = (char *)s_ShowTextByName_____dialogs_c_1420_1227 + 1;
  DAT_1228_0004 = 0;
  puVar5[1] = 0x100;
  *puVar5 = (char *)s_ShowTextByName_____dialogs_c_1420_1227 + 1;
  puVar5[-1] = 4;
  puVar5[-2] = 0x1000;
  puVar6 = puVar5 + -3;
  puVar5[-3] = 0x4e;
  uVar2 = FUN_1000_2826();
  *(undefined2 *)(iVar4 + -0x16) = pcVar38;
  *(undefined2 *)(iVar4 + -0x18) = uVar2;
  *(undefined2 *)(iVar4 + -6) = 2;
  DAT_1420_54fa = uVar3;
  *(int *)((int)puVar6 + 8) = iVar1;
  *(undefined2 *)((int)puVar6 + 6) = uVar3;
  *(undefined2 *)((int)puVar6 + 4) = 0x1000;
  pcVar38 = (char *)s_nothing_1420_1093 + 5;
  puVar7 = (undefined2 *)((int)puVar6 + 2);
  *(undefined2 *)((int)puVar6 + 2) = 0x6d;
  FUN_1098_06f8();
  if (iVar1 == 0) {
    DAT_12d8_28fe = 0;
    puVar7[3] = (char *)s_Error_allocating_civilopedia_tex_1420_0ffa + 0xe;
    puVar7[2] = 0xd68;
    puVar7[1] = DAT_1420_54fa;
    *puVar7 = (char *)s_nothing_1420_1093 + 5;
    puVar9 = puVar7 + -1;
    puVar7[-1] = 0xab;
    uVar39 = MAKEPROCINSTANCE();
    DAT_1420_514a = (undefined2)((ulong)uVar39 >> 0x10);
    DAT_1420_5148 = (undefined2)uVar39;
    puVar9[3] = 0;
    puVar9[2] = 0;
    puVar9[1] = 5;
    *puVar9 = DAT_1420_514a;
    puVar9[-1] = DAT_1420_5148;
    puVar9[-2] = 0x1608;
    pcVar38 = (char *)0x1608;
    puVar8 = puVar9 + -3;
    puVar9[-3] = 0xe0;
    DAT_12d8_28fa = SETTIMER();
    if (DAT_12d8_28fa != 0) {
      puVar8[5] = DAT_1420_54fa;
      puVar8[4] = 0;
      puVar8[3] = 0x80;
      puVar8[2] = 0x1608;
      puVar10 = puVar8 + 1;
      puVar8[1] = 0x101;
      DAT_1420_54f8 = LOADACCELERATORS();
      *(undefined2 *)((int)puVar10 + 6) = 0x1608;
      puVar11 = (undefined2 *)((int)puVar10 + 4);
      *(undefined2 *)((int)puVar10 + 4) = 0x10f;
      uVar39 = GETWINFLAGS();
      *puVar11 = (int)((ulong)uVar39 >> 0x10);
      puVar11[-1] = (int)uVar39;
      puVar36 = (undefined4 *)CONCAT22(uVar37,puVar11 + -1) + 1;
      *(undefined4 *)(iVar4 + -10) = *(undefined4 *)CONCAT22(uVar37,puVar11 + -1);
      puVar12 = (undefined4 *)puVar36;
      uVar2 = (undefined2)((ulong)puVar36 >> 0x10);
      *(undefined2 *)((int)puVar12 + -2) = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
      *(undefined2 *)(puVar12 + -1) = 0xa4;
      *(undefined2 *)((int)puVar12 + -6) = 0x1608;
      puVar13 = puVar12 + -2;
      *(undefined2 *)(puVar12 + -2) = 0x120;
      uVar3 = ADDFONTRESOURCE();
      *(undefined2 *)(iVar4 + -0x14) = uVar3;
      *(undefined2 *)((int)puVar13 + 4) = 0xffff;
      *(undefined2 *)((int)puVar13 + 2) = 0x1d;
      *(undefined2 *)puVar13 = 0;
      *(undefined4 *)((undefined2 *)CONCAT22(uVar2,puVar13) + -2) = 0;
      puVar14 = (undefined4 *)((undefined2 *)CONCAT22(uVar2,puVar13) + -2);
      *(undefined2 *)((int)puVar14 + -2) = 0x1608;
      puVar15 = puVar14 + -1;
      *(undefined2 *)(puVar14 + -1) = 0x131;
      SENDMESSAGE();
      *(undefined2 *)((int)puVar15 + 10) = 0x1608;
      puVar16 = (undefined2 *)((int)puVar15 + 8);
      *(undefined2 *)((int)puVar15 + 8) = 0x136;
      FUN_1008_0e99();
      puVar16[1] = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
      *puVar16 = 0xb1;
      puVar16[-1] = 0x1608;
      puVar17 = puVar16 + -2;
      puVar16[-2] = 0x13f;
      FUN_1190_0000();
      puVar17[3] = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
      puVar17[2] = 0xbe;
      puVar17[1] = (char *)s__Taken_from_1420_1187 + 9;
      *puVar17 = 0x14b;
      FUN_1190_0000();
      puVar17[3] = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
      puVar17[2] = 0xcb;
      puVar17[1] = (char *)s__Taken_from_1420_1187 + 9;
      *puVar17 = 0x157;
      FUN_1190_0000();
      puVar17[3] = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
      puVar17[2] = 0xd8;
      puVar17[1] = (char *)s__Taken_from_1420_1187 + 9;
      *puVar17 = 0x163;
      FUN_1190_0000();
      puVar17[3] = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
      puVar17[2] = 0xe5;
      puVar17[1] = (char *)s__Taken_from_1420_1187 + 9;
      *puVar17 = 0x16f;
      FUN_1190_0000();
      *(undefined2 *)((int)puVar17 + 6) = (char *)s__Taken_from_1420_1187 + 9;
      puVar18 = (undefined2 *)((int)puVar17 + 4);
      *(undefined2 *)((int)puVar17 + 4) = 0x177;
      FUN_1008_059a();
      puVar18[1] = (char *)s__Taken_from_1420_1187 + 9;
      *puVar18 = 0x17c;
      FUN_10b8_0000();
      *(undefined2 *)(iVar4 + -0x14) = 0x1e;
      puVar18[1] = (char *)s____1_if_government_is_Despotism__1420_109b + 0x1d;
      *puVar18 = 0x186;
      FUN_1008_0b4a();
      puVar18[1] = (char *)s____1_if_government_is_Despotism__1420_109b + 0x1d;
      *puVar18 = 0x18b;
      FUN_1090_0488();
      puVar18[1] = (char *)s___with_Roads__1420_1085 + 0xb;
      *puVar18 = 400;
      FUN_11e8_0337();
      DAT_12e0_0012 = 1;
      DAT_1228_0104 = 1;
      puVar18[1] = (char *)s_resources__1420_11e6 + 2;
      *puVar18 = 0x1ad;
      FUN_11e0_0000();
      puVar18[1] = (char *)s_Cost__1420_11df + 1;
      *puVar18 = 0x1b2;
      FUN_10e8_2d46();
      *(undefined2 *)((int)_DAT_1420_2990 + 0x10) = 1;
      puVar18[1] = 1;
      *puVar18 = 1;
      puVar18[-1] = (char *)s____1_if_government_is_Republic_D_1420_10c5 + 0x23;
      puVar19 = puVar18 + -2;
      puVar18[-2] = 0x1cb;
      FUN_1150_00c9();
      puVar19[3] = unaff_SS;
      puVar19[2] = iVar4 + -0x12;
      puVar19[1] = 0;
      *puVar19 = 0;
      puVar19[-1] = 0x58;
      puVar19[-2] = 0x60;
      puVar19[-3] = (char *)s_improvement_1420_1147 + 9;
      puVar20 = puVar19 + -4;
      puVar19[-4] = 0x1e0;
      SETRECT();
      *(undefined2 *)((int)puVar20 + 0xc) = 0;
      *(undefined2 *)((int)puVar20 + 10) = 0;
      *(undefined2 *)((int)puVar20 + 8) = 0x1608;
      puVar21 = (undefined2 *)((int)puVar20 + 6);
      *(undefined2 *)((int)puVar20 + 6) = 0x1e9;
      uVar3 = FUN_1098_114a();
      puVar21[1] = uVar3;
      *puVar21 = unaff_SS;
      puVar21[-1] = iVar4 + -0x12;
      puVar21[-2] = 0x12d8;
      puVar21[-3] = (char *)s_Your_memory_resources_are_gettin_1420_4473 + 0x31;
      puVar21[-4] = (char *)s_nothing_1420_1093 + 5;
      puVar22 = puVar21 + -5;
      puVar21[-5] = 0x1fa;
      FUN_10d0_0000();
      *(undefined2 *)((int)puVar22 + 0x10) = 0x12d8;
      *(undefined2 *)((int)puVar22 + 0xe) =
           (char *)s_Your_memory_resources_are_gettin_1420_4473 + 0x31;
      *(undefined2 *)((int)puVar22 + 0xc) =
           (char *)s____1_if_government_is_Republic_D_1420_10c5 + 0xb;
      puVar23 = (undefined *)((int)puVar22 + 10);
      *(undefined2 *)((int)puVar22 + 10) = 0x208;
      FUN_10d0_0956();
      *(char **)(puVar23 + 6) = (char *)s____1_if_government_is_Republic_D_1420_10c5 + 0xb;
      puVar24 = (undefined2 *)(puVar23 + 4);
      *(undefined2 *)(puVar23 + 4) = 0x210;
      FUN_1208_0af1();
      puVar24[1] = (char *)s_Defense_Strength__1420_1204 + 4;
      *puVar24 = 0x215;
      FUN_1210_025b();
      puVar24[1] = (char *)s_Defense_Strength__1420_1204 + 0xc;
      *puVar24 = 0x21a;
      FUN_1218_0386();
      puVar24[1] = DAT_12b0_0004;
      *puVar24 = (char *)s_Movement_Rate__1420_1217 + 1;
      puVar25 = puVar24 + -1;
      puVar24[-1] = 0x229;
      SETFOCUS();
      puVar25[1] = 0x48;
      *puVar25 = 0;
      puVar25[-1] = DAT_1420_b388;
      puVar25[-2] = (undefined2 *)&DAT_1420_12b0;
      puVar25[-3] = 4;
      puVar25[-4] = 0x1608;
      puVar26 = puVar25 + -5;
      puVar25[-5] = 0x242;
      FUN_1200_00a8();
      *(undefined2 *)((int)puVar26 + 0xc) = (undefined2 *)&DAT_1420_12b0;
      *(undefined2 *)((int)puVar26 + 10) = 0x34;
      *(undefined2 *)((int)puVar26 + 8) = (char *)s_Attack_Strength__1420_11f2 + 0xe;
      pcVar38 = (char *)s_Movement_Rate__1420_1217 + 1;
      puVar27 = (undefined2 *)((int)puVar26 + 6);
      *(undefined2 *)((int)puVar26 + 6) = 0x250;
      FUN_1218_0801();
      puVar29 = puVar27 + 4;
      if (DAT_12d8_42fe == 0) {
        if (DAT_12d8_42f6 == 0) {
          puVar27[3] = 0;
          puVar27[2] = DAT_12d8_4300;
          puVar27[1] = (char *)s_Movement_Rate__1420_1217 + 1;
          pcVar38 = (char *)s_resources__1420_11e6 + 10;
          *puVar27 = 0x2c3;
          FUN_11f0_468c();
          puVar29 = (undefined2 *)((int)puVar27 + 8);
        }
      }
      else {
        if (DAT_12d8_42fe < 0) {
          DAT_12d8_42fe = 0;
        }
        puVar27[3] = 0x19;
        puVar27[2] = *(undefined2 *)((char *)s_declare_war_on_1420_391a + 4 + DAT_12d8_4300 * 2);
        puVar27[1] = DAT_12d8_4300;
        *puVar27 = (char *)s_Movement_Rate__1420_1217 + 1;
        pcVar38 = (char *)s_Defense_bonus____1420_1108 + 8;
        puVar28 = puVar27 + -1;
        puVar27[-1] = 0x2a0;
        FUN_1110_03ae();
        puVar29 = (undefined2 *)((int)puVar28 + 10);
      }
      *(undefined2 *)((int)puVar29 + -2) = pcVar38;
      puVar30 = (undefined2 *)((int)puVar29 + -4);
      *(undefined2 *)((int)puVar29 + -4) = 0x2cb;
      FUN_1048_0000();
      puVar30[1] = 0x81;
      *puVar30 = (char *)s_units__1420_1043 + 5;
      puVar31 = puVar30 + -1;
      puVar30[-1] = 0x2d3;
      FUN_1098_096c();
      DAT_12d8_24ee = 0;
      puVar31[2] = unaff_SS;
      puVar31[1] = iVar4 + -0x118;
      *puVar31 = (char *)s_nothing_1420_1093 + 5;
      puVar32 = puVar31 + -1;
      puVar31[-1] = 0x2eb;
      GETKEYBOARDSTATE();
      DAT_1420_5146 = *(undefined *)(iVar4 + -0x88);
      *(byte *)(iVar4 + -0x88) = *(byte *)(iVar4 + -0x88) | 1;
      puVar32[2] = unaff_SS;
      puVar32[1] = iVar4 + -0x118;
      *puVar32 = 0x1608;
      pcVar38 = (char *)0x1608;
      puVar34 = puVar32 + -1;
      puVar32[-1] = 0x302;
      SETKEYBOARDSTATE();
      puVar33 = (undefined *)((int)puVar34 + 6);
      while (DAT_12d8_24ee == 0) {
        *(char **)(puVar33 + -2) = pcVar38;
        pcVar38 = (char *)s___with_Roads__1420_1085 + 3;
        puVar35 = puVar33 + -4;
        *(undefined2 *)(puVar33 + -4) = 0x309;
        FUN_1088_0000();
        puVar33 = puVar35 + 4;
      }
      *(char **)(puVar33 + -2) = pcVar38;
      *(undefined2 *)(puVar33 + -4) = 0x31b;
      FUN_1008_0329();
      return 0;
    }
    puVar8[5] = 0;
    puVar8[4] = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
    puVar8[3] = 0x89;
    puVar8[2] = DAT_1420_1d40._2_2_;
    puVar8[1] = (undefined2)DAT_1420_1d40;
    *puVar8 = 0x30;
  }
  else {
    puVar7[3] = 0;
    puVar7[2] = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
    puVar7[1] = 0x56;
    *puVar7 = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
    puVar7[-1] = 0x76;
    puVar8 = puVar7 + -2;
    puVar7[-2] = 0x10;
  }
  *(undefined2 *)((int)puVar8 + -2) = pcVar38;
  *(undefined2 *)((int)puVar8 + -4) = 0x85;
  MESSAGEBOX();
  return 0;
}

