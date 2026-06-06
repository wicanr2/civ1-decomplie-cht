
undefined2 __cdecl16far FUN_1000_18d4(undefined2 *param_1)

{
  undefined2 uVar1;
  undefined2 in_DX;
  int unaff_BP;
  undefined local_a [4];
  undefined local_6 [4];
  int iStack_2;
  
  iStack_2 = unaff_BP + 1;
  FUN_1000_13bc(local_6);
  FUN_1000_13d1(local_a);
  uVar1 = FUN_1000_386e(local_6);
  if ((undefined2 *)param_1 != (undefined2 *)0x0 || param_1._2_2_ != 0) {
    ((undefined2 *)param_1)[1] = in_DX;
    *param_1 = uVar1;
  }
  return uVar1;
}

