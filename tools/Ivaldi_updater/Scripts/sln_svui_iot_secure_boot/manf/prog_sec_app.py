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
/Scripts/sln_svui_iot_secure_boot/manf/prog_sec_app.py

Brief
+++++
** Programs secured app on NXP Voice Module. **

.. versionadded:: 0.0

"""

import sys
import os
import time
import argparse
import logging
import subprocess
from shutil import copyfile

import Ivaldi.blhost as blhost
import Ivaldi.sdphost as sdphost
import Ivaldi.helpers as helpers
import Ivaldi.onboard.aws as ob

TOP_DIR = "../../.."
IMG_DIR = TOP_DIR + '/Image_Binaries'
LD_PATH = IMG_DIR + '/ivt_flashloader_signed.bin'
SB_CRYPT_PATH = IMG_DIR + '/boot_crypt_image.sb'
SB_SIGN_PATH = IMG_DIR + '/boot_sign_image.sb'
FLDR_BIN_PATH = IMG_DIR + '/ivt_flashloader_signed.bin'
OUTPUT_DIR = TOP_DIR + '/Output'
PY3_DIR = TOP_DIR + '/Scripts'

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
                - APP_B_BIN               @ FICA_TABLE_ADDR
            - Programs secure binary with:
                - Bootstrap [Signed with HAB]
                - Bootloader [Encrypted]
                - Local Demo [Encrypted]
            
        :returns: None
    """
    # We will be assigning ser_num value in this function, applying global keyword to allow modification
    global ser_num
    global board_config

    """ Parse the provided parameters """
    parser = argparse.ArgumentParser()
    parser.add_argument("-s", "--signed-only", action="store_true", help="Run signed rather than encrypted app")
    parser.add_argument('-cf', '--config-folder', default="../../sln_platforms_config/sln_svui_iot_config/", type=str, help="Specify the folder that contains board_config.py file")
    parser.add_argument('-ivd', '--image-verification-disable', action='store_true', help="Disable Image Verification")
    args = parser.parse_args()

    """ Import board_config.py that contains the platform specs """
    try:
        print('Importing board_config.py from ' + args.config_folder + ' folder')
        sys.path.append(args.config_folder)
        import board_config
    except ImportError:
        print('ERROR: Could not import board_config.py file from ' + args.config_folder + ' folder')
        sys.exit(1)

    app_sb_path = SB_CRYPT_PATH if not args.signed_only else SB_SIGN_PATH

    if not os.access(app_sb_path, os.R_OK):
        logging.error("Unable to read {} boot app file: {}"
                    .format("signed" if args.signed_only else "encrypted", app_sb_path))
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
    if sdp.write_file('0x20000000', FLDR_BIN_PATH)['ret']:
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

    if not args.image_verification_disable:
        # Sign bootloader and application and generate fica table
        try:
            copyfile(IMG_DIR + '/' + board_config.APP_A_BIN, PY3_DIR + '/ota_signing/sign/' + board_config.APP_A_BIN)
        except:
            print('ERROR: Unable to copy ' + MAIN_APP_NAME)
            sys.exit(1)

        try:
            copyfile(IMG_DIR + '/' + board_config.BOOTLOADER_BIN, PY3_DIR + '/ota_signing/sign/' + board_config.BOOTLOADER_BIN)
        except:
            print('ERROR: Unable to copy bootloader!')
            sys.exit(1)

        # NOTE: End user will need to update the device signing entity used below (by default prod.app.a used)
        # python sign_package.py -p PLATFORM_PREFIX -a prod.app.a
        cmd = ['python', 'sign_package.py', '-p', board_config.PLATFORM_PREFIX, '-m', board_config.MAIN_APP_NAME, '-bl', board_config.BOOTLOADER_NAME, '-a', 'prod.app.a', '-bc', board_config.ROOT_FOLDER]
        out = subprocess.run(cmd, cwd=PY3_DIR + '/ota_signing/sign', stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if out.returncode != 0:
            print(str(out.stdout.strip(), 'utf-8', 'ignore'))
            print(str(out.stderr.strip(), 'utf-8', 'ignore'))
            print('FICA generation failed')
            sys.exit(1)

        try:
            copyfile(PY3_DIR + '/ota_signing/sign/fica_table.bin', IMG_DIR + '/fica_table.bin')
        except:
            print('ERROR: Unable to copy fica table!')
            sys.exit(1)

        # Program FICA table
        print('Programming FICA table...')
        fica_path = IMG_DIR + '/fica_table.bin'
        if not bl.write_memory(board_config.FICA_TABLE_ADDR, fica_path)['status']:
            print('ERROR: Could not program flash with FICA for this "thing"!')
            sys.exit(1)
        else:
            print('SUCCESS: Programmed flash with FICA for this "thing".')

    # Generate File-system
    env_python_bin = str(sys.executable).split('.')[0]

    if args.image_verification_disable:
        process = subprocess.Popen([env_python_bin, '../littlefs/generate_image_littlefs.py', '-cf', '../' + str(board_config.ROOT_FOLDER), '-ivd'], cwd='./..', stdout=subprocess.DEVNULL)
    else:
        process = subprocess.Popen([env_python_bin, '../littlefs/generate_image_littlefs.py', '-cf', '../' + str(board_config.ROOT_FOLDER)], cwd='./..', stdout=subprocess.DEVNULL)
    process.wait() 
    std_out, std_err = process.communicate()
    if process.returncode != 0:
        print(str(std_out.strip(), 'utf-8', 'ignore'))
        print(str(std_err.strip(), 'utf-8', 'ignore'))
        print('File-system generation failed')
        sys.exit(1)

    try:
        copyfile('../littlefs.bin', '../../../Image_Binaries/'+ board_config.PLATFORM_PREFIX +'file_system.bin')
        os.remove('../littlefs.bin')
    except:
        print('ERROR: Unable to copy file-system table!')
        sys.exit(1)

    # Program File-system
    print('Programming file-system at address...', board_config.FILESYSTEM_ADDR)
    file_system_path = '../../../Image_Binaries/' + board_config.PLATFORM_PREFIX + 'file_system.bin'
    if not bl.write_memory(board_config.FILESYSTEM_ADDR, file_system_path)['status']:
        print('ERROR: Could not program flash with file-system for this "thing"!')
        sys.exit(1)
    else:
        print('SUCCESS: Programmed flash with file-system for this "thing".')

    # Load secure app file
    print('Programming flash with secure app file...')
    if bl.receive_sb_file(app_sb_path)['ret']:
        print('ERROR: Could not program secure app file!')
        sys.exit(1)
    else:
        print('SUCCESS: Programmed flash with secure app file.')

    if args.signed_only:
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
            print('SUCCESS: Application running.')

    print('\nNOTE:Unpower module, move jumper ' + board_config.JUMPER_SERIAL_NR + ' from position ' + board_config.JUMPER_SERIAL_ON + ' (Serial mode on) to position ' + board_config.JUMPER_SERIAL_OFF + ' (Serial mode off)\n')

if __name__ == '__main__':
    main()
