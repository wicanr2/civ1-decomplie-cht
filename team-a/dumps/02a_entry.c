/* CIV.EXE callback decompile
 * function : entry
 * address  : 1000:0000
 * body size: 178 bytes
 *
 * Source: Ghidra DecompInterface (Pcode).
 * NOT a faithful original-source recovery; the original
 * Borland C++ source has been lost. Variable names are
 * Ghidra synthetic (uVar1 / iVar2). Use as a structural
 * guide only.
 */


/* Title:  �������� ������
   Format: New Executable (NE) Windows
   CRC:    00000000
   
   Program Entry Point (CS:IP):   0001:0000
   Initial Stack Pointer (SS:SP): 0085:0000
   Auto Data Segment Index:       0085
   Initial Heap Size:             0c00
   Initial Stack Size:            2400
   Minimum Code Swap Size:        0000
   
   Linker Version:  5.10
   Target OS:       Windows
   Windows Version: 3.10
   
   Program Flags:     0a
           Multi Data
           Protected Mode
   Application Flags: 03
           Windows P.M. API
   Other Flags:       08
    */

void __cdecl16far entry(void)

{
  undefined *puVar1;
  code *pcVar2;
  char cVar3;
  int iVar4;
  undefined2 uVar5;
  int iVar6;
  undefined2 extraout_DX;
  undefined2 in_BX;
  undefined2 unaff_SI;
  undefined2 unaff_DI;
  undefined *puVar7;
  undefined2 unaff_ES;
  undefined4 uVar8;
  ulong uVar9;
  
                    /* Segment:    1
                       Offset:     00000c00
                       Length:     4b85
                       Min Alloc:  4b85
                       Flags:      1d50
                           Code
                           Discardable
                           Moveable
                           Preload
                           Impure (Non-shareable)
                        */
  uVar8 = INITTASK();
  if ((int)uVar8 != 0) {
    puVar7 = (undefined *)&DAT_1420_5146;
    DAT_1420_0016 = unaff_ES;
    DAT_1420_0018 = unaff_DI;
    DAT_1420_001a = unaff_SI;
    DAT_1420_001c = in_BX;
    DAT_1420_001e = (int)((ulong)uVar8 >> 0x10);
    for (iVar6 = 0x6378; iVar6 != 0; iVar6 = iVar6 + -1) {
      puVar1 = puVar7;
      puVar7 = puVar7 + 1;
      *puVar1 = 0;
    }
    WAITEVENT(0x1608,0);
    iVar4 = INITAPP(0x1608,DAT_1420_0018);
    if (iVar4 != 0) {
      pcVar2 = (code *)swi(0x1a);
      cVar3 = (*pcVar2)();
      if (cVar3 != '\0') {
        DAT_0040_0070 = 1;
      }
      pcVar2 = (code *)swi(0x21);
      DAT_1420_0020 = extraout_DX;
      DAT_1420_0022 = iVar6;
      DAT_1420_0024 = (*pcVar2)();
      uVar9 = GETWINFLAGS();
      if ((uVar9 & 1) != 0) {
        DAT_1420_0012 = 8;
      }
      if ((uVar9 & 0x40000) != 0) {
        DAT_1420_0014 = 1;
      }
      FUN_1000_00de();
      uVar5 = FUN_1008_0000(DAT_1420_001e,DAT_1420_001c,DAT_1420_0016,DAT_1420_001a,DAT_1420_0018);
      FUN_1000_12d6(uVar5);
      FUN_1000_0122(0x5140,0x511c);
      return;
    }
  }
  FUN_1000_12d6(0xff);
  pcVar2 = (code *)swi(0x21);
  (*pcVar2)();
  return;
}

