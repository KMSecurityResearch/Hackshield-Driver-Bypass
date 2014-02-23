/*------------------------------------------------------------------------
* ���W�� : NtOpenProcess.h                                                 
* �sĶ���� : WDK 7600.16385.1
/-------------------------------------------------------------------------
* File Name            : NtOpenProcess.h 
* Building environment : WDK 7600.16385.1
-------------------------------------------------------------------------*/

ULONG NtOpenProcess_CN;
ULONG NtOpenProcess_Addr;
ULONG NtOpenProcess_HookAddr;
UCHAR NtOpenProcess_Hook_Org_Mem[5]={0x00,0x00,0x00,0x00,0x00};

ULONG MyNtOpenProcess_JmpAddr;
ULONG MyNtOpenProcessCall;

ULONG NtOpenProcess_Hook_Calc;
UCHAR NtOpenProcess_Hook_Mem[5]={0xe9,0x00,0x00,0x00,0x00};

//////////////////////////////////////

__declspec(naked) NTSTATUS __stdcall MyNtOpenProcess()
{
 __asm
 {
 _EMIT 0x90;
 _EMIT 0x90;
 _EMIT 0x90;
 _EMIT 0x90;
 _EMIT 0x90;
 }
  __asm
 {
 pushad
 pushf
 }
 if (GameExecutionCheck()) //HS�|�˴�Hook�O�_�B�@���`,�����`�|�� Error 0x10301.�ҥH�n��D�{���]���¦W��
 {
 __asm
 {
 popf
 popad
 }
 __asm
 { 
 mov  eax,[MyNtOpenProcess_JmpAddr]
 sub  eax,5 
 jmp  eax
 }
 }
 else
 {
 __asm
 {
 popf
 popad
 }
 __asm
  {
  call MyNtOpenProcessCall
  jmp  [MyNtOpenProcess_JmpAddr] 
  }
 }
}

VOID NtOpenProcess_Hook()
{
 //���o��Ʀ�l WinXp_x86 NtOpenProcess�ǦC���� 0x7A
 //NtOpenProcess_CN = 0x7A;

//�ʺA���o��Ʀ�l
 NtOpenProcess_CN = GetFunctionId("NtOpenProcess");
DbgPrint("[NtOpenProcess_CN] : 0x%x\n",NtOpenProcess_CN);
 NtOpenProcess_Addr = FindOriAddress(NtOpenProcess_CN);
DbgPrint("[NtOpenProcess_Addr] : 0x%08X\n",NtOpenProcess_Addr);


 //Jmp�I
 MyNtOpenProcess_JmpAddr = NtOpenProcess_Addr + 0xF;
DbgPrint("[MyNtOpenProcess_JmpAddr] : 0x%08X\n",MyNtOpenProcess_JmpAddr);

 //Hook�I
 NtOpenProcess_HookAddr = NtOpenProcess_Addr + 0x5;
DbgPrint("[NtOpenProcess_HookAddr] : 0x%08X\n",NtOpenProcess_HookAddr);

 //�p��CALL
 MyNtOpenProcessCall = NtOpenProcess_Addr + 0xB;
 MyNtOpenProcessCall = Find_CurAddr_OriData(MyNtOpenProcessCall); 
 MyNtOpenProcessCall = (ULONG)*((PULONG)MaxByte)-(ULONG)MyNtOpenProcessCall;   
 MyNtOpenProcessCall = MyNtOpenProcess_JmpAddr-MyNtOpenProcessCall-0x1;  
DbgPrint("[MyNtOpenProcessCall] : 0x%08X\n",MyNtOpenProcessCall);

 //�ƻsHook�I��l���e
 WPOFF();
 RtlCopyMemory (NtOpenProcess_Hook_Org_Mem, (PVOID)NtOpenProcess_HookAddr , 5);
 RtlCopyMemory((PVOID)MyNtOpenProcess,NtOpenProcess_Hook_Org_Mem,5); 
 WPON();
DbgPrint("[MyNtOpenProcess] : 0x%08X\n",(PVOID)MyNtOpenProcess);
 
 ////�p��Jmp�a�},�æX��Jmp���O
 NtOpenProcess_Hook_Calc = (PCHAR)MyNtOpenProcess - (PCHAR)NtOpenProcess_HookAddr - 5;
 RtlCopyMemory(NtOpenProcess_Hook_Mem + 1,&NtOpenProcess_Hook_Calc,4);

 //Hook
 WPOFF();
 RtlCopyMemory((PVOID)NtOpenProcess_HookAddr,(PVOID)NtOpenProcess_Hook_Mem,5);
 WPON();

 DbgPrint("NtOpenProcess Hook Success!\n");

}

VOID NtOpenProcess_UnHook()
{
  WPOFF();
  RtlCopyMemory((PVOID)NtOpenProcess_HookAddr,NtOpenProcess_Hook_Org_Mem,5);
  WPON(); 
  DbgPrint("NtOpenProcess UnHook Success!\n");
}

