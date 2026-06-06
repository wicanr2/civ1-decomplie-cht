
int __cdecl16far FUN_11a8_0054(int param_1,int param_2)

{
  int iVar1;
  int *piVar2;
  undefined2 uVar3;
  long lVar4;
  undefined2 uVar5;
  undefined4 local_8;
  
  FUN_1148_003c();
  lVar4 = (long)param_2;
  uVar5 = 0x42;
  iVar1 = GLOBALALLOC((char *)s_improvement_1420_1147 + 1,param_1 * lVar4 + 6);
  uVar3 = (undefined2)lVar4;
  if (iVar1 == 0) {
    iVar1 = 0;
  }
  else {
    piVar2 = (int *)GLOBALLOCK(0x1608,iVar1,uVar5);
    local_8 = (int *)CONCAT22(uVar3,piVar2);
    *local_8 = param_1;
    piVar2[1] = param_2;
    if (iVar1 != 0) {
      GLOBALUNLOCK(0x1608,iVar1);
    }
  }
  return iVar1;
}

