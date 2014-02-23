/*------------------------------------------------------------------------
* ���W�� : NtReadVirtualMemory.h                                                 
* �sĶ���� : WDK 7600.16385.1
--------------------------------------------------------------------------
* File Name            : NtReadVirtualMemory.h 
* Building environment : WDK 7600.16385.1
-------------------------------------------------------------------------*/

ULONG NtReadVirtualMemory_CN;
ULONG NtReadVirtualMemory_Addr;
ULONG NtReadVirtualMemory_HookAddr;
UCHAR NtReadVirtualMemory_Hook_Org_Mem[5]={0x00,0x00,0x00,0x00,0x00};

ULONG MyNtReadVirtualMemory_JmpAddr;
ULONG MyNtReadVirtualMemoryCall;

ULONG NtReadVirtualMemory_Hook_Calc;
UCHAR NtReadVirtualMemory_Hook_Mem[5]={0xe9,0x00,0x00,0x00,0x00};

//////////////////////////////////////

__declspec(naked) NTSTATUS __stdcall MyNtReadVirtualMemory()
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
 mov  eax,[MyNtReadVirtualMemory_JmpAddr]
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
  call MyNtReadVirtualMemoryCall
  jmp  [MyNtReadVirtualMemory_JmpAddr] 
  }
 }
}

VOID NtReadVirtualMemory_Hook()
{
 //���o��Ʀ�l Win7_x86 NtReadVirtualMemory�ǦC���� 0x115
 //NtReadVirtualMemory_CN = 0x115;

//�ʺA���o��Ʀ�l
 NtReadVirtualMemory_CN = GetFunctionId("NtReadVirtualMemory");
DbgPrint("[NtReadVirtualMemory_CN] : 0x%x\n",NtReadVirtualMemory_CN);
 NtReadVirtualMemory_Addr = FindOriAddress(NtReadVirtualMemory_CN);
DbgPrint("[NtReadVirtualMemory_Addr] : 0x%08X\n",NtReadVirtualMemory_Addr);

 //Jmp�I
 MyNtReadVirtualMemory_JmpAddr = NtReadVirtualMemory_Addr + 0xC;
DbgPrint("[MyNtReadVirtualMemory_JmpAddr] : 0x%08X\n",MyNtReadVirtualMemory_JmpAddr);

 //Hook�I
 NtReadVirtualMemory_HookAddr = NtReadVirtualMemory_Addr + 2;
DbgPrint("[NtReadVirtualMemory_HookAddr] : 0x%08X\n",NtReadVirtualMemory_HookAddr);

 //�p��CALL
 MyNtReadVirtualMemoryCall = NtReadVirtualMemory_Addr + 8;
 MyNtReadVirtualMemoryCall = Find_CurAddr_OriData(MyNtReadVirtualMemoryCall); 
 MyNtReadVirtualMemoryCall = (ULONG)*((PULONG)MaxByte)-(ULONG)MyNtReadVirtualMemoryCall;   
 MyNtReadVirtualMemoryCall = MyNtReadVirtualMemory_JmpAddr-MyNtReadVirtualMemoryCall-0x1;  
DbgPrint("[MyNtReadVirtualMemoryCall] : 0x%08X\n",MyNtReadVirtualMemoryCall);

 //�ƻsHook�I��l���e
 WPOFF();
 RtlCopyMemory (NtReadVirtualMemory_Hook_Org_Mem, (PVOID)NtReadVirtualMemory_HookAddr , 5);
 RtlCopyMemory((PVOID)MyNtReadVirtualMemory,NtReadVirtualMemory_Hook_Org_Mem,5); 
 WPON();
DbgPrint("[MyNtReadVirtualMemory] : 0x%08X\n",(PVOID)MyNtReadVirtualMemory);
 
 ////�p��Jmp�a�},�æX��Jmp���O
 NtReadVirtualMemory_Hook_Calc = (PCHAR)MyNtReadVirtualMemory - (PCHAR)NtReadVirtualMemory_HookAddr - 5;
 RtlCopyMemory(NtReadVirtualMemory_Hook_Mem + 1,&NtReadVirtualMemory_Hook_Calc,4);

 //Hook
 WPOFF();
 RtlCopyMemory((PVOID)NtReadVirtualMemory_HookAddr,(PVOID)NtReadVirtualMemory_Hook_Mem,5);
 WPON();

 DbgPrint("NtReadVirtualMemory Hook Success!\n");

}

VOID NtReadVirtualMemory_UnHook()
{
  WPOFF();
  RtlCopyMemory((PVOID)NtReadVirtualMemory_HookAddr,NtReadVirtualMemory_Hook_Org_Mem,5);
  WPON(); 
  DbgPrint("NtReadVirtualMemory UnHook Success!\n");
}
