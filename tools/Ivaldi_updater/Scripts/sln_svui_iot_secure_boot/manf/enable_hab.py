#!/usr/bin/env python3

"""

Copyright 2022-2023 NXP.

NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
activating and/or otherwise using the software, you are agreeing that you have read, and that you
agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
applicable license terms, then you may not retain, install, activate or otherwise use the software.

File
++++
/Scripts/sln_svui_iot_secure_boot/manf/enable_hab.py

Brief
+++++
** Enables High Assurance Boot (HAB) on i.mx RT device. **

.. versionadded:: 0.0

"""

import sys
import os
import time
import base64
import argparse
import logging

import Ivaldi.blhost as blhost
import Ivaldi.sdphost as sdphost
import Ivaldi.helpers as hlpr

TOP_DIR = "../../.."
IMG_DIR = TOP_DIR + '/Image_Binaries'
LD_PATH = IMG_DIR + '/ivt_flashloader_signed.bin'
SB_HAB_PATH = IMG_DIR + '/enable_hab.sb'

def main():
    """
        Enables High Assurance Boot (HAB) on target i.MX RT Device

        :returns: None
    """
    global board_config

    """ Parse the provided parameters """
    parser = argparse.ArgumentParser()
    parser.add_argument('-cf', '--config-folder', default="../../sln_platforms_config/sln_svui_iot_config/", type=str, help="Specify the folder that contains board_config.py file")
    args = parser.parse_args()

    """ Import board_config.py that contains the platform specs """
    try:
        print('Importing board_config.py from ' + args.config_folder + ' folder')
        sys.path.append(args.config_folder)
        import board_config
    except ImportError:
        print('ERROR: Could not import board_config.py file from ' + args.config_folder + ' folder')
        sys.exit(1)

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
        ser_num = hlpr.encode_unique_id(prop['response'])
        print('SUCCESS: Device serial number is %s' %(ser_num))

    # Write config option block to RAM
    print('Writing memory config option block...')
    if bl.fill_memory('0x2000', '0x4', board_config.OPTION_BLOCK_MASK)['ret']:
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
    if bl.flash_erase_all()['ret']:
        print('ERROR: Could not erase memory!')
        sys.exit(1)
    else:
        print('SUCCESS: Flash erased.')

    # Load secure boot file to enable HAB
    print('Loading secure boot file...')
    if bl.receive_sb_file(SB_HAB_PATH)['ret']:
        print('ERROR: Could not load secure boot file!')
        sys.exit(1)
    else:
        print('SUCCESS: Loaded secure boot file.')

    print('Resetting device...')
    if bl.reset()['ret']:
        print('ERROR: Could not reset device!')
        sys.exit(1)
    else:
        print('SUCCESS: Device Permanently Locked with HAB!')

if __name__ == '__main__':
    main()
