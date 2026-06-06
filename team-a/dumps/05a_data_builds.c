/* CIV.EXE game-data init decompile (spec 02 §2.1.2 E 段)
 * task     : data_builds
 * address  : 1090:0488
 * body size: 270 bytes
 *
 * spec 02 §2.1.2 E 段對應假設角色：
 *   data_builds
 *
 * Source: Ghidra DecompInterface (Pcode).
 * 變數名 Ghidra synthetic (uVar1/iVar2)。
 */


void __cdecl16far FUN_1090_0488(void)

{
  undefined2 *puVar1;
  int iVar2;
  undefined2 *puVar3;
  undefined2 uVar4;
  
  FUN_1148_003c();
  uVar4 = 0x42;
  DAT_1420_9964 = GLOBALALLOC((char *)s_improvement_1420_1147 + 1,0x3b4);
  puVar3 = (undefined2 *)GLOBALLOCK(0x1608,DAT_1420_9964,uVar4);
  uVar4 = (undefined2)((ulong)puVar3 >> 0x10);
  puVar1 = (undefined2 *)puVar3;
  *puVar3 = 0x300;
  puVar1[1] = 0xec;
  for (iVar2 = 0; iVar2 < 0xec; iVar2 = iVar2 + 1) {
    *(undefined *)(puVar1 + iVar2 * 2 + 2) = 0;
    *(undefined *)((int)puVar1 + iVar2 * 4 + 5) = 0;
    *(undefined *)(puVar1 + iVar2 * 2 + 3) = 0;
    if (DAT_1420_17a6 == 0) {
      *(undefined *)((int)puVar1 + iVar2 * 4 + 7) = 1;
    }
    else {
      *(undefined *)((int)puVar1 + iVar2 * 4 + 7) = 0;
    }
  }
  if (DAT_1420_9966 != 0) {
    DELETEOBJECT(0x1608,DAT_1420_9966);
  }
  DAT_1420_9966 = CREATEPALETTE(0x1608,puVar1);
  if (DAT_1420_9964 != 0) {
    GLOBALUNLOCK(0x1608,DAT_1420_9964,uVar4);
  }
  return;
}

