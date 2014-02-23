/*------------------------------------------------------------------------
* ���W�� : Driver.h                                                   
* �sĶ���� : WDK 7600.16385.1
/-------------------------------------------------------------------------
* File Name            : Driver.c
* Building environment : WDK 7600.16385.1
-------------------------------------------------------------------------*/

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
#include <NTDDK.h>
#ifdef __cplusplus
}
#endif 

#define PAGEDCODE code_seg("PAGE")
#define LOCKEDCODE code_seg()
#define INITCODE code_seg("INIT")

#define PAGEDDATA data_seg("PAGE")
#define LOCKEDDATA data_seg()
#define INITDATA data_seg("INIT")

#define arraysize(p) (sizeof(p)/sizeof((p)[0]))

typedef struct _DEVICE_EXTENSION {
	PDEVICE_OBJECT pDevice;
	UNICODE_STRING ustrDeviceName;	//�]�ƦW��
	UNICODE_STRING ustrSymLinkName;	//�Ÿ��챵�W
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

// ����n��

NTSTATUS CreateDevice (IN PDRIVER_OBJECT pDriverObject);
VOID HelloDDKUnload (IN PDRIVER_OBJECT pDriverObject);
NTSTATUS HelloDDKDispatchRoutine(IN PDEVICE_OBJECT pDevObj,
								 IN PIRP pIrp);