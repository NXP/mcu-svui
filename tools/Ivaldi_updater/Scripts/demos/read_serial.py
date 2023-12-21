#!/usr/bin/env python3

"""

Copyright 2019, 2021 NXP.

This software is owned or controlled by NXP and may only be used strictly in accordance with the
license terms that accompany it. By expressly accepting such terms or by downloading, installing,
activating and/or otherwise using the software, you are agreeing that you have read, and that you
agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
applicable license terms, then you may not retain, install, activate or otherwise use the software.

File
++++
/Scripts/demos/read_serial.py

Brief
+++++
** Test script for blhost and sdphost wrappers. Read serial number from i.mx RT device. **

.. versionadded:: 0.0

"""

import sys
import Ivaldi.blhost as blhost
import Ivaldi.sdphost as sdphost
import Ivaldi.helpers as helpers
import time
import base64

def main():
    """
        Read serial number (unique ID) from device

        :returns: None
    """

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
    if sdp.write_file('0x20000000', '../../Flashloader/ivt_flashloader.bin')['ret']:
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

    # Read out unique ID
    print('Reading device unique ID...')
    prop = bl.get_property('0x12')
    ser_num = ''
    if prop['ret']:
        print('ERROR: Could not read device unique ID!')
        sys.exit(1)
    else:
        print('SUCCESS: Device serial number is:\r\n')

        ser_num = helpers.encode_unique_id(prop['response'])
        print('BASE64 format: %s' %(ser_num))

        ser_num = helpers.encode_unique_id_to_hex(prop['response'])
        print('HEX    format: %s' %(ser_num))

if __name__ == '__main__':
    main()
