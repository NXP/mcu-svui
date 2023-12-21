#!/usr/bin/env python3

'''

Copyright 2019-2021 NXP.

This software is owned or controlled by NXP and may only be used strictly in accordance with the
license terms that accompany it. By expressly accepting such terms or by downloading, installing,
activating and/or otherwise using the software, you are agreeing that you have read, and that you
agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
applicable license terms, then you may not retain, install, activate or otherwise use the software.

'''
import os
import argparse
import binascii
import sys
import Ivaldi.helpers as helpers

def main():
    """ Parse the provided parameters """
    parser = argparse.ArgumentParser()
    parser.add_argument('-cn', '--ca-name', required=True, type=str, help="Specify CA name")
    parser.add_argument('-ft', '--flash-type', required=True, type=str, help="Specify board`s flash type. Add [-ft H] for HyperFlash, [-ft Q] for QSPI or [-ft ALL] for both")
    parser.add_argument('-d', '--destination', default="./", type=str, help="Path to the folder where to store the binaries. Default path is to the current folder")
    args = parser.parse_args()

    if args.flash_type not in ['H', 'Q', 'ALL']:
        print('Error: Flash type ' + args.flash_type + ' is not acceptable')
        sys.exit(-1)

    if not os.path.isdir(args.destination):
        print('Error: Path "' + args.destination + '" does not point to a folder')
        sys.exit(-1)

    if args.flash_type == 'H' or args.flash_type == 'ALL':
        print('Generating bins for HyperFlash')
        helpers.file_format('ca/certs/' + args.ca_name + '.root.ca.crt.pem', os.path.join(args.destination, 'ca_crt.bin'), "HyperFlash")
        helpers.file_format('ca/certs/' + args.ca_name + '.app.a.crt.pem', os.path.join(args.destination, 'app_crt.bin'), "HyperFlash")

    if args.flash_type == 'Q' or args.flash_type == 'ALL':
        print('Generating bins for QSPI')
        helpers.file_format('ca/certs/' + args.ca_name + '.root.ca.crt.pem', os.path.join(args.destination, 'ca_crt_qspi.bin'), "QSPI")
        helpers.file_format('ca/certs/' + args.ca_name + '.app.a.crt.pem', os.path.join(args.destination, 'app_crt_qspi.bin'), "QSPI")


if __name__ == '__main__':
    main()
