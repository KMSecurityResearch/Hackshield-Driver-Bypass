/*------------------------------------------------------------------------
* ���W�� : NtOpenProcess_Win7.h                                                 
* �sĶ���� : WDK 7600.16385.1
* ��    �� : Win7 NtOpenProcess ��H���������P�ҥH�ݥt�~�g
/-------------------------------------------------------------------------
* File Name            : NtOpenProcess_Win7.h
* Building environment : WDK 7600.16385.1
-------------------------------------------------------------------------*/

ULONG NtOpenProcess_Win7_CN;
ULONG NtOpenProcess_Win7_Addr;
ULONG NtOpenProcess_Win7_HookAddr;
UCHAR NtOpenProcess_Win7_Hook_Org_Mem[6]={0x00,0x00,0x00,0x00,0x00,0x00};

ULONG MyNtOpenProcess_Win7_JmpAddr;
ULONG MyNtOpenProcess_Win7_Call;

ULONG NtOpenProcess_Win7_Hook_Calc;
UCHAR NtOpenProcess_Win7_Hook_Mem[6]={0xe9,0x00,0x00,0x00,0x00,0x90};

__declspec(naked) NTSTATUS __stdcall MyNtOpenProcess_Win7() 
{
 __asm
 {
 _EMIT 0x90;
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
 mov  eax,[MyNtOpenProcess_Win7_JmpAddr]
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
 call MyNtOpenProcess_Win7_Call
 jmp  [MyNtOpenProcess_Win7_JmpAddr]
 }
 }
}

VOID NtOpenProcess_Win7_Hook()
{
 //���o��Ʀ�l Win7_x86 NtOpenProcess�ǦC���� 0xBE
 //NtOpenProcess_Win7_CN = 0xBE;

//�ʺA���o��Ʀ�l
 NtOpenProcess_Win7_CN = GetFunctionId("NtOpenProcess");
DbgPrint("[NtOpenProcess_Win7_CN] : 0x%x\n",NtOpenProcess_Win7_CN);
 NtOpenProcess_Win7_Addr = FindOriAddress(NtOpenProcess_Win7_CN);
DbgPrint("[NtOpenProcess_Win7_Addr] : 0x%08X\n",NtOpenProcess_Win7_Addr);

 //Jmp�I
 MyNtOpenProcess_Win7_JmpAddr = NtOpenProcess_Win7_Addr + 0x2D;
DbgPrint("[MyNtOpenProcess_Win7_JmpAddr] : 0x%08X\n",MyNtOpenProcess_Win7_JmpAddr);

 //Hook�I
 NtOpenProcess_Win7_HookAddr = NtOpenProcess_Win7_Addr + 0x22;
DbgPrint("[NtOpenProcess_Win7_HookAddr] : 0x%08X\n",NtOpenProcess_Win7_HookAddr);

 //�p��CALL
 MyNtOpenProcess_Win7_Call = NtOpenProcess_Win7_Addr + 0x29;
 MyNtOpenProcess_Win7_Call = Find_CurAddr_OriData(MyNtOpenProcess_Win7_Call); 
 MyNtOpenProcess_Win7_Call = (ULONG)*((PULONG)MaxByte)-(ULONG)MyNtOpenProcess_Win7_Call;   
 MyNtOpenProcess_Win7_Call = MyNtOpenProcess_Win7_JmpAddr-MyNtOpenProcess_Win7_Call-0x1;  
DbgPrint("[MyNtOpenProcess_Win7_Call] : 0x%08X\n",MyNtOpenProcess_Win7_Call);

//�ƻsHook�I��l���e
 WPOFF();
 RtlCopyMemory (NtOpenProcess_Win7_Hook_Org_Mem, (PVOID)NtOpenProcess_Win7_HookAddr , 6);
 RtlCopyMemory((PVOID)MyNtOpenProcess_Win7,NtOpenProcess_Win7_Hook_Org_Mem,6); 
 WPON();
DbgPrint("[MyNtOpenProcess] : 0x%08X\n",(PVOID)MyNtOpenProcess_Win7);
 
 ////�p��Jmp�a�},�æX��Jmp���O
 NtOpenProcess_Win7_Hook_Calc = (PCHAR)MyNtOpenProcess_Win7- (PCHAR)NtOpenProcess_Win7_HookAddr - 5;
 RtlCopyMemory(NtOpenProcess_Win7_Hook_Mem + 1,&NtOpenProcess_Win7_Hook_Calc,4);

 //Hook
 WPOFF();
 RtlCopyMemory((PVOID)NtOpenProcess_Win7_HookAddr,(PVOID)NtOpenProcess_Win7_Hook_Mem,6);
 WPON();

 DbgPrint("NtOpenProcess Hook Success!\n");

}

VOID NtOpenProcess_Win7_UnHook()
{
  WPOFF();
  RtlCopyMemory((PVOID)NtOpenProcess_Win7_HookAddr,NtOpenProcess_Win7_Hook_Org_Mem,6);
  WPON(); 
  DbgPrint("NtOpenProcess UnHook Success!\n");
}