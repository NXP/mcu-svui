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
1. Make sure that a valid 'bootstrap' is loaded onto target board.
2. Connect a USB cable between the host PC and the USB-C port on the target board. 
3. Connect SEGGER J-Link to the 10-pin SWD connector on bottom of target board.
4. Connect USB/UART converter as described above.
5. Open a serial terminal with the following settings:
    - 115200 baud rate
    - 8 data bits
    - No parity
    - One stop bit
    - No flow control
6. Download the program to the target board.
7. Either power cycle the board or launch the debugger in your IDE to begin running the demo.

Running the demo
================
The log below shows the output of this demo in the terminal window:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
0 0 [BOOTLOADER_Task]

*** BOOTLOADER v0.0.1 ***

1 0 [BOOTLOADER_Task] [FICA] VERSION 3
2 0 [BOOTLOADER_Task] [FICA] RT Flash Check...
3 0 [BOOTLOADER_Task] [FICA] Found Flash device!
4 0 [BOOTLOADER_Task] [FICA] Checking Image Config Area (ICA) initialization
5 0 [BOOTLOADER_Task] [FICA] Flash ICA already initialized
6 0 [BOOTLOADER_Task] [FICA] Flash ICA initialization complete
7 0 [BOOTLOADER_Task] Jumping to main application...
8 0 [BOOTLOADER_Task] [FICA] VERSION 3
9 0 [BOOTLOADER_Task] [FICA] RT Flash Check...
10 0 [BOOTLOADER_Task] [FICA] Found Flash device!
11 1 [BOOTLOADER_Task] [FICA] Checking Image Config Area (ICA) initialization
12 1 [BOOTLOADER_Task] [FICA] Flash ICA already initialized
13 1 [BOOTLOADER_Task] [FICA] Flash ICA initialization complete
14 1 [BOOTLOADER_Task] [FICA] VERSION 3
15 1 [BOOTLOADER_Task] [FICA] RT Flash Check...
16 1 [BOOTLOADER_Task] [FICA] Found Flash device!
17 1 [BOOTLOADER_Task] [FICA] Checking Image Config Area (ICA) initialization
18 1 [BOOTLOADER_Task] [FICA] Flash ICA already initialized
19 1 [BOOTLOADER_Task] [FICA] Flash ICA initialization complete
20 1 [BOOTLOADER_Task] Remapping enabled. Running from 0x60C00000
21 1 [BOOTLOADER_Task] Launching into application at 0x60200000
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Version and jump address may vary.

NOTE: A Crypto context failure log may be shown.