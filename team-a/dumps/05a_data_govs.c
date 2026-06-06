/* CIV.EXE game-data init decompile (spec 02 §2.1.2 E 段)
 * task     : data_govs
 * address  : 1008:0b4a
 * body size: 117 bytes
 *
 * spec 02 §2.1.2 E 段對應假設角色：
 *   data_govs
 *
 * Source: Ghidra DecompInterface (Pcode).
 * 變數名 Ghidra synthetic (uVar1/iVar2)。
 */


void __cdecl16far FUN_1008_0b4a(void)

{
  int iVar1;
  undefined2 unaff_CS;
  int iVar2;
  char *pcVar3;
  
  DAT_12d0_0064 = DAT_1420_17ca;
  pcVar3 = (char *)s_Type__Propulsion_Fuel_1420_1416 + 10;
  iVar1 = _LOPEN(unaff_CS,0,0x10b);
  if (iVar1 != -1) {
    iVar2 = iVar1;
    _LREAD(0x1608,0x1a,(undefined2 *)&DAT_1420_17b2,(char *)s_Type__Propulsion_Fuel_1420_1416 + 10);
    _LCLOSE(0x1608,iVar1,iVar2,pcVar3);
    DAT_12d0_0064 = DAT_1420_17ca;
  }
  return;
}

