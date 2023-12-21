#!/usr/bin/env python3

'''

Copyright 2019, 2022 NXP.

This software is owned or controlled by NXP and may only be used strictly in accordance with the
license terms that accompany it. By expressly accepting such terms or by downloading, installing,
activating and/or otherwise using the software, you are agreeing that you have read, and that you
agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
applicable license terms, then you may not retain, install, activate or otherwise use the software.

'''

import base64
import subprocess
import sys
import os
import math
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-p", "--prefix", type=str, nargs='?', default='', help="Prefix for main_app and bootloader binaries")
parser.add_argument("-m", "--main_app", type=str, nargs='?', default='', help="Name of main app (ais_ffs_demo, local_commands_demo, etc.)")
parser.add_argument("-bl", "--bootloader", type=str, nargs='?', default='bootloader', help="Name of the bootloader binary")
parser.add_argument("-a", "--certa", type=str, nargs='?', default='', help="Name of signing entity for bank a [must be defined]")
parser.add_argument("-b", "--certb", type=str, nargs='?', default='', help="Name of signing entity for bank b [same as bank a if not defined]")
parser.add_argument("-c", "--certc", type=str, nargs='?', default='', help="Name of signing entity for bootloader [same as bank a if not defined]")
parser.add_argument("-bc", "--board_config", type=str, nargs='?', default=None, help="Name of the folder containing board_config.py")
args = parser.parse_args()

print("Generating signed package and fica_table for %s" % args.prefix)

FILE_PREFIX = args.prefix
MAIN_APP = args.main_app
BANK_A_ENTITY = args.certa
BANK_B_ENTITY = args.certb
BANK_C_ENTITY = args.certc
BOARD_CONFIG = args.board_config

if BANK_A_ENTITY == '':
    print("ERROR: No bank a certificate set!")
    sys.exit(1)

if MAIN_APP == '':
    print("ERROR: No main app name set!")
    sys.exit(1)

if BANK_B_ENTITY == '':
    BANK_B_ENTITY += args.certa

if BANK_C_ENTITY == '':
    BANK_C_ENTITY += args.certa

print(FILE_PREFIX)
print(MAIN_APP)
print(BANK_A_ENTITY)
print(BANK_B_ENTITY)
print(BANK_C_ENTITY)

MAIN_APP_LEN = 0
BOOTLOADER_LEN = 0

try:
    MAIN_APP_LEN = os.path.getsize(FILE_PREFIX + MAIN_APP + '.bin')
except OSError:
    print("ERROR: unable to find " + MAIN_APP + " binary!")
    sys.exit(1)

try:
    BOOTLOADER_LEN = os.path.getsize(FILE_PREFIX + args.bootloader + '.bin')
except OSError:
    print("ERROR: unable to find bootloader binary!")
    sys.exit(1)

cmd1 = ['python', 'sign_me.py', FILE_PREFIX + MAIN_APP + '.bin', BANK_A_ENTITY]
out = subprocess.run(cmd1, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
if out.returncode == 0:
    print('SUCCESS: ' + str(cmd1))
else:
    print("ERROR: sign_me failed!")
    print(str(out.stdout.strip(), 'utf-8', 'ignore'))
    print(str(out.stderr.strip(), 'utf-8', 'ignore'))
    sys.exit(1)

# TODO
#cmd2 = ['python', 'sign_me.py', FILE_PREFIX + MAIN_APP + '.bin', BANK_B_ENTITY]
#out = subprocess.run(cmd2, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

cmd3 = ['python', 'sign_me.py', FILE_PREFIX + args.bootloader + '.bin', BANK_C_ENTITY]
out = subprocess.run(cmd3, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
if out.returncode == 0:
    print('SUCCESS: ' + str(cmd3))
else:
    print("ERROR: sign_me failed!")
    print(str(out.stdout.strip(), 'utf-8', 'ignore'))
    print(str(out.stderr.strip(), 'utf-8', 'ignore'))
    sys.exit(1)

if BOARD_CONFIG == None:
    cmd4 = ['python', 'fica_maker.py', str(BOOTLOADER_LEN), './output/' + FILE_PREFIX + args.bootloader + '.bin.sha256', str(MAIN_APP_LEN), './output/' + FILE_PREFIX + MAIN_APP + '.bin.sha256']
else:
    cmd4 = ['python', 'fica_maker.py', str(BOOTLOADER_LEN), './output/' + FILE_PREFIX + args.bootloader + '.bin.sha256', str(MAIN_APP_LEN), './output/' + FILE_PREFIX + MAIN_APP + '.bin.sha256', BOARD_CONFIG]
out = subprocess.run(cmd4, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
if out.returncode == 0:
    print('SUCCESS: ' + str(cmd4))
else:
    print("ERROR: sign_me failed!")
    print(str(out.stdout.strip(), 'utf-8', 'ignore'))
    print(str(out.stderr.strip(), 'utf-8', 'ignore'))
    sys.exit(1)

sys.exit(0)
