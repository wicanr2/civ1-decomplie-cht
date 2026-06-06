
undefined2 __cdecl16far FUN_1100_05b8(long param_1)

{
  undefined2 uVar1;
  char *pcVar2;
  undefined2 uVar3;
  
  pcVar2 = (char *)s_improvement_1420_1147 + 1;
  FUN_1148_003c();
  if (param_1 == 0x240) {
    pcVar2 = (char *)s_Error_allocating_civilopedia_tex_1420_0ffa + 0xe;
    FUN_1008_09d3();
  }
  uVar3 = 0x42;
  uVar1 = GLOBALALLOC(pcVar2,param_1);
  uVar1 = GLOBALLOCK(0x1608,uVar1,uVar3);
  return uVar1;
}

