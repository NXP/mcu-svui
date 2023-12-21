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
/Scripts/sln_iot_utils/file_format.py

Brief
+++++
** Convert a file for the file system on HyperFlash or QSPI type flash **

.. versionadded:: 0.0

"""
import sys
import argparse
import Ivaldi.helpers as helpers

def main():
    """
        Convert an input file to the filesystem defined in sln_flash_mgmt module from the embedded device.
        Therea are 2 available filesystems for 2 types of flash memories: H (HyperFlash) and Q (QSPI).

        :returns: None
    """

    """ Parse the provided parameters """
    parser = argparse.ArgumentParser()
    parser.add_argument('-if', '--in-file', required=True, type=str, help="Specify input file name")
    parser.add_argument('-of', '--out-file', required=True, type=str, help="Specify output file name")
    parser.add_argument('-ft', '--flash-type', required=True, type=str, help="Specify board`s flash type. Add [-ft H] for HyperFlash and [-ft Q] for QSPI")
    args = parser.parse_args()

    if args.flash_type == 'H':
        print('Converting for HyperFlash')
        helpers.file_format(args.in_file, args.out_file, "HyperFlash")
    elif args.flash_type == 'Q':
        print('Converting for QSPI')
        helpers.file_format(args.in_file, args.out_file, "QSPI")
    else:
        print('Error: Flash type ' + args.flash_type + ' is not acceptable')
        sys.exit(1)

if __name__ == '__main__':
    main()
