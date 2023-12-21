#!/usr/bin/env python3

"""

Copyright 2022 NXP.

NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
activating and/or otherwise using the software, you are agreeing that you have read, and that you
agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
applicable license terms, then you may not retain, install, activate or otherwise use the software.

File
++++
/Scripts/sln_svui_iot_secure_boot/manf/lock_device.py

Brief
+++++
** Permanently locks device to disable debugger access. **

.. versionadded:: 0.0

"""

import sys
import time

import Ivaldi.blhost as blhost
import Ivaldi.sdphost as sdphost
import Ivaldi.helpers as helpers
import logging

#logging.basicConfig(level=logging.DEBUG)

IMG_DIR = '../../../Image_Binaries'
LD_PATH = IMG_DIR + '/ivt_flashloader_signed.bin'

sdp = sdphost.SDPHost('0x1fc9', '0x135')

# Check communication with device
print('Establishing connection...')
if sdp.error_status()['ret']:
    print('ERROR: Could not establish communication with device. Power cycle device.')
    sys.exit(1)
else:
    print('SUCCESS: Communication established with device.')

# Load flashloader onto device
print('Loading flashloader...')
if sdp.write_file('0x20000000', LD_PATH)['ret']:
    print('ERROR: Could not write file!')
    sys.exit(1)
else:
    print('SUCCESS: Flashloader loaded successfully.')

# Jump to flash loader entry point
print('Jumping to flashloader entry point...')
if sdp.jump_to_address('0x20000400')['ret']:
    print('ERROR: Could not jump to address!')
    sys.exit(1)
else:
    print('SUCCESS: Device jumped to execute flashloader.')

bl = blhost.BLHost()

# Poll device to make sure it is ready
print('Waiting for device to be ready for blhost...')
waitCount = 0
while bl.get_property('0x01')['ret']:
    time.sleep(0.5)
    waitCount += 1
    if waitCount == 10:
        print('ERROR: Timeout waiting for device to be ready. Power cycle device and try again.')
        sys.exit(1)

print('SUCCESS: Device is ready for blhost!')

print('Reading device unique ID...')
prop = bl.get_property('0x12')
ser_num = ''
if prop['ret']:
    print('ERROR: Could not read device unique ID!')
    sys.exit(1)
else:
    ser_num = helpers.encode_unique_id(prop['response'])
    print('SUCCESS: Device serial number is %s' %(ser_num))


# Program efuse CFG5 bit 20     (SJC DISABLE)
# Program efuse CFG5 bit 23 22  (JTAG_SMODE =11)
# Program efuse CFG5 bit 26     (Kill trace)
# blhost -u -- efuse-program-once 6 04D00000

# Program efuse CFG5 with 0x04D00000
print('Programming efuse CFG5...')
if bl.efuse_program(fuse="CFG5", data='04D00000')['ret']:
    print('ERROR: Unable to program efuse CFG5!')
    sys.exit(1)
else:
    print('SUCCESS: Programmed efuse CFG5.')

# Program efuse LOCK bit 3 (BOOT_CFG_LOCK)
# blhost -u -- efuse-program-once 0 00000008

# Program efuse LOCK with 0x00000008
print('Programming efuse BOOT CFG LOCK...')
if bl.efuse_program(fuse="LOCK", data='00000008')['ret']:
    print('ERROR: Unable to program efuse BOOT CFG LOCK!')
    sys.exit(1)
else:
    print('SUCCESS: Programmed efuse BOOT CFG LOCK.')

print('Resetting device...')
if bl.reset()['ret']:
    print('ERROR: Could not reset device!')
    sys.exit(1)
else:
    print('SUCCESS: Device Permanently Locked From Debugger Access!')
