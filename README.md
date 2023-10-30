# HelloWorldDriver

This repository details how to start writing your first ever Windows Driver. The main motivation for this has been to overcome the dread of venturing into the Kernel space and gradually learn Windows Kernel Exploitation techniques. 

## Prerequisite

For building and testing the Driver we need to have a couple of things:

-  Two windows Machines: One to develop the driver on, and another to test it on (this better be a Virtual Machine)
- On the Development machine - follow the steps listed [here](https://learn.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk) to have the right SDK and WDK.
- Have the `DebugView` 

## Creating A WDM Project

The first thing we need to do is to start a WDM project in Visual Studio:
- Open Visual Studio and Create a new Project
- Search the "Empty WDM Driver" template and select it

    ![Alt text](<imgs/WDM template.png>)

- Give your project the name you want and choose the location where you want to create it. For this example, I chose the name `HelloWorldDriver`
- You should have your template ready! 

    > Note that under `Driver Files`, you would find a file called `HelloWorldDriver.inf`, which can be used, with some tweaking, to install the resultant driver, but since we wont be installing the driver using it, feel free to get rid of it. 

- Finally, under the `Source Files`, add a `HelloWorld.c`file to store the Driver code.

    > Also, for this project, I would recommend setting Warning Levels to `Wall` instead of `W4` by going to Project Properties->C/C++->General->Warning Level

## Code Walkthrough

The code for the driver looks as follows:

```c
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
	if (g_RegPath.Buffer == NULL) {
		DbgPrint("Error allocating memory!\n");
		return STATUS_NO_MEMORY;
	}

	// Copy Registry Path
	memcpy(g_RegPath.Buffer, RegistryPath->Buffer,RegistryPath->Length);
	g_RegPath.Length = g_RegPath.MaximumLength = RegistryPath->Length;
	DbgPrint("Parameter Key copy: %wZ\n", g_RegPath);

	// Unload Function
	DriverObject->DriverUnload = UnloadMe;
	return STATUS_SUCCESS;
}

void UnloadMe(PDRIVER_OBJECT DriverObject) {
	UNREFERENCED_PARAMETER(DriverObject);
	ExFreePool(g_RegPath.Buffer);
	DbgPrint("Bye Bye from HelloWorld Driver\n");
}
```

The PoC code outlines some basic concepts of Driver deployment including Memory Allocation, programming disciplines, etc. 

Start off with the _main_ function: `DriverEntry()`:

```c
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
	DbgPrint("HelloWorld from the Kernel Land!\n");
	DbgPrint("Driver Object:\t\t0x%p\n", DriverObject);
	DbgPrint("Registry Path:\t\t0x%p\n", RegistryPath);

	// Allocate memory for variable
	g_RegPath.Buffer = (PWSTR)ExAllocatePool2(POOL_FLAG_PAGED, RegistryPath->Length, DRIVER_TAG);
	if (g_RegPath.Buffer == NULL) {
		DbgPrint("Error allocating memory!\n");
		return STATUS_NO_MEMORY;
	}

	// Copy Registry Path
	memcpy(g_RegPath.Buffer, RegistryPath->Buffer,RegistryPath->Length);
	g_RegPath.Length = g_RegPath.MaximumLength = RegistryPath->Length;
	DbgPrint("Parameter Key copy: %wZ\n", g_RegPath);

	// Unload Function
	DriverObject->DriverUnload = UnloadMe;
	return STATUS_SUCCESS;
}
```

Before we begin, there is one important thing to note:
- Ususally when we write Userland code, the standard include files are: `Windows.h`, `stdio.h`, etc.
- Instead, for Drivers we include `ntddk.h` which defines the functions which we would be using in this PoC.

Moving onto the `DriverEntry()` function - it is analogous to the `main()` function and serves as an entrypoint to the driver. It is responsible for initializing the driver. The entry point function has a strict function definition: 

- It must return a `NTSTATUS` to indicate if the function succeeded or failed, and in case of the latter, also provide some information about the cause of the failure. 
- The function name: `DriverEntry`. If you are writing a driver in C++, make sure that function name is not mangled.
- The function parameters:
	- `PDRIVER_OBJECT DriverObject`: A pointer to a `DRIVER_OBJECT` structure that represents the driver's WDM driver object. Detailing the individual memembers of the structure is beyond the scope of this blog, but we would discuss the some of them as we come across them.
	- `PUNICODE_STRING RegistryPath`: A pointer to a UNICODE_STRING structure that specifies the path to the driver's Parameters key in the registry. The driver's Parameters key can contain configuration information for your driver.


Moving onto the main function body, we are first greeted with a bunch of `DbgPrint()` functions which look very similar to our good friend `printf()`, and to be honest, it mostly works the same except for floating points and some IRQL stuff (Read more [here](https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint)). Unlike `printf()` which prints to console, The `DbgPrint()` routine sends a message to the kernel debugger. We use the `DbgPrint()` function to print the address of the function parameters. 

Next up, we move to the memory allocation part of the driver. In the driver code, we have a global variable `g_RegPath` of the type `UNICODE_STRING` where we would store a copy of the Parameter key aka the `RegistryPath` so that we can share it among other functions. This is similar to the times in Usermode programming when we allocate memory on the heap to share structures across functions. Whereas in user mode, we use good ol' `malloc()`, in DriverLand we have the `ExAllocatePool2()` function.


