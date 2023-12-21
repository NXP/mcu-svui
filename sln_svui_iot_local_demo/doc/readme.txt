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

Board settings
==============
Ensure J61 is set to position '12' (i.e. is not set to serial downloader mode)

Demos to test
================
1. Elevator
2. Smart home
3. Washing machie

Languages to test
================
1. English
2. French
3. German
4. Chinese

Prepare the Demo
================
1. Make sure that a valid 'bootstrap' and 'bootloader' are loaded onto target board.
2. Connect a USB cable between the host PC and the USB-C port on the target board.
3. Download the program to the target board.
4. Either power cycle the board or launch the debugger in your IDE to begin running the demo.

Running the demo - VIT
================
1. Open the shell interface using a serial terminal application (MobaXterm, teraterm, PuTTY).
2. Use 'commands' to see a list of available commands.
3. Say the default English wake-word 'Hey NXP'.
4. When detected, the RGB LED turns blue.
5. Say any command from the commands list.
6. An audio prompt is played after the command is detected.
7. To change the language, use 'changelang' command in shell or say the wake-word 'Hey NXP' followed by 'Change language' voice command.
8. To change the demo, use 'changedemo' command in shell or say the wake-work 'Hey NXP' followed by 'Change demo' voice command.
9. Use 'help' for more information.

Running the demo - DSMT
================
1. Open the shell interface using a serial terminal application (MobaXterm, teraterm, PuTTY).
2. Use 'commands' to see a list of available commands.
3. Say one of the supported wake-words 'Hey NXP', 'Hallo NXP', 'Salut NXP', '你好， 恩智浦'.
4. When detected, the RGB LED turns blue.
5. Say any command from the commands list (matching the language of the chosen wake-word).
6. An audio prompt is played after the command is detected.
7. Four languages are active in parallel for DSMT. The language can be changed by saying one of the
   supported wake-words ('Hey NXP', 'Hallo NXP', 'Salut NXP', '你好， 恩智浦') or by using 'changelang' command in shell.
8. To change the demo, use 'changedemo' command in shell or say the wake-work 'Hey NXP' followed by 'Change demo' voice command.
9. Use 'help' for more information.