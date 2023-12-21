#!/usr/bin/env python3

"""

Copyright 2019 NXP.

This software is owned or controlled by NXP and may only be used strictly in accordance with the
license terms that accompany it. By expressly accepting such terms or by downloading, installing,
activating and/or otherwise using the software, you are agreeing that you have read, and that you
agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
applicable license terms, then you may not retain, install, activate or otherwise use the software.

File
++++
/Scripts/demos/erase.py

Brief
+++++
** Script to erase i.mx RT device hyperflash. **

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
        Erase 0x2000000 bytes of flash starting at address 0x60000000

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

    print('Reading device unique ID...')
    prop = bl.get_property('0x12')
    ser_num = ''
    if prop['ret']:
        print('ERROR: Could not read device unique ID!')
        sys.exit(1)
    else:
        ser_num = helpers.encode_unique_id(prop['response'])
        print('SUCCESS: Device serial number is %s' %(ser_num))

    # Write config option block to RAM
    print('Writing memory config option block...')
    if bl.fill_memory('0x2000', '0x4', '0xc0333006')['ret']:
        print('ERROR: Could not fill memory!')
        sys.exit(1)
    else:
        print('SUCCESS: Config option block loaded into RAM.')

    # Configure FlexSPI
    print('Configuring FlexSPI...')
    if bl.configure_memory('0x9', '0x2000')['ret']:
        print('ERROR: Could not configure memory!')
        sys.exit(1)
    else:
        print('SUCCESS: FlexSPI configured.')

    # Erase flash
    print('Erasing flash...')
    if bl.flash_erase_region('0x60000000', '0x2000000')['ret']:
        print('ERROR: Could not erase memory!')
        sys.exit(1)
    else:
        print('SUCCESS: Flash erased.')

    sys.exit(0)

if __name__ == '__main__':
    main()
