/* CIV.EXE game-data init decompile (spec 02 §2.1.2 E 段)
 * task     : data_civs
 * address  : 1008:059a
 * body size: 149 bytes
 *
 * spec 02 §2.1.2 E 段對應假設角色：
 *   data_civs
 *
 * Source: Ghidra DecompInterface (Pcode).
 * 變數名 Ghidra synthetic (uVar1/iVar2)。
 */


void __cdecl16far FUN_1008_059a(void)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  
  uVar1 = GETDC();
  iVar2 = GETDEVICECAPS(0x1608,0xc);
  iVar3 = GETDEVICECAPS(0x1608,0xe);
  uVar4 = GETDEVICECAPS(0x1608,0x26);
  DAT_1420_17a6 = (uint)((uVar4 & 0x100) == 0);
  if ((iVar2 * iVar3 == 4) ||
     ((iVar5 = GETDEVICECAPS(0x1608,0x18), iVar5 == 0x10 && (iVar2 * iVar3 == 8)))) {
    DAT_1420_17a8 = 1;
  }
  else {
    DAT_1420_17a8 = 0;
  }
  RELEASEDC(0x1608,uVar1);
  return;
}

