Overview
========
Hello World Virtual Com demonstrates the use of virtual com to print the "Hello World" string to the terminal.
Different with Hello World demo (a HW UART peripheral is used to transmit data), the demo transmits data based on
a serial port simulated by the USB device stack on target board side via USB peripheral.
The purpose of this demo is to demonstrate how to use virtual com and provide a simple project for debugging and further development. 

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
1.  Connect a USB cable between the host PC and the USB-C port on the target board.
2.  Download the program to the target board.
3.  Either press the reset button on your board or launch the debugger in your IDE to begin running the demo.
4.  Open a serial terminal(like putty) to communicate with the board via virtual com.

Running the demo
================
The log below shows the output of this demo in the terminal window:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
hello world.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Terminal will echo back the received characters.