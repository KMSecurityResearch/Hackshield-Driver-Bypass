/*------------------------------------------------------------------------
* ���W�� : NtProtectVirtualMemory.h                                                 
* �sĶ���� : WDK 7600.16385.1
--------------------------------------------------------------------------
* File Name            : NtProtectVirtualMemory.h
* Building environment : WDK 7600.16385.1
-------------------------------------------------------------------------*/

ULONG NtProtectVirtualMemory_CN;
ULONG NtProtectVirtualMemory_Addr;
ULONG NtProtectVirtualMemory_HookAddr;
UCHAR NtProtectVirtualMemory_Hook_Org_Mem[5]={0x00,0x00,0x00,0x00,0x00};

ULONG MyNtProtectVirtualMemory_JmpAddr;
ULONG MyNtProtectVirtualMemoryCall;

ULONG NtProtectVirtualMemory_Hook_Calc;
UCHAR NtProtectVirtualMemory_Hook_Mem[5]={0xe9,0x00,0x00,0x00,0x00};

//////////////////////////////////////

__declspec(naked) NTSTATUS __stdcall MyNtProtectVirtualMemory()
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
 mov  eax,[MyNtProtectVirtualMemory_JmpAddr]
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
 call MyNtProtectVirtualMemoryCall
 jmp  [MyNtProtectVirtualMemory_JmpAddr] 
 }
 }
}

VOID NtProtectVirtualMemory_Hook()
{
 //���o��Ʀ�l Win7_x86 NtProtectVirtualMemory�ǦC���� 0xD7
 //NtProtectVirtualMemory_CN = 0xD7;

//�ʺA���o��Ʀ�l
 NtProtectVirtualMemory_CN = GetFunctionId("NtProtectVirtualMemory");
DbgPrint("[NtProtectVirtualMemory_CN] : 0x%x\n",NtProtectVirtualMemory_CN);
 NtProtectVirtualMemory_Addr = FindOriAddress(NtProtectVirtualMemory_CN);
DbgPrint("[NtProtectVirtualMemory_Addr] : 0x%08X\n",NtProtectVirtualMemory_Addr);

 //Jmp�I
 MyNtProtectVirtualMemory_JmpAddr = NtProtectVirtualMemory_Addr + 0xC;
DbgPrint("[MyNtProtectVirtualMemory_JmpAddr] : 0x%08X\n",MyNtProtectVirtualMemory_JmpAddr);

 //Hook�I
 NtProtectVirtualMemory_HookAddr = NtProtectVirtualMemory_Addr + 2;
DbgPrint("[NtProtectVirtualMemory_HookAddr] : 0x%08X\n",NtProtectVirtualMemory_HookAddr);

 //�p��CALL
 MyNtProtectVirtualMemoryCall = NtProtectVirtualMemory_Addr + 8;
 MyNtProtectVirtualMemoryCall = Find_CurAddr_OriData(MyNtProtectVirtualMemoryCall); 
 MyNtProtectVirtualMemoryCall = (ULONG)*((PULONG)MaxByte)-(ULONG)MyNtProtectVirtualMemoryCall;   
 MyNtProtectVirtualMemoryCall = MyNtProtectVirtualMemory_JmpAddr-MyNtProtectVirtualMemoryCall-0x1;  
DbgPrint("[MyNtProtectVirtualMemoryCall] : 0x%08X\n",MyNtProtectVirtualMemoryCall);

 //�ƻsHook�I��l���e
 WPOFF();
 RtlCopyMemory (NtProtectVirtualMemory_Hook_Org_Mem, (PVOID)NtProtectVirtualMemory_HookAddr , 5);
 RtlCopyMemory((PVOID)MyNtProtectVirtualMemory,NtProtectVirtualMemory_Hook_Org_Mem,5); 
 WPON();
DbgPrint("[MyNtProtectVirtualMemory] : 0x%08X\n",(PVOID)MyNtProtectVirtualMemory);
 
 ////�p��Jmp�a�},�æX��Jmp���O
 NtProtectVirtualMemory_Hook_Calc = (PCHAR)MyNtProtectVirtualMemory - (PCHAR)NtProtectVirtualMemory_HookAddr - 5;
 RtlCopyMemory(NtProtectVirtualMemory_Hook_Mem + 1,&NtProtectVirtualMemory_Hook_Calc,4);

 //Hook
 WPOFF();
 RtlCopyMemory((PVOID)NtProtectVirtualMemory_HookAddr,(PVOID)NtProtectVirtualMemory_Hook_Mem,5);
 WPON();

 DbgPrint("NtProtectVirtualMemory Hook Success!\n");

}

VOID NtProtectVirtualMemory_UnHook()
{
  WPOFF();
  RtlCopyMemory((PVOID)NtProtectVirtualMemory_HookAddr,NtProtectVirtualMemory_Hook_Org_Mem,5);
  WPON(); 
  DbgPrint("NtProtectVirtualMemory UnHook Success!\n");
}

