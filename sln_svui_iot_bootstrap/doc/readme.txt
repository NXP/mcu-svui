Overview
========


Toolchain supported
===================
- MCUXpresso  11.8.0

Hardware requirements
=====================
- USB-C Cable
- SLN-SVUI-IOT Development Kit
- Personal Computer
- SEGGER J-Link
- USB/UART converter (optional - this is used to check the logs)

Board settings
==============
Ensure J61 is set to position '12' (i.e. is not set to serial downloader mode)

Connect the board with PC by using USB/UART converter:
- board uart RX (pin 6 on J64) - connect it to TX pin on converter
- board uart TX (pin 8 on J64) - connect it to RX pin on converter
- board GND (pin 15 on J64)

Prepare the Demo
================
1. Connect a USB cable between the host PC and the USB-C port on the target board. 
2. Connect SEGGER J-Link to the 10-pin SWD connector on bottom of target board.
3. Connect USB/UART converter as described above.
4. Open a serial terminal with the following settings:
    - 115200 baud rate
    - 8 data bits
    - No parity
    - One stop bit
    - No flow control
5. Download the program to the target board.
6. Either power cycle the board or launch the debugger in your IDE to begin running the demo.

Running the demo
================
The log below shows the output of this demo in the terminal window:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*** BOOTSTRAP v0.0.1 ***

BOOTSTRAP v0.0.1 > ERROR: JTAG active violation!
BOOTSTRAP v0.0.1 > Running NON encrypted XIP.
BOOTSTRAP v0.0.1 > Attempting to jump to 0x60040451
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Version and jump address may vary.

NOTE: If debugger is attached, a security violation will be present in the log. This is expected when debugging.