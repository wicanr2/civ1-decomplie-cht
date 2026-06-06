/* CIV.EXE gr_pic.c family decompile via byte-pattern scan
 * function : LoadGifPicture
 * address  : 10b8:11bc
 * body size: 717 bytes
 *
 * Located by scanning for instruction `PUSH 0x2372` (= assert
 * string offset for `LoadGifPicture()  :  gr_pic.c` in DGROUP).
 *
 * Source: Ghidra DecompInterface (Pcode).
 * Variable names are Ghidra synthetic (uVar1 / iVar2).
 */


int __cdecl16far
FUN_10b8_11bc(int param_1,undefined2 param_2,char *param_3,byte param_4,int *param_5,
             undefined4 param_6)

{
  undefined uVar1;
  undefined uVar2;
  undefined uVar3;
  undefined2 uVar4;
  undefined2 *puVar5;
  int iVar6;
  int iVar7;
  undefined2 *puVar8;
  undefined2 uVar9;
  int iVar10;
  int iVar11;
  undefined2 in_DX;
  undefined2 uVar12;
  int iVar13;
  int iVar14;
  uint uVar15;
  char *pcVar16;
  char *pcVar17;
  undefined2 uVar18;
  undefined2 uVar19;
  undefined4 local_26;
  undefined4 local_22;
  undefined2 *local_1a;
  undefined4 local_a;
  
  iVar6 = FUN_1190_0aea();
  if (iVar6 == 0) {
    iVar7 = 0;
  }
  else {
    puVar8 = (undefined2 *)GLOBALLOCK((char *)s__Taken_from_1420_1187 + 9,iVar6);
    local_a = (undefined2 *)CONCAT22(in_DX,puVar8);
    uVar9 = FUN_1100_0565(*local_a);
    *local_a = uVar9;
    uVar9 = FUN_1100_0565(puVar8[1]);
    puVar8[1] = uVar9;
    uVar9 = *local_a;
    uVar4 = puVar8[1];
    iVar10 = *(byte *)((int)puVar8 + 5) + 1;
    uVar1 = *(undefined *)(puVar8 + 2);
    local_1a = (undefined2 *)CONCAT22(in_DX,puVar8 + 3);
    iVar11 = iVar10 * 3;
    FUN_1148_003c();
    uVar19 = 0x42;
    pcVar16 = (char *)0x1608;
    uVar12 = uVar4;
    iVar7 = GLOBALALLOC();
    if (iVar7 == 0) {
      pcVar16 = (char *)s_nothing_1420_1093 + 5;
      FUN_1098_28e4((long)param_1,(char *)s_Memory_error_during_picture_load_1420_2390,
                    (char *)s_Type__Propulsion_Fuel_1420_1416 + 10,uVar19);
    }
    pcVar17 = (char *)CONCAT22(iVar7,pcVar16);
    uVar19 = GLOBALLOCK(pcVar16,iVar7);
    local_22 = CONCAT22(uVar12,uVar19);
    if (local_22 != 0) {
      if (0xec < (uint)param_4 + iVar10) {
        iVar10 = 0xec - (uint)param_4;
      }
      *param_5 = iVar10;
      iVar14 = 0;
      while( true ) {
        iVar13 = (int)param_6;
        uVar18 = (undefined2)((ulong)param_6 >> 0x10);
        if ((int)(uint)param_4 <= iVar14) break;
        *(undefined *)(iVar13 + iVar14 * 4) = 0;
        *(undefined *)(iVar13 + iVar14 * 4 + 1) = 0;
        *(undefined *)(iVar13 + iVar14 * 4 + 2) = 0;
        if (DAT_1420_17a6 == 0) {
          *(undefined *)(iVar13 + iVar14 * 4 + 3) = 1;
        }
        else {
          *(undefined *)(iVar13 + iVar14 * 4 + 3) = 0;
        }
        iVar14 = iVar14 + 1;
      }
      for (uVar15 = (uint)param_4; uVar15 < (uint)param_4 + iVar10; uVar15 = uVar15 + 1) {
        uVar2 = *(undefined *)local_1a;
        puVar5 = (undefined2 *)local_1a;
        local_1a = (undefined2 *)
                   CONCAT22(local_1a._2_2_,(undefined *)((int)(undefined2 *)local_1a + 1));
        uVar3 = *(undefined *)local_1a;
        local_1a = (undefined2 *)CONCAT22(local_1a._2_2_,puVar5 + 1);
        local_26._0_3_ = CONCAT12(*(undefined *)local_1a,CONCAT11(uVar3,uVar2));
        local_1a = (undefined2 *)CONCAT22(local_1a._2_2_,(undefined2 *)((int)puVar5 + 3));
        if (DAT_1420_17a6 == 0) {
          local_26 = CONCAT13(1,(uint3)local_26);
        }
        else {
          local_26 = (ulong)(uint3)local_26;
        }
        *(ulong *)(iVar13 + uVar15 * 4) = local_26;
      }
      iVar10 = (uint)param_4 + iVar10;
      iVar14 = iVar10;
      for (; iVar10 < 0xec; iVar10 = iVar10 + 1) {
        *(undefined *)(iVar13 + iVar10 * 4) = 0;
        *(undefined *)(iVar13 + iVar10 * 4 + 1) = 0;
        *(undefined *)(iVar13 + iVar10 * 4 + 2) = 0;
        if (DAT_1420_17a6 == 0) {
          *(undefined *)(iVar13 + iVar10 * 4 + 3) = 1;
        }
        else {
          *(undefined *)(iVar13 + iVar10 * 4 + 3) = 0;
        }
        iVar14 = iVar10 << 2;
      }
      uVar18 = (undefined2)((ulong)pcVar17 >> 0x10);
      FUN_10b8_158c((undefined *)((int)(puVar8 + 3) + iVar11),in_DX,
                    CONCAT11((char)((uint)iVar14 >> 8),param_4),uVar1,uVar9,uVar9,uVar4,uVar19,
                    uVar12);
      pcVar17 = (char *)CONCAT22(uVar18,param_3);
      SETRECT(0x1608,uVar4,uVar9,0,0,param_2);
    }
    if (iVar7 != 0) {
      GLOBALUNLOCK(0x1608,iVar7,pcVar17);
    }
    if (iVar6 != 0) {
      GLOBALUNLOCK(0x1608,iVar6,pcVar17);
    }
    FUN_1190_09ed();
  }
  return iVar7;
}

