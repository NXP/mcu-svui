Overview
========
The LED Blinky demo application provides a sanity check for the new SDK build environments and board bring up. The LED Blinky demo 
uses the systick interrupt to realize the function of timing delay. The example takes turns to shine the LED. The purpose of this 
demo is to provide a simple project for debugging and further development.
The code of this demo has been prepared and updated for use with the MCUXpresso Configuration Tools (Pins/Clocks/Peripherals).

Toolchain supported
===================
- MCUXpresso  11.8.0

Hardware requirements
=====================
- USB-C Cable
- SLN-SVUI-IOT Development Kit
- Personal Computer
- SEGGER J-Link

Board settings
==============
Ensure J61 is set to position '12' (i.e. is not set to serial downloader mode)

Prepare the Demo
================
1. Connect a USB cable between the host PC and the USB-C port on the target board. 
2. Download the program to the target board.
3. Either press the reset button on your board or launch the debugger in your IDE to begin running the demo.

Running the demo
================
When the demo runs successfully, you will find the LED is blinking.