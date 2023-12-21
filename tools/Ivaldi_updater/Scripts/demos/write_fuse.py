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
/Scripts/demos/write_fuse.py

Brief
+++++
** Test script for blhost and sdphost wrappers. Writes a fuse to device. **

.. versionadded:: 0.0

"""

import sys
import Ivaldi.blhost as blhost
import Ivaldi.sdphost as sdphost
import Ivaldi.helpers as helpers
import time
import base64

FUSE = 'GP3_0'

def main():
    """
        Write 0x80000001 fuse GP3_0

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

    #print('Get list of named fuses...')

    #bl.efuse_get_available_fuses()

    print('Reading fuse %s...' % FUSE)
    read_resp_1 = bl.efuse_read(fuse=FUSE)
    if read_resp_1['ret']:
        print('ERROR: Could not read fuse!')
        sys.exit(1)
    else:
        ser_num = helpers.encode_unique_id(read_resp_1['response'])
        print('SUCCESS: Device fuse %s reads %s bytes as %s' %(FUSE, read_resp_1['response'][0], hex(read_resp_1['response'][1])))

    print('Writing to fuse ...')
    write_resp = bl.efuse_program(fuse=FUSE, data='80000001')
    if write_resp['ret']:
        print('ERROR: Could not write fuse %s!' % FUSE)
        sys.exit(1)
    else:
        print('... fuse write successful.')

    print('Reading back fuse %s...' % FUSE)
    read_resp_2 = bl.efuse_read(fuse=FUSE)
    if read_resp_2['ret']:
        print('ERROR: Could not read fuse %s!' % FUSE)
        sys.exit(1)
    else:
        ser_num = helpers.encode_unique_id(read_resp_2['response'])
        print('SUCCESS: Device fuse %s reads %s bytes as %s' %(FUSE, read_resp_2['response'][0], hex(read_resp_2['response'][1])))

if __name__ == '__main__':
    main()
