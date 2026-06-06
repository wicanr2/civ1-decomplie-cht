/* CIV.EXE game-data init decompile (spec 02 §2.1.2 E 段)
 * task     : data_tech
 * address  : 10b8:0000
 * body size: 1244 bytes
 *
 * spec 02 §2.1.2 E 段對應假設角色：
 *   data_tech
 *
 * Source: Ghidra DecompInterface (Pcode).
 * 變數名 Ghidra synthetic (uVar1/iVar2)。
 */


/* WARNING: Stack frame is not setup normally: Input value of stackpointer is not used */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_10b8_0000(void)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 uVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  int *piVar7;
  undefined2 *puVar8;
  int *piVar9;
  int *piVar10;
  undefined2 *puVar11;
  undefined4 *puVar12;
  undefined2 *puVar14;
  undefined2 *puVar15;
  undefined2 *puVar16;
  undefined2 *puVar17;
  undefined2 *puVar18;
  undefined2 *puVar19;
  undefined2 *puVar20;
  undefined2 *puVar21;
  undefined2 *puVar22;
  undefined2 *puVar23;
  undefined2 *puVar24;
  undefined2 *puVar25;
  undefined2 *puVar26;
  undefined2 *puVar27;
  undefined2 *puVar28;
  undefined2 *puVar29;
  undefined2 *puVar30;
  undefined2 *puVar31;
  undefined2 *puVar32;
  undefined2 *puVar33;
  undefined2 *puVar34;
  undefined2 *puVar35;
  undefined2 *puVar36;
  undefined2 *puVar37;
  undefined2 *puVar38;
  undefined2 *puVar39;
  undefined2 *puVar40;
  undefined2 *puVar41;
  undefined2 *puVar42;
  undefined2 *puVar43;
  undefined2 *puVar44;
  undefined2 *puVar45;
  undefined2 *puVar46;
  undefined2 *puVar47;
  undefined2 *puVar48;
  undefined2 *puVar49;
  undefined2 *puVar50;
  undefined2 *puVar51;
  undefined *puVar52;
  undefined2 *puVar53;
  undefined2 *puVar54;
  undefined2 *puVar55;
  undefined2 *puVar56;
  undefined4 in_ESP;
  int unaff_BP;
  undefined2 unaff_SI;
  char *pcVar57;
  undefined2 unaff_DI;
  int iVar58;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  undefined4 *puVar13;
  
                    /* Segment:    24
                       Offset:     00040800
                       Length:     1b4e
                       Min Alloc:  1b4e
                       Flags:      1d50
                           Code
                           Discardable
                           Moveable
                           Preload
                           Impure (Non-shareable)
                        */
  iVar5 = (int)in_ESP;
  uVar3 = (undefined2)((ulong)in_ESP >> 0x10);
  *(int *)(iVar5 + -2) = unaff_BP + 1;
  *(undefined2 *)(iVar5 + -4) = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
  *(undefined2 *)(iVar5 + -0x41c) = unaff_SI;
  *(undefined2 *)(iVar5 + -0x41e) = unaff_DI;
  if (DAT_1420_17a8 != 0) {
    *(undefined2 *)(iVar5 + -0x420) = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
    *(undefined2 *)(iVar5 + -0x422) = 0x1f54;
    *(undefined2 *)(iVar5 + -0x424) = 0;
    *(undefined2 *)(iVar5 + -0x426) = unaff_CS;
    piVar6 = (int *)(iVar5 + -0x428);
    *(undefined2 *)(iVar5 + -0x428) = 0x2b;
    uVar1 = _LOPEN();
    *(undefined2 *)(iVar5 + -6) = uVar1;
    if (*(int *)(iVar5 + -6) == -1) {
      piVar6[3] = (int)((char *)s_Type__Propulsion_Fuel_1420_1416 + 10);
      piVar6[2] = (int)(char *)s_You_are_currently_running_a_16_c_1420_1f5d;
      piVar6[1] = unaff_SS;
      *piVar6 = iVar5 + -0x41a;
      piVar6[-1] = 0x1608;
      piVar7 = piVar6 + -2;
      piVar6[-2] = 0x4e;
      FUN_1000_36b2();
      piVar7[5] = (int)((char *)s_Type__Propulsion_Fuel_1420_1416 + 10);
      piVar7[4] = (int)(char *)s_The_game_artwork_must_be_convert_1420_1f8c;
      piVar7[3] = unaff_SS;
      piVar7[2] = iVar5 + -0x41a;
      piVar7[1] = 0x1000;
      *piVar7 = 0x60;
      FUN_1000_363e();
      piVar7[5] = (int)((char *)s_Type__Propulsion_Fuel_1420_1416 + 10);
      piVar7[4] = (int)(char *)s_properly__You_have_the_option_to_1420_1fbb;
      piVar7[3] = unaff_SS;
      piVar7[2] = iVar5 + -0x41a;
      piVar7[1] = 0x1000;
      *piVar7 = 0x72;
      FUN_1000_363e();
      piVar7[5] = (int)((char *)s_Type__Propulsion_Fuel_1420_1416 + 10);
      piVar7[4] = (int)(char *)s_all_now_or_to_wait_and_create_th_1420_1feb;
      piVar7[3] = unaff_SS;
      piVar7[2] = iVar5 + -0x41a;
      piVar7[1] = 0x1000;
      *piVar7 = 0x84;
      FUN_1000_363e();
      piVar7[5] = (int)((char *)s_Type__Propulsion_Fuel_1420_1416 + 10);
      piVar7[4] = (int)(char *)s_game_play__This_could_take_a_whi_1420_201e;
      piVar7[3] = unaff_SS;
      piVar7[2] = iVar5 + -0x41a;
      piVar7[1] = 0x1000;
      *piVar7 = 0x96;
      FUN_1000_363e();
      piVar7[5] = (int)((char *)s_Type__Propulsion_Fuel_1420_1416 + 10);
      piVar7[4] = (int)(char *)s_Select_YES_to_create_them_now__1420_2046;
      piVar7[3] = unaff_SS;
      piVar7[2] = iVar5 + -0x41a;
      piVar7[1] = 0x1000;
      *piVar7 = 0xa8;
      FUN_1000_363e();
      piVar7[5] = (int)((char *)s_Type__Propulsion_Fuel_1420_1416 + 10);
      piVar7[4] = (int)(char *)s_Select_NO_to_continue_the_game__1420_2068;
      piVar7[3] = unaff_SS;
      piVar7[2] = iVar5 + -0x41a;
      piVar7[1] = 0x1000;
      *piVar7 = 0xba;
      FUN_1000_363e();
      piVar7[5] = (int)((char *)s_Type__Propulsion_Fuel_1420_1416 + 10);
      piVar7[4] = (int)(char *)s_Select_CANCEL_to_quit_now__1420_208b;
      piVar7[3] = unaff_SS;
      piVar7[2] = iVar5 + -0x41a;
      piVar7[1] = 0x1000;
      *piVar7 = 0xcc;
      FUN_1000_363e();
      piVar7[5] = 0;
      piVar7[4] = unaff_SS;
      piVar7[3] = iVar5 + -0x41a;
      piVar7[2] = (int)((char *)s_Type__Propulsion_Fuel_1420_1416 + 10);
      piVar7[1] = (int)(char *)s_CONVERT_CIVILIZATION_ARTWORK__1420_20a9;
      *piVar7 = 0x43;
      piVar7[-1] = 0x1000;
      puVar8 = piVar7 + -2;
      piVar7[-2] = 0xe2;
      uVar1 = MESSAGEBOX();
      *(undefined2 *)(iVar5 + -8) = uVar1;
      if (*(int *)(iVar5 + -8) != 7) {
        if (*(int *)(iVar5 + -8) == 2) {
          *(undefined2 *)((int)puVar8 + 0xc) = 0x1608;
          *(undefined2 *)((int)puVar8 + 10) = 0xfc;
          FUN_1008_0329();
        }
        else {
          *(undefined2 *)((int)puVar8 + 0xc) = 0x1608;
          piVar9 = (int *)((int)puVar8 + 10);
          *(undefined2 *)((int)puVar8 + 10) = 0x104;
          iVar2 = FUN_10b8_04df();
          if (iVar2 != 0) {
            piVar9[1] = unaff_SS;
            *piVar9 = iVar5 + -0xe;
            piVar9[-1] = 0x41;
            piVar9[-2] = 0x29a;
            piVar9[-3] = 0x1608;
            piVar10 = piVar9 + -4;
            piVar9[-4] = 0x11a;
            FUN_1098_124f();
            DAT_1420_9986 = 1;
            _DAT_1420_9982 = *(undefined4 *)(iVar5 + -0xe);
            *(undefined2 *)((int)piVar10 + 10) = *(undefined2 *)*(undefined4 *)(iVar5 + -0xe);
            *(undefined2 *)((int)piVar10 + 8) = 0x67;
            *(undefined2 *)((int)piVar10 + 6) = (char *)s_nothing_1420_1093 + 5;
            puVar11 = (undefined2 *)((int)piVar10 + 4);
            *(undefined2 *)((int)piVar10 + 4) = 0x138;
            uVar1 = GETDLGITEM();
            *(undefined2 *)(iVar5 + -0x18) = uVar1;
            puVar11[2] = *(undefined2 *)(iVar5 + -0x18);
            puVar11[1] = 0x30;
            *puVar11 = DAT_1420_54a6;
            *(undefined4 *)((undefined2 *)CONCAT22(uVar3,puVar11) + -2) = 0;
            puVar12 = (undefined4 *)((undefined2 *)CONCAT22(uVar3,puVar11) + -2);
            *(undefined2 *)((int)puVar12 + -2) = 0x1608;
            puVar13 = puVar12 + -1;
            *(undefined2 *)(puVar12 + -1) = 0x152;
            SENDMESSAGE();
            *(undefined2 *)((int)puVar13 + 10) = *(undefined2 *)(iVar5 + -0x18);
            *(undefined2 *)((int)puVar13 + 8) = 5;
            *(undefined2 *)((int)puVar13 + 6) = 0x1608;
            puVar14 = (undefined2 *)((int)puVar13 + 4);
            *(undefined2 *)((int)puVar13 + 4) = 0x15c;
            SHOWWINDOW();
            puVar14[2] = *(undefined2 *)*(undefined4 *)(iVar5 + -0xe);
            puVar14[1] = 0x66;
            *puVar14 = 0x1608;
            puVar15 = puVar14 + -1;
            puVar14[-1] = 0x169;
            uVar3 = GETDLGITEM();
            *(undefined2 *)(iVar5 + -0x18) = uVar3;
            puVar15[2] = *(undefined2 *)(iVar5 + -0x18);
            puVar15[1] = 5;
            *puVar15 = 0x1608;
            puVar16 = puVar15 + -1;
            puVar15[-1] = 0x176;
            SHOWWINDOW();
            puVar16[2] = *(undefined2 *)(iVar5 + -0xc);
            puVar16[1] = *(undefined2 *)(iVar5 + -0xe);
            *puVar16 = 0x1608;
            puVar17 = puVar16 + -1;
            puVar16[-1] = 0x181;
            FUN_10b8_06a0();
            *(undefined4 *)(puVar17 + 2) = 0xff0000;
            puVar17[1] = 0x1608;
            *puVar17 = 399;
            uVar3 = CREATESOLIDBRUSH();
            *(undefined2 *)(iVar5 + -0x1a) = uVar3;
            puVar17[2] = unaff_SS;
            puVar17[1] = iVar5 + -0x16;
            *puVar17 = 0x32;
            puVar17[-1] = 0x32;
            puVar17[-2] = 0xfa;
            puVar17[-3] = 0x50;
            puVar17[-4] = 0x1608;
            puVar18 = puVar17 + -5;
            puVar17[-5] = 0x1a5;
            SETRECT();
            *(undefined2 *)((int)puVar18 + 0xc) =
                 *(undefined2 *)((int)*(undefined4 *)(iVar5 + -0xe) + 2);
            *(undefined2 *)((int)puVar18 + 10) = unaff_SS;
            *(int *)((int)puVar18 + 8) = iVar5 + -0x16;
            *(undefined2 *)((int)puVar18 + 6) = 4;
            *(undefined2 *)((int)puVar18 + 4) = 0x1608;
            puVar19 = (undefined2 *)((int)puVar18 + 2);
            *(undefined2 *)((int)puVar18 + 2) = 0x1b8;
            uVar3 = GETSTOCKOBJECT();
            puVar19[1] = uVar3;
            *puVar19 = 0x1608;
            puVar20 = puVar19 + -1;
            puVar19[-1] = 0x1be;
            FRAMERECT();
            puVar20[4] = unaff_SS;
            puVar20[3] = iVar5 + -0x16;
            puVar20[2] = 0xffff;
            puVar20[1] = 0xffff;
            *puVar20 = 0x1608;
            puVar21 = puVar20 + -1;
            puVar20[-1] = 0x1cc;
            INFLATERECT();
            puVar52 = (undefined *)((int)puVar21 + 10);
            *(undefined2 *)(iVar5 + -10) = 0x14a;
            iVar58 = 0;
            for (iVar2 = 200; iVar2 < 0xcb; iVar2 = iVar2 + 1) {
              *(int *)(puVar52 + -2) = iVar2;
              *(undefined2 *)(puVar52 + -4) = 0x1608;
              puVar22 = (undefined2 *)(puVar52 + -6);
              *(undefined2 *)(puVar52 + -6) = 0x1de;
              FUN_10b8_076a();
              iVar58 = iVar58 + 1;
              puVar22[2] = *(undefined2 *)(iVar5 + -0xc);
              puVar22[1] = *(undefined2 *)(iVar5 + -0xe);
              *puVar22 = 0x1608;
              puVar23 = puVar22 + -1;
              puVar22[-1] = 0x1eb;
              iVar4 = FUN_10b8_06a0();
              puVar56 = puVar23 + 4;
              if (iVar4 == 1) goto LAB_10b8_04b3;
              puVar23[3] = *(undefined2 *)(iVar5 + -0x1a);
              puVar23[2] = iVar58;
              puVar23[1] = *(undefined2 *)(iVar5 + -10);
              *puVar23 = unaff_SS;
              puVar23[-1] = iVar5 + -0x16;
              puVar23[-2] = *(undefined2 *)(iVar5 + -0xc);
              puVar23[-3] = *(undefined2 *)(iVar5 + -0xe);
              puVar23[-4] = 0x1608;
              puVar24 = puVar23 + -5;
              puVar23[-5] = 0x20d;
              FUN_10b8_062d();
              puVar52 = (undefined *)((int)puVar24 + 0x12);
            }
            for (iVar2 = 0x44c; iVar2 < 0x515; iVar2 = iVar2 + 1) {
              *(int *)(puVar52 + -2) = iVar2;
              *(undefined2 *)(puVar52 + -4) = 0x1608;
              puVar25 = (undefined2 *)(puVar52 + -6);
              *(undefined2 *)(puVar52 + -6) = 0x222;
              FUN_10b8_076a();
              iVar58 = iVar58 + 1;
              puVar25[2] = *(undefined2 *)(iVar5 + -0xc);
              puVar25[1] = *(undefined2 *)(iVar5 + -0xe);
              *puVar25 = 0x1608;
              puVar26 = puVar25 + -1;
              puVar25[-1] = 0x22f;
              iVar4 = FUN_10b8_06a0();
              puVar56 = puVar26 + 4;
              if (iVar4 == 1) goto LAB_10b8_04b3;
              puVar26[3] = *(undefined2 *)(iVar5 + -0x1a);
              puVar26[2] = iVar58;
              puVar26[1] = *(undefined2 *)(iVar5 + -10);
              *puVar26 = unaff_SS;
              puVar26[-1] = iVar5 + -0x16;
              puVar26[-2] = *(undefined2 *)(iVar5 + -0xc);
              puVar26[-3] = *(undefined2 *)(iVar5 + -0xe);
              puVar26[-4] = 0x1608;
              puVar27 = puVar26 + -5;
              puVar26[-5] = 0x251;
              FUN_10b8_062d();
              puVar52 = (undefined *)((int)puVar27 + 0x12);
            }
            for (iVar2 = 0x80; iVar2 < 0x91; iVar2 = iVar2 + 1) {
              *(int *)(puVar52 + -2) = iVar2;
              *(undefined2 *)(puVar52 + -4) = 0x1608;
              puVar28 = (undefined2 *)(puVar52 + -6);
              *(undefined2 *)(puVar52 + -6) = 0x266;
              FUN_10b8_076a();
              iVar58 = iVar58 + 1;
              puVar28[2] = *(undefined2 *)(iVar5 + -0xc);
              puVar28[1] = *(undefined2 *)(iVar5 + -0xe);
              *puVar28 = 0x1608;
              puVar29 = puVar28 + -1;
              puVar28[-1] = 0x273;
              iVar4 = FUN_10b8_06a0();
              puVar56 = puVar29 + 4;
              if (iVar4 == 1) goto LAB_10b8_04b3;
              puVar29[3] = *(undefined2 *)(iVar5 + -0x1a);
              puVar29[2] = iVar58;
              puVar29[1] = *(undefined2 *)(iVar5 + -10);
              *puVar29 = unaff_SS;
              puVar29[-1] = iVar5 + -0x16;
              puVar29[-2] = *(undefined2 *)(iVar5 + -0xc);
              puVar29[-3] = *(undefined2 *)(iVar5 + -0xe);
              puVar29[-4] = 0x1608;
              puVar30 = puVar29 + -5;
              puVar29[-5] = 0x295;
              FUN_10b8_062d();
              puVar52 = (undefined *)((int)puVar30 + 0x12);
            }
            for (iVar2 = 500; iVar2 < 0x202; iVar2 = iVar2 + 1) {
              *(int *)(puVar52 + -2) = iVar2;
              *(undefined2 *)(puVar52 + -4) = 0x1608;
              puVar31 = (undefined2 *)(puVar52 + -6);
              *(undefined2 *)(puVar52 + -6) = 0x2aa;
              FUN_10b8_076a();
              iVar58 = iVar58 + 1;
              puVar31[2] = *(undefined2 *)(iVar5 + -0xc);
              puVar31[1] = *(undefined2 *)(iVar5 + -0xe);
              *puVar31 = 0x1608;
              puVar32 = puVar31 + -1;
              puVar31[-1] = 0x2b7;
              iVar4 = FUN_10b8_06a0();
              puVar56 = puVar32 + 4;
              if (iVar4 == 1) goto LAB_10b8_04b3;
              puVar32[3] = *(undefined2 *)(iVar5 + -0x1a);
              puVar32[2] = iVar58;
              puVar32[1] = *(undefined2 *)(iVar5 + -10);
              *puVar32 = unaff_SS;
              puVar32[-1] = iVar5 + -0x16;
              puVar32[-2] = *(undefined2 *)(iVar5 + -0xc);
              puVar32[-3] = *(undefined2 *)(iVar5 + -0xe);
              puVar32[-4] = 0x1608;
              puVar33 = puVar32 + -5;
              puVar32[-5] = 0x2d9;
              FUN_10b8_062d();
              puVar52 = (undefined *)((int)puVar33 + 0x12);
            }
            for (iVar2 = 400; iVar2 < 0x197; iVar2 = iVar2 + 1) {
              *(int *)(puVar52 + -2) = iVar2;
              *(undefined2 *)(puVar52 + -4) = 0x1608;
              puVar34 = (undefined2 *)(puVar52 + -6);
              *(undefined2 *)(puVar52 + -6) = 0x2ee;
              FUN_10b8_076a();
              iVar58 = iVar58 + 1;
              puVar34[2] = *(undefined2 *)(iVar5 + -0xc);
              puVar34[1] = *(undefined2 *)(iVar5 + -0xe);
              *puVar34 = 0x1608;
              puVar35 = puVar34 + -1;
              puVar34[-1] = 0x2fb;
              iVar4 = FUN_10b8_06a0();
              puVar56 = puVar35 + 4;
              if (iVar4 == 1) goto LAB_10b8_04b3;
              puVar35[3] = *(undefined2 *)(iVar5 + -0x1a);
              puVar35[2] = iVar58;
              puVar35[1] = *(undefined2 *)(iVar5 + -10);
              *puVar35 = unaff_SS;
              puVar35[-1] = iVar5 + -0x16;
              puVar35[-2] = *(undefined2 *)(iVar5 + -0xc);
              puVar35[-3] = *(undefined2 *)(iVar5 + -0xe);
              puVar35[-4] = 0x1608;
              puVar36 = puVar35 + -5;
              puVar35[-5] = 0x31d;
              FUN_10b8_062d();
              puVar52 = (undefined *)((int)puVar36 + 0x12);
            }
            for (iVar2 = 0x3f5; iVar2 < 0x3f8; iVar2 = iVar2 + 1) {
              *(int *)(puVar52 + -2) = iVar2;
              *(undefined2 *)(puVar52 + -4) = 0x1608;
              puVar37 = (undefined2 *)(puVar52 + -6);
              *(undefined2 *)(puVar52 + -6) = 0x332;
              FUN_10b8_076a();
              iVar58 = iVar58 + 1;
              puVar37[2] = *(undefined2 *)(iVar5 + -0xc);
              puVar37[1] = *(undefined2 *)(iVar5 + -0xe);
              *puVar37 = 0x1608;
              puVar38 = puVar37 + -1;
              puVar37[-1] = 0x33f;
              iVar4 = FUN_10b8_06a0();
              puVar56 = puVar38 + 4;
              if (iVar4 == 1) goto LAB_10b8_04b3;
              puVar38[3] = *(undefined2 *)(iVar5 + -0x1a);
              puVar38[2] = iVar58;
              puVar38[1] = *(undefined2 *)(iVar5 + -10);
              *puVar38 = unaff_SS;
              puVar38[-1] = iVar5 + -0x16;
              puVar38[-2] = *(undefined2 *)(iVar5 + -0xc);
              puVar38[-3] = *(undefined2 *)(iVar5 + -0xe);
              puVar38[-4] = 0x1608;
              puVar39 = puVar38 + -5;
              puVar38[-5] = 0x361;
              FUN_10b8_062d();
              puVar52 = (undefined *)((int)puVar39 + 0x12);
            }
            for (pcVar57 = (char *)s_The_Top_Five_Cities_in_the_World_1420_159c + 0x17;
                (int)pcVar57 < 0x15d8; pcVar57 = pcVar57 + 1) {
              *(char **)(puVar52 + -2) = pcVar57;
              *(undefined2 *)(puVar52 + -4) = 0x1608;
              puVar40 = (undefined2 *)(puVar52 + -6);
              *(undefined2 *)(puVar52 + -6) = 0x376;
              FUN_10b8_076a();
              iVar58 = iVar58 + 1;
              puVar40[2] = *(undefined2 *)(iVar5 + -0xc);
              puVar40[1] = *(undefined2 *)(iVar5 + -0xe);
              *puVar40 = 0x1608;
              puVar41 = puVar40 + -1;
              puVar40[-1] = 899;
              iVar2 = FUN_10b8_06a0();
              puVar56 = puVar41 + 4;
              if (iVar2 == 1) goto LAB_10b8_04b3;
              puVar41[3] = *(undefined2 *)(iVar5 + -0x1a);
              puVar41[2] = iVar58;
              puVar41[1] = *(undefined2 *)(iVar5 + -10);
              *puVar41 = unaff_SS;
              puVar41[-1] = iVar5 + -0x16;
              puVar41[-2] = *(undefined2 *)(iVar5 + -0xc);
              puVar41[-3] = *(undefined2 *)(iVar5 + -0xe);
              puVar41[-4] = 0x1608;
              puVar42 = puVar41 + -5;
              puVar41[-5] = 0x3a5;
              FUN_10b8_062d();
              puVar52 = (undefined *)((int)puVar42 + 0x12);
            }
            for (iVar2 = 0xd05; iVar2 < 0xd14; iVar2 = iVar2 + 1) {
              *(int *)(puVar52 + -2) = iVar2;
              *(undefined2 *)(puVar52 + -4) = 0x1608;
              puVar43 = (undefined2 *)(puVar52 + -6);
              *(undefined2 *)(puVar52 + -6) = 0x3ba;
              FUN_10b8_076a();
              iVar58 = iVar58 + 1;
              puVar43[2] = *(undefined2 *)(iVar5 + -0xc);
              puVar43[1] = *(undefined2 *)(iVar5 + -0xe);
              *puVar43 = 0x1608;
              puVar44 = puVar43 + -1;
              puVar43[-1] = 0x3c7;
              iVar4 = FUN_10b8_06a0();
              puVar56 = puVar44 + 4;
              if (iVar4 == 1) goto LAB_10b8_04b3;
              puVar44[3] = *(undefined2 *)(iVar5 + -0x1a);
              puVar44[2] = iVar58;
              puVar44[1] = *(undefined2 *)(iVar5 + -10);
              *puVar44 = unaff_SS;
              puVar44[-1] = iVar5 + -0x16;
              puVar44[-2] = *(undefined2 *)(iVar5 + -0xc);
              puVar44[-3] = *(undefined2 *)(iVar5 + -0xe);
              puVar44[-4] = 0x1608;
              puVar45 = puVar44 + -5;
              puVar44[-5] = 0x3e9;
              FUN_10b8_062d();
              puVar52 = (undefined *)((int)puVar45 + 0x12);
            }
            for (pcVar57 = (char *)s_Allows__1420_115c; (int)pcVar57 < 0x1176; pcVar57 = pcVar57 + 1
                ) {
              *(char **)(puVar52 + -2) = pcVar57;
              *(undefined2 *)(puVar52 + -4) = 0x1608;
              puVar46 = (undefined2 *)(puVar52 + -6);
              *(undefined2 *)(puVar52 + -6) = 0x3fe;
              FUN_10b8_076a();
              iVar58 = iVar58 + 1;
              puVar46[2] = *(undefined2 *)(iVar5 + -0xc);
              puVar46[1] = *(undefined2 *)(iVar5 + -0xe);
              *puVar46 = 0x1608;
              puVar47 = puVar46 + -1;
              puVar46[-1] = 0x40b;
              iVar2 = FUN_10b8_06a0();
              puVar56 = puVar47 + 4;
              if (iVar2 == 1) goto LAB_10b8_04b3;
              puVar47[3] = *(undefined2 *)(iVar5 + -0x1a);
              puVar47[2] = iVar58;
              puVar47[1] = *(undefined2 *)(iVar5 + -10);
              *puVar47 = unaff_SS;
              puVar47[-1] = iVar5 + -0x16;
              puVar47[-2] = *(undefined2 *)(iVar5 + -0xc);
              puVar47[-3] = *(undefined2 *)(iVar5 + -0xe);
              puVar47[-4] = 0x1608;
              puVar48 = puVar47 + -5;
              puVar47[-5] = 0x42d;
              FUN_10b8_062d();
              puVar52 = (undefined *)((int)puVar48 + 0x12);
            }
            for (pcVar57 = (char *)s_Prob__of_Success_1420_137a + 0xe; (int)pcVar57 < 0x138f;
                pcVar57 = pcVar57 + 1) {
              *(char **)(puVar52 + -2) = pcVar57;
              *(undefined2 *)(puVar52 + -4) = 0x1608;
              puVar49 = (undefined2 *)(puVar52 + -6);
              *(undefined2 *)(puVar52 + -6) = 0x442;
              FUN_10b8_076a();
              iVar58 = iVar58 + 1;
              puVar49[2] = *(undefined2 *)(iVar5 + -0xc);
              puVar49[1] = *(undefined2 *)(iVar5 + -0xe);
              *puVar49 = 0x1608;
              puVar50 = puVar49 + -1;
              puVar49[-1] = 0x44f;
              iVar2 = FUN_10b8_06a0();
              puVar56 = puVar50 + 4;
              if (iVar2 == 1) goto LAB_10b8_04b3;
              puVar50[3] = *(undefined2 *)(iVar5 + -0x1a);
              puVar50[2] = iVar58;
              puVar50[1] = *(undefined2 *)(iVar5 + -10);
              *puVar50 = unaff_SS;
              puVar50[-1] = iVar5 + -0x16;
              puVar50[-2] = *(undefined2 *)(iVar5 + -0xc);
              puVar50[-3] = *(undefined2 *)(iVar5 + -0xe);
              puVar50[-4] = 0x1608;
              puVar51 = puVar50 + -5;
              puVar50[-5] = 0x470;
              FUN_10b8_062d();
              puVar52 = (undefined *)((int)puVar51 + 0x12);
            }
            *(char **)(puVar52 + -2) = (char *)s_plans_retirement_in_20_years__1420_15d0 + 0x10;
            *(undefined2 *)(puVar52 + -4) = 0x1608;
            puVar53 = (undefined2 *)(puVar52 + -6);
            *(undefined2 *)(puVar52 + -6) = 0x482;
            FUN_10b8_076a();
            puVar53[2] = *(undefined2 *)(iVar5 + -0xc);
            puVar53[1] = *(undefined2 *)(iVar5 + -0xe);
            *puVar53 = 0x1608;
            puVar54 = puVar53 + -1;
            puVar53[-1] = 0x48f;
            iVar2 = FUN_10b8_06a0();
            puVar56 = puVar54 + 4;
            if (iVar2 != 1) {
              puVar54[3] = *(undefined2 *)(iVar5 + -0x1a);
              puVar54[2] = iVar58 + 1;
              puVar54[1] = *(undefined2 *)(iVar5 + -10);
              *puVar54 = unaff_SS;
              puVar54[-1] = iVar5 + -0x16;
              puVar54[-2] = *(undefined2 *)(iVar5 + -0xc);
              puVar54[-3] = *(undefined2 *)(iVar5 + -0xe);
              puVar54[-4] = 0x1608;
              puVar55 = puVar54 + -5;
              puVar54[-5] = 0x4b0;
              FUN_10b8_062d();
              puVar56 = (undefined2 *)((int)puVar55 + 0x12);
            }
LAB_10b8_04b3:
            DAT_1420_9980 = 0;
            _DAT_1420_9982 = 0;
            DAT_1420_9986 = 0;
            *(undefined2 *)((int)puVar56 + -2) = *(undefined2 *)(iVar5 + -0xc);
            *(undefined2 *)((int)puVar56 + -4) = *(undefined2 *)(iVar5 + -0xe);
            *(undefined2 *)((int)puVar56 + -6) = 0x1608;
            *(undefined2 *)((int)puVar56 + -8) = 0x4d3;
            FUN_1098_1612();
          }
        }
      }
    }
    else {
      piVar6[3] = *(undefined2 *)(iVar5 + -6);
      piVar6[2] = 0x1608;
      piVar6[1] = 0x3c;
      _LCLOSE();
    }
  }
  return;
}

