/* CIV.EXE game-data init decompile (spec 02 §2.1.2 E 段)
 * task     : data_units
 * address  : 11e8:0337
 * body size: 461 bytes
 *
 * spec 02 §2.1.2 E 段對應假設角色：
 *   data_units
 *
 * Source: Ghidra DecompInterface (Pcode).
 * 變數名 Ghidra synthetic (uVar1/iVar2)。
 */


void __cdecl16far FUN_11e8_0337(void)

{
  undefined2 uVar1;
  char *pcVar2;
  int iVar3;
  
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
  DAT_1420_aea2 = FUN_11a8_0054(0x20,0x20,0);
  pcVar2 = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
  DAT_1420_b394 = FUN_1100_05b8(64000);
  DAT_1420_b396 = pcVar2;
  FUN_11c8_0000();
  FUN_1000_18d4(0,0);
  FUN_1000_1716();
  for (iVar3 = 0; iVar3 < 8; iVar3 = iVar3 + 1) {
    uVar1 = FUN_11f8_0080(0x8f,iVar3);
    FUN_1000_36b2(iVar3 * 0x20 + 0x3da,0x12d8,uVar1);
    uVar1 = FUN_11f8_0080(0x90);
    FUN_1000_36b2(iVar3 * 0x20 + 0x2da,0x12d8,uVar1);
    FUN_11f8_0080(0x91);
    FUN_1000_36b2(iVar3 * 0x20 + 0x1da,0x12d8);
  }
  for (iVar3 = 0; iVar3 < 0xd; iVar3 = iVar3 + 1) {
    FUN_11f8_0080(0x92,iVar3);
    FUN_1000_36b2(iVar3 * 0x20 + 0x3a,0x12d8);
  }
  for (iVar3 = 0; iVar3 < 0x100; iVar3 = iVar3 + 1) {
    FUN_11f8_0080(0x87,iVar3);
    FUN_1000_36b2(iVar3 * 0x20 + 0x4da,0x12d8);
  }
  return;
}

