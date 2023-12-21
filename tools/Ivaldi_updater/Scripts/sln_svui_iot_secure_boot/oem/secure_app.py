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
/Scripts/sln_svui_iot_secure_boot/oem/secure_app.py

Brief
+++++
** Secure application for execution with High Assurance Boot (HAB). **

.. versionadded:: 0.0

"""

import sys
import os
import logging
import argparse

import Ivaldi.elftosb as elftosb
import Ivaldi.helpers as hlpr

def main():
    """
        Generate a secured binary file based on provided binary images.
    """
    global board_config

    """ Parse the provided parameters """
    parser = argparse.ArgumentParser()
    parser.add_argument("-s", "--signed-only", action="store_true"
                        , help="Only sign rather than encrypt app")
    parser.add_argument("--bin", action="store_true"
                        , help="App hab file in 'bin' format rather than 'srec'")
    parser.add_argument("uvoice_hab", nargs='?', default=None, help="path to Srec file")
    parser.add_argument("bootloader",  nargs='?',default=None, help="path to bin file")
    parser.add_argument("uvoice_app",  nargs='?',default=None, help="path to bin file")
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

    TOP_DIR = "../../.."
    IMG_DIR = TOP_DIR + "/Image_Binaries"
    BD_SIGN_IMAGE = TOP_DIR + "/Tools/bd_file/imx-flexspinor-normal-signed.bd"
    BD_GEN_SIGN_SB = TOP_DIR + '/Tools/bd_file/' + board_config.BD_GEN_SIGN_SB_FILE
    BD_GEN_CRYPT_SB = TOP_DIR + '/Tools/bd_file/' + board_config.BD_GEN_CRYPT_SB_FILE

    DEF_HAB_PATH  = IMG_DIR + '/' + board_config.BOOTSTRAP_SREC
    DEF_BOOT_PATH  = IMG_DIR + '/' + board_config.BOOTLOADER_BIN
    DEF_APP_PATH  = IMG_DIR + '/' + board_config.APP_A_BIN

    if args.uvoice_hab != None:
        uvoice_hab_path = args.uvoice_hab
    else:
        uvoice_hab_path = DEF_HAB_PATH

    if args.bootloader != None:
        bootloader_path = args.bootloader
    else:
        bootloader_path = DEF_BOOT_PATH

    if args.uvoice_app != None:
        uvoice_app_path = args.uvoice_app
    else:
        uvoice_app_path = DEF_APP_PATH

    if args.signed_only:
        sec_boot_file = "boot_sign_image.sb"
        bd_gen_sb = BD_GEN_SIGN_SB
    else:
        sec_boot_file = "boot_crypt_image.sb"
        bd_gen_sb = BD_GEN_CRYPT_SB

    sec_boot_path = IMG_DIR + '/' + sec_boot_file

    if not os.access(uvoice_hab_path, os.R_OK):
        logging.error("Unable to read uvoice_hab_path: %s", uvoice_hab_path)
        sys.exit(1)

    # Need trailing '/' to handle paths of form
    # "../../../../<file_name>" as dirname removes
    # trailing '/'
    uvoice_hab_dir = os.path.dirname(uvoice_hab_path) + '/'
    uvoice_hab_file = os.path.basename(uvoice_hab_path)
    uvoice_hab_name = os.path.splitext(uvoice_hab_file)[0]

    uvoice_hab_image_path = IMG_DIR + '/' + uvoice_hab_name + "_signed.bin"
    uvoice_hab_image_nopad_path = IMG_DIR + '/' + uvoice_hab_name + "_signed_nopadding.bin"


    if not os.access(IMG_DIR, os.W_OK):
        logging.error("Unable to write secure boot app directory: %s", IMG_DIR)
        sys.exit(1)

    if os.path.exists(sec_boot_path) and not os.access(sec_boot_path, os.W_OK):
        logging.error("Unable to write secure boot app file: %s", sec_boot_path)
        sys.exit(1)


    e2sb = elftosb.Elftosb()

    # Create signed image
    print('{} app image ...'.format("Cryptographicly signing" if args.signed_only else "Encrypting"))
    if e2sb.sign_srec(  BD_SIGN_IMAGE
                    , uvoice_hab_image_path
                    , uvoice_hab_dir
                    , uvoice_hab_file
                    )['ret']:

        print('ERROR: Unable to create {} image.'.format("signed" if args.signed_only else "encrypted"))
        sys.exit(1)

    elif os.stat(uvoice_hab_image_path).st_size == 0 or os.stat(uvoice_hab_image_nopad_path).st_size == 0:
        # cst, a subprocess of elftosb, may fail without proper error propagation to elftosb
        print('ERROR: Zero size {} image.'.format("signed" if args.signed_only else "encrypted"))
        sys.exit(1)
    else:
        print('SUCCESS: Created {} image.'.format("signed" if args.signed_only else "encrypted"))

    # Create secure boot file
    print('Creating {} app file ...'.format("signed" if args.signed_only else "encrypted"))

    if e2sb.create_sb(  bd_gen_sb
                    , sec_boot_path
                    , IMG_DIR
                    , [uvoice_hab_image_nopad_path, bootloader_path, uvoice_app_path]
                    )['ret']:

        print('ERROR: Unable to create {} boot app file.'.format("signed" if args.signed_only else "encrypted"))
        sys.exit(1)
    else:
        print('SUCCESS: Created {} app file.'.format("signed" if args.signed_only else "encrypted"))

if __name__ == '__main__':
    main()
