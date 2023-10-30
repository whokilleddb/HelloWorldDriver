#include <ntddk.h>
#define DRIVER_TAG 'hwdb'

UNICODE_STRING g_RegPath;

void UnloadMe(PDRIVER_OBJECT);

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
	DbgPrint("HelloWorld from the Kernel Land!\n");
	DbgPrint("Driver Object:\t\t0x%p\n", DriverObject);
	DbgPrint("Registry Path:\t\t0x%p\n", RegistryPath);

	// Allocate memory for variable
	g_RegPath.Buffer = (PWSTR)ExAllocatePool2(POOL_FLAG_PAGED, RegistryPath->Length, DRIVER_TAG);

	// Copy Registry Path
	memcpy(g_RegPath.Buffer, RegistryPath->Buffer,RegistryPath->Length);
	g_RegPath.Length = g_RegPath.MaximumLength = RegistryPath->Length;

	// Unload Function
	DriverObject->DriverUnload = UnloadMe;
	return STATUS_SUCCESS;
}

void UnloadMe(PDRIVER_OBJECT DriverObject) {
	UNREFERENCED_PARAMETER(DriverObject);
	DbgPrint("Bye Bye from HelloWorld Driver\n");
	ExFreePool(g_RegPath.Buffer);
}