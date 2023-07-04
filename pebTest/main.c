#pragma warning( disable : 4305 4100)
#include "main.h"

ULONG64 G_NT_BASE = 0xfffff80333800000;
ULONG64 G_NT_OFFSET = 0xC8BC;
ULONG64 G_PTE_BASE;
ULONG64 G_PDE_BASE;
ULONG64 G_PPE_BASE;
ULONG64 G_PXE_BASE;


PULONG64 GetPxeAddress(PVOID addr)
{
	return (PULONG64)(((((ULONG64)addr & 0xffffffffffff) >> 39) << 3) + G_PXE_BASE);
}

PULONG64 GetPpeAddress(PVOID addr)
{
	return (PULONG64)(((((ULONG64)addr & 0xffffffffffff) >> 30) << 3) + G_PPE_BASE);
}

PULONG64 GetPdeAddress(PVOID addr)
{
	return (PULONG64)(((((ULONG64)addr & 0xffffffffffff) >> 21) << 3) + G_PDE_BASE);
}

PULONG64 GetPteAddress(PVOID addr)
{
	return (PULONG64)(((((ULONG64)addr & 0xffffffffffff) >> 12) << 3) + G_PTE_BASE);
}



void DriverUnload(PDRIVER_OBJECT driver )
{
	DbgPrint("driver is unloading....\r\n");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING RegistryPath )
{
	DbgPrint("Driver Start!\r\n");
	driver->DriverUnload = DriverUnload;

	PLDR_DATA_TABLE_ENTRY pDriverList;
	PLIST_ENTRY pCurrentList;

	pDriverList = (PLDR_DATA_TABLE_ENTRY)(driver->DriverSection);
	pCurrentList = (PLIST_ENTRY)pDriverList;

	UNICODE_STRING targetStr;

	RtlInitUnicodeString(&targetStr, L"ntoskrnl.exe");

	

	

	while (((PLIST_ENTRY)pDriverList)->Blink != pCurrentList)
	{  
		UNICODE_STRING currentStr;
		RtlInitUnicodeString(&currentStr, (pDriverList->BaseDllName).Buffer);
		    
			if (RtlCompareUnicodeString(&targetStr, &currentStr, FALSE)==0)
			{
				DbgPrint("DriverBase=0x%08X        \tDriverSize 0x%08X      \tDriverName=%ws\n",
					pDriverList->DllBase,
					pDriverList->SizeOfImage,
					(pDriverList->BaseDllName).Buffer);
				G_NT_BASE = pDriverList->DllBase;
				DbgPrint("NTBase:%p \n",G_NT_BASE);
				break;
			}

		pDriverList = ((PLIST_ENTRY)pDriverList)->Blink;
	}



	G_PTE_BASE = *(PULONG64)(G_NT_BASE+G_NT_OFFSET);
	G_PDE_BASE = (ULONG64)GetPteAddress((PVOID)G_PTE_BASE);
	G_PPE_BASE = (ULONG64)GetPteAddress((PVOID)G_PDE_BASE);
	G_PXE_BASE = (ULONG64)GetPteAddress((PVOID)G_PPE_BASE);

	ULONG64 testPtr = 0xfffff80336690fb0;

	DbgPrint("PXE:%p\n",G_PXE_BASE);
	DbgPrint("PPE:%p\n", G_PPE_BASE);
	DbgPrint("PDE:%p\n", G_PDE_BASE);
	DbgPrint("PTE:%p\n", G_PTE_BASE);
	DbgPrint("=======================\n", G_PXE_BASE);
	DbgPrint("PXE:%p\n", GetPxeAddress(testPtr));
	DbgPrint("PPE:%p\n", GetPpeAddress(testPtr));
	DbgPrint("PDE:%p\n", GetPdeAddress(testPtr));
	DbgPrint("PTE:%p\n", GetPteAddress(testPtr));
	return STATUS_SUCCESS;
}