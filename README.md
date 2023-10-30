# HelloWorldDriver

This repository details how to start writing your first ever Windows Driver. The main motivation for this has been to overcome the dread of venturing into the Kernel space and gradually learn Windows Kernel Exploitation techniques. 

## Prerequisite

For building and testing the Driver we need to have a couple of things:

-  Two windows Machines: One to develop the driver on, and another to test it on (this better be a Virtual Machine)
- On the Development machine - follow the steps listed [here](https://learn.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk) to have the right SDK and WDK.

## Creating A WDM Project

The first thing we need to do is to start a WDM project in Visual Studio:
- Open Visual Studio and Create a new Project
- Search the "Empty WDM Driver" template and select it

    ![Alt text](<imgs/WDM template.png>)

- Give your project the name you want and choose the location where you want to create it. 
- You should have your template ready! 

