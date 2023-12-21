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
/Scripts/sln_svui_iot_secure_boot/oem/setup_hab.py

Brief
+++++
** Setup High Assurance Boot (HAB). **

.. versionadded:: 0.0

"""

import os
import sys
import logging

import Ivaldi.cst as cst
import Ivaldi.elftosb as elftosb
import Ivaldi.helpers as hlpr

# logging.basicConfig(level=logging.DEBUG)

TOP_DIR = "../../.."
WORK_DIR = TOP_DIR + "/work_dir"
CRT_DIR = TOP_DIR + "/crts"
KEY_DIR = TOP_DIR + "/keys"
# Must be Relative to KEY_DIR
KEY_SCRIPT_DIR = "../Tools/cst/utils"

TABLE_PATH = KEY_DIR + "/SRK_1_2_3_4_table.bin"
FUSE_PATH = KEY_DIR + "/SRK_1_2_3_4_fuse.bin"
SRK_LIST = [CRT_DIR + "/SRK1_sha256_2048_65537_v3_ca_crt.pem"
            , CRT_DIR + "/SRK2_sha256_2048_65537_v3_ca_crt.pem"
            , CRT_DIR + "/SRK3_sha256_2048_65537_v3_ca_crt.pem"
            , CRT_DIR + "/SRK4_sha256_2048_65537_v3_ca_crt.pem"]
EN_HAB_PATH = WORK_DIR + "/enable_hab.bd"
SRK_PATH = KEY_DIR + "/SRK_1_2_3_4_fuse.bin"
IMG_DIR = TOP_DIR + "/Image_Binaries"
HAB_SB_PATH = IMG_DIR + "/enable_hab.sb"
BD_DIR = TOP_DIR + "/Tools/bd_file"
LD_DIR = TOP_DIR + "/Flashloader"
LD_SREC = "flashloader.srec"
SIGN_LD_BD_PATH = BD_DIR + "/imx-dtcm-signed.bd"
SIGN_LD_BIN_PATH = IMG_DIR + "/ivt_flashloader_signed.bin"


def gen_enable_hab(srk_path, sb_path):
    """
        Generate enable_hab.sb from SRKs

        :param srk_path: path to generated hab enable file
        :returns: (bool) status
    """
    ret_val = int(-1)

    with open(srk_path, 'rb') as fsrk, open(sb_path, 'w') as fsb:
        byte_arr = fsrk.read()

        fsb.write("sources {\n")
        fsb.write("}\n")
        fsb.write("\n")
        fsb.write("constants {\n")
        fsb.write("}\n")
        fsb.write("\n")
        fsb.write("section (0) {\n")
        for i in range(8):
            idx = 4*i
            big_end = hlpr.swap_endian(byte_arr[idx:idx+4], 4)
            fsb.write("    load fuse 0x{} > 0x{:02X};\n".format((big_end).hex(), i + 0x18))
        fsb.write("\n")
        fsb.write("    # Program SEC_CONFIG to enable HAB closed mode\n")
        fsb.write("    load fuse 0x{:08X} > 0x{:02X};\n".format(0x00000002, 0x06))
        fsb.write("}\n")

        ret_val = int(0)

    return ret_val

def main():
    """
        Configures PKI (Certificates and Keys) for signing HAB images

        NOTE: Output is populated in the /keys and /crts directories

        NOTE: These files should be backed up as they will be tied to target devices through fuses.

        :returns: None
    """

    # Instantiate container
    cntr = cst.Cst()

    ans = "NA"
    while ans[0] not in ('y', 'n'):
        print("\nThis operation will delete all previous keys. Continue? [y,n]")
        ans = sys.stdin.readline()

    if ans[0] == 'n':
        sys.exit(0)

    # Check existance of key and certificate directories
    if not os.path.exists(KEY_DIR):
        os.mkdir(KEY_DIR)

    if not os.path.exists(CRT_DIR):
        os.mkdir(CRT_DIR)

    print("Cleaning keys and certificate directories...")
    try:
        for item in os.listdir(KEY_DIR):
            if item != ".gitignore":
                os.remove(KEY_DIR + '/' + item)
    except:
        print("ERROR: Unable to remove {}".format(KEY_DIR + '/' + item))
        sys.exit(1)

    try:
        for item in os.listdir(CRT_DIR):
            if item != ".gitignore":
                os.remove(CRT_DIR + '/' + item)
    except:
        print("ERROR: Unable to remove {}".format(CRT_DIR + '/' + item))
        sys.exit(1)
    print("SUCCESS: Cleaned keys and certificate directories...")

    print('Generating PKI tree...')
    if cntr.gen_pki(KEY_SCRIPT_DIR, KEY_DIR)['ret']:
        print('ERROR: Unable to generate PKI tree.')
        sys.exit(1)
    else:
        print('SUCCESS: Created PKI tree.')

    print('Generating Super Root Keys (SRK)s...')
    if cntr.gen_srk(TABLE_PATH
                    , FUSE_PATH
                    , SRK_LIST)['ret']:
        print('ERROR: Unable to generate SRKs.')
        sys.exit(1)
    else:
        print('SUCCESS: Generated SRKs.')

    print('Generating boot directive file to enable HAB...')
    if gen_enable_hab(SRK_PATH, EN_HAB_PATH):
        print('ERROR: Unable to generate boot directive file.')
        sys.exit(1)
    else:
        print('SUCCESS: Generated boot directive file.')

    print('Generating secure boot(.sb) file to enable HAB...')
    e2sb = elftosb.Elftosb()
    if e2sb.srk_sb(EN_HAB_PATH, HAB_SB_PATH)['ret']:
        print('ERROR: Unable to create secure boot app file.')
        sys.exit(1)
    else:
        print('SUCCESS: Created secure boot file to enable HAB.')

    print('Cryptographically signing flashloader image ...')
    if e2sb.sign_srec(SIGN_LD_BD_PATH
                    , SIGN_LD_BIN_PATH
                    , LD_DIR
                    , LD_SREC
                    )['ret']:
        print('ERROR: Unable to create signed flashloader image.')
        sys.exit(1)
    elif os.stat(SIGN_LD_BIN_PATH).st_size == 0:
        # cst, a subprocess of elftosb, may fail without proper error propagation to elftosb
        print('ERROR: Zero size flashloader image.')
        sys.exit(1)
    else:
        print('SUCCESS: Created signed flashloader image.')

if __name__ == '__main__':
    main()
