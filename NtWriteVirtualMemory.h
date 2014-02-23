/*------------------------------------------------------------------------
* ���W�� : NtWriteVirtualMemory.h                                                 
* �sĶ���� : WDK 7600.16385.1
/-------------------------------------------------------------------------
* File Name            : NtWriteVirtualMemory.h 
* Building environment : WDK 7600.16385.1
-------------------------------------------------------------------------*/

ULONG NtWriteVirtualMemory_CN;
ULONG NtWriteVirtualMemory_Addr;
ULONG NtWriteVirtualMemory_HookAddr;
UCHAR NtWriteVirtualMemory_Hook_Org_Mem[5]={0x00,0x00,0x00,0x00,0x00};

ULONG MyNtWriteVirtualMemory_JmpAddr;
ULONG MyNtWriteVirtualMemoryCall;

ULONG NtWriteVirtualMemory_Hook_Calc;
UCHAR NtWriteVirtualMemory_Hook_Mem[5]={0xe9,0x00,0x00,0x00,0x00};

//////////////////////////////////////

__declspec(naked) NTSTATUS __stdcall MyNtWriteVirtualMemory()
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
 mov  eax,[MyNtWriteVirtualMemory_JmpAddr]
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
  call MyNtWriteVirtualMemoryCall
  jmp  [MyNtWriteVirtualMemory_JmpAddr] 
  }
 }
}

VOID NtWriteVirtualMemory_Hook()
{
 //���o��Ʀ�l Win7_x86 NtWriteVirtualMemory�ǦC���� 0x18F
 //NtWriteVirtualMemory_CN = 0x18F;

//�ʺA���o��Ʀ�l
 NtWriteVirtualMemory_CN = GetFunctionId("NtWriteVirtualMemory");
DbgPrint("[NtWriteVirtualMemory_CN] : 0x%x\n",NtWriteVirtualMemory_CN);
 NtWriteVirtualMemory_Addr = FindOriAddress(NtWriteVirtualMemory_CN);
DbgPrint("[NtWriteVirtualMemory_Addr] : 0x%08X\n",NtWriteVirtualMemory_Addr);

 //Jmp�I
 MyNtWriteVirtualMemory_JmpAddr = NtWriteVirtualMemory_Addr + 0xC;
DbgPrint("[MyNtWriteVirtualMemory_JmpAddr] : 0x%08X\n",MyNtWriteVirtualMemory_JmpAddr);

 //Hook�I
 NtWriteVirtualMemory_HookAddr = NtWriteVirtualMemory_Addr + 2;
DbgPrint("[NtWriteVirtualMemory_HookAddr] : 0x%08X\n",NtWriteVirtualMemory_HookAddr);

 //�p��CALL
 MyNtWriteVirtualMemoryCall = NtWriteVirtualMemory_Addr + 8;
 MyNtWriteVirtualMemoryCall = Find_CurAddr_OriData(MyNtWriteVirtualMemoryCall); 
 MyNtWriteVirtualMemoryCall = (ULONG)*((PULONG)MaxByte)-(ULONG)MyNtWriteVirtualMemoryCall;   
 MyNtWriteVirtualMemoryCall = MyNtWriteVirtualMemory_JmpAddr-MyNtWriteVirtualMemoryCall-0x1;  
DbgPrint("[MyNtWriteVirtualMemoryCall] : 0x%08X\n",MyNtWriteVirtualMemoryCall);

 //�ƻsHook�I��l���e
 WPOFF();
 RtlCopyMemory (NtWriteVirtualMemory_Hook_Org_Mem, (PVOID)NtWriteVirtualMemory_HookAddr , 5);
 RtlCopyMemory((PVOID)MyNtWriteVirtualMemory,NtWriteVirtualMemory_Hook_Org_Mem,5); 
 WPON();
DbgPrint("[MyNtWriteVirtualMemory] : 0x%08X\n",(PVOID)MyNtWriteVirtualMemory);
 
 ////�p��Jmp�a�},�æX��Jmp���O
 NtWriteVirtualMemory_Hook_Calc = (PCHAR)MyNtWriteVirtualMemory - (PCHAR)NtWriteVirtualMemory_HookAddr - 5;
 RtlCopyMemory(NtWriteVirtualMemory_Hook_Mem + 1,&NtWriteVirtualMemory_Hook_Calc,4);

 //Hook
 WPOFF();
 RtlCopyMemory((PVOID)NtWriteVirtualMemory_HookAddr,(PVOID)NtWriteVirtualMemory_Hook_Mem,5);
 WPON();

 DbgPrint("NtWriteVirtualMemory Hook Success!\n");

}

VOID NtWriteVirtualMemory_UnHook()
{
  WPOFF();
  RtlCopyMemory((PVOID)NtWriteVirtualMemory_HookAddr,NtWriteVirtualMemory_Hook_Org_Mem,5);
  WPON(); 
  DbgPrint("NtWriteVirtualMemory UnHook Success!\n");
}
