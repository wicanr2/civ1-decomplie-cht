
void __cdecl16far FUN_11c8_0000(void)

{
  uint uVar1;
  int unaff_BP;
  
                    /* Segment:    58
                       Offset:     0007d600
                       Length:     0459
                       Min Alloc:  0459
                       Flags:      1d50
                           Code
                           Discardable
                           Moveable
                           Preload
                           Impure (Non-shareable)
                        */
  uVar1 = FUN_1100_02e1((char *)s_Type__Propulsion_Fuel_1420_1416 + 10,unaff_BP + 1);
  DAT_13c0_0000 = uVar1 & 0x7fff;
  if (DAT_13c0_0000 == 0) {
    DAT_13c0_0000 = 1;
  }
  FUN_1000_1716(DAT_13c0_0000);
  return;
}

