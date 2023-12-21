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
/Scripts/sln_svui_iot_open_boot/open_prog_full.py

Brief
+++++
** Full suite script for SLN-SVUI-IOT project. Onboarding, Programming. **

.. versionadded:: 0.0

"""

import argparse
import os
import sys
import subprocess
import Ivaldi.blhost as blhost
import Ivaldi.sdphost as sdphost
import Ivaldi.helpers as helpers
import Ivaldi.onboard.aws as ob
import time
import base64
from shutil import copyfile

ser_num = ''

def main():
    """
        Based on board_config, performs the following operations:
            - Erases:
                - FLASH_SIZE bytes of flash starting at address FLASH_START_ADDR
            - Programs:
                - BOOTSTRAP_BIN                         @ BOOTSTRAP_ADDR
                - BOOTLOADER_BIN                        @ BOOTLOADER_ADDR
                - APP_A_BIN                             @ APP_A_ADDR
                - APP_B_BIN                             @ APP_B_ADDR

                - fica_table.bin                        @ FICA_TABLE_ADDR
                - Filesystem:
                    - Audio files
                    - Image CA Root Certificate         @ ROOT_CA_CERT_ADDR
                    - Bank A Signing Certificate        @ APP_A_CERT_ADDR
                    - Bootloader Signing Certificate    @ BOOTLOADER_CERT_ADDR

            - Signs:
                - BOOTLOADER_BIN
                - APP_A_BIN
                - APP_B_BIN

        :returns: None
    """
    global ser_num
    global board_config

    """ Parse the provided parameters """
    parser = argparse.ArgumentParser()
    parser.add_argument('-cf', '--config-folder', default="../sln_platforms_config/sln_svui_iot_config/", type=str, help="Specify the folder that contains board_config.py file")
    parser.add_argument('-ivd', '--image-verification-disable', action='store_true', help="Disable Image Verification")
    parser.add_argument('-fbb', '--flash-bank-b', action='store_true', help="Flash DSMT demo in bank B")
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
        print('\r\nERROR: Could not establish communication with device.\r\n')
        print('       Please check that Serial Mode Jumper, ' + board_config.JUMPER_SERIAL_NR + ', is in correct position: ' + board_config.JUMPER_SERIAL_ON)
        print('       If ' + board_config.JUMPER_SERIAL_NR + ' is in correct position, then power reset the board before re-running the script\r\n')
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
    if prop['ret']:
        print('ERROR: Could not read device unique ID!')
        sys.exit(1)
    else:
        temp_num = helpers.encode_unique_id(prop['response'])
        print('SUCCESS: Device serial number is %s' %(temp_num))
        # Make cloud safe thing name
        ser_num = helpers.prep_for_cloud_from_base64(temp_num)
        print('SUCCESS: Device thing name is %s' %(ser_num))

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

    # Write files to flash
    print('Programming bootstrap...')
    if not bl.write_memory(board_config.BOOTSTRAP_ADDR, '../../Image_Binaries/' + board_config.BOOTSTRAP_BIN)['status']:
        print('ERROR: Could not write file to memory!')
        sys.exit(1)
    else:
        print('SUCCESS: Bootstrap written to flash.')

    print('Programming bootloader...')
    if not bl.write_memory(board_config.BOOTLOADER_ADDR, '../../Image_Binaries/' + board_config.BOOTLOADER_BIN)['status']:
        print('ERROR: Could not write file to memory!')
        sys.exit(1)
    else:
        print('SUCCESS: Bootloader written to flash.')

    if not args.image_verification_disable:

        # Sign bootloader and application and generate fica table
        try:
            copyfile('../../Image_Binaries/' + board_config.APP_A_BIN, '../ota_signing/sign/' + board_config.APP_A_BIN)
        except:
            print('ERROR: Unable to copy ' + board_config.MAIN_APP_NAME)
            sys.exit(1)

        try:
            copyfile('../../Image_Binaries/' + board_config.BOOTLOADER_BIN, '../ota_signing/sign/' + board_config.BOOTLOADER_BIN)
        except:
            print('ERROR: Unable to copy bootloader!')
            sys.exit(1)

        # NOTE: End user will need to update the device signing entity used below (by default prod.app.a used)
        # python sign_package.py -p PLATFORM_PREFIX -a prod.app.a
        cmd = ['python', 'sign_package.py', '-p', board_config.PLATFORM_PREFIX, '-m', board_config.MAIN_APP_NAME, '-bl', board_config.BOOTLOADER_NAME, '-a', 'prod.app.a', '-bc', board_config.ROOT_FOLDER]
        out = subprocess.run(cmd, cwd='../ota_signing/sign', stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if out.returncode != 0:
            print(str(out.stdout.strip(), 'utf-8', 'ignore'))
            print(str(out.stderr.strip(), 'utf-8', 'ignore'))
            print('FICA generation failed')
            sys.exit(1)

        try:
            copyfile('../ota_signing/sign/fica_table.bin', '../../Image_Binaries/fica_table.bin')
        except:
            print('ERROR: Unable to copy fica table!')
            sys.exit(1)

        # Program FICA table
        print('Programming FICA table...')
        fica_path = '../../Image_Binaries/fica_table.bin'
        if not bl.write_memory(board_config.FICA_TABLE_ADDR, fica_path)['status']:
            print('ERROR: Could not program flash with FICA for this "thing"!')
            sys.exit(1)
        else:
            print('SUCCESS: Programmed flash with FICA for this "thing".')

    # Generate File-system
    env_python_bin = str(sys.executable).split('.')[0]

    if args.image_verification_disable:
        process = subprocess.Popen([env_python_bin, '../littlefs/generate_image_littlefs.py', '-cf', str(args.config_folder), '-ivd'], stdout=subprocess.DEVNULL)
    else:
        process = subprocess.Popen([env_python_bin, '../littlefs/generate_image_littlefs.py', '-cf', str(args.config_folder)], stdout=subprocess.DEVNULL)
    process.wait()
    std_out, std_err = process.communicate()
    if process.returncode != 0:
        print(str(std_out.strip(), 'utf-8', 'ignore'))
        print(str(std_err.strip(), 'utf-8', 'ignore'))
        print('File-system generation failed')
        sys.exit(1)

    try:
        copyfile('littlefs.bin', '../../Image_Binaries/'+ board_config.PLATFORM_PREFIX +'file_system.bin')
        os.remove('littlefs.bin')
    except:
        print('ERROR: Unable to copy file-system table!')
        sys.exit(1)

    # Program File-system
    print('Programming file-system at address...', board_config.FILESYSTEM_ADDR)
    file_system_path = '../../Image_Binaries/' + board_config.PLATFORM_PREFIX + 'file_system.bin'
    if not bl.write_memory(board_config.FILESYSTEM_ADDR, file_system_path)['status']:
        print('ERROR: Could not program flash with file-system for this "thing"!')
        sys.exit(1)
    else:
        print('SUCCESS: Programmed flash with file-system for this "thing".')

    # Program BankA
    print('Programming Application Bank A...')
    if not bl.write_memory(board_config.APP_A_ADDR, '../../Image_Binaries/' + board_config.APP_A_BIN)['status']:
        print('ERROR: Could not write file to memory!')
        sys.exit(1)
    else:
        print('SUCCESS: Application Bank A written to flash.')

    if args.flash_bank_b:
        # Program BankB
        print('Programming Application Bank B...')
        if not bl.write_memory(board_config.APP_B_ADDR, '../../Image_Binaries/' + board_config.APP_B_BIN)['status']:
            print('ERROR: Could not write file to memory!')
            sys.exit(1)
        else:
            print('SUCCESS: Application Bank B written to flash.')

    # Get app entry point
    read_entry_resp = bl.read_memory('0x60002004', '4')
    entry_pnt = 0
    if read_entry_resp['ret']:
        print('ERROR: Could ready memory!')
        sys.exit(1)
    else:
        entry_pnt = helpers.bytes_to_word(read_entry_resp['response'], 4)
        print('SUCCESS: Application entry point at %s' % (entry_pnt))

    # Get initial stack pointer
    read_sp_resp = bl.read_memory('0x60002000', '4')
    stack_pnt = 0
    if read_sp_resp['ret']:
        print('ERROR: Could ready memory!')
        sys.exit(1)
    else:
        stack_pnt = helpers.bytes_to_word(read_sp_resp['response'], 4)
        print('SUCCESS: Application entry point at %s' % (stack_pnt))

    # Execute application
    print('Attempting to execute application...')
    if bl.execute(entry_pnt, '0', stack_pnt)['ret']:
        print('ERROR: Could not execute application!')
        sys.exit(1)
    else:
        print('SUCCESS: Application running.\r\n\r\n')
        print('Please do the following actions:')
        print('1. Power off the board')
        print('2. Move the Serial Mode Jumper, ' + board_config.JUMPER_SERIAL_NR + ', in correct position for booting: ' + board_config.JUMPER_SERIAL_OFF)
        print('3. Power on the board\r\n')

if __name__ == '__main__':
    main()
