#!/usr/bin/env python3

'''

Copyright 2019-2020 NXP.

This software is owned or controlled by NXP and may only be used strictly in accordance with the
license terms that accompany it. By expressly accepting such terms or by downloading, installing,
activating and/or otherwise using the software, you are agreeing that you have read, and that you
agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
applicable license terms, then you may not retain, install, activate or otherwise use the software.

'''

import binascii
import sys

BOOTLOADER_TYPE = 0
APP_A_TYPE = 1
APP_B_TYPE = 2

fica_table = []
fica_records = []

if len(sys.argv) < 5:
    print("Usage: python fica_maker.py bootLen bootSigFile appALen appASigFile\n")
    sys.exit(1)

bootLen = int(sys.argv[1])
bootSigFile = sys.argv[2]
appALen = int(sys.argv[3])
appASigFile = sys.argv[4]

# Default image addresses
BOOTLOADER_OFFSET = '0x00040000'
APP_A_OFFSET      = '0x00300000'
APP_B_OFFSET      = '0x00D00000'

# Default FICA version
FICA_VERSION = '0x02'

# In case a board_config was provided, overwrite default image addresses
if len(sys.argv) == 6:
    sys.path.append("../../" + sys.argv[5])
    import board_config

    BOOTLOADER_OFFSET = board_config.BOOTLOADER_OFFSET
    APP_A_OFFSET      = board_config.APP_A_OFFSET
    APP_B_OFFSET      = board_config.APP_B_OFFSET
    FICA_VERSION      = board_config.FICA_VERSION

def int_to_bytes(data, dataList):
    shift_count = 0
    while (shift_count < 32):
        dataList.append(((data >> shift_count) & 0xFF))
        shift_count += 8

def write_record(imgType, imgLen, imgSigFile, record):
    global BOOTLOADER_OFFSET
    global APP_A_OFFSET
    global APP_B_OFFSET

    # Add descriptor
    int_to_bytes(int('0x5A5A5A5A', 16), record)

    # Add image type
    int_to_bytes(imgType, record)

    # Add image address
    if imgType == BOOTLOADER_TYPE:
        int_to_bytes(int(BOOTLOADER_OFFSET, 16), record)
    elif imgType == APP_A_TYPE:
        int_to_bytes(int(APP_A_OFFSET, 16), record)
    elif imgType == APP_B_TYPE:
        int_to_bytes(int(APP_B_OFFSET, 16), record)

    # Add image length
    int_to_bytes(imgLen, record)

    # Add image format
    int_to_bytes(int('0x00', 16), record)

    # Add image hash type
    int_to_bytes(int('0x00', 16), record)

    # Add image hash loc
    int_to_bytes(int('0x00', 16), record)

    # Add image enc type
    int_to_bytes(int('0x00', 16), record)

    # Add img pki type
    int_to_bytes(int('0x00', 16), record)

    # Add img pki loc
    int_to_bytes(int('0x00', 16), record)

    # Add img signature
    if imgSigFile:
        with open(imgSigFile, 'r+b') as f:
            try:
                data = ord(f.read(1))
                while data != b"":
                    record.append(data)
                    print(data)
                    data = ord(f.read(1))
            except TypeError:
                pass
            f.close()
    else:
        # Fill with zeros
        for n in range(256):
            record.append(int('0x00', 16))

    # Add reserved padding 12 bytes
    int_to_bytes(int('0x00', 16), record)
    int_to_bytes(int('0x00', 16), record)
    int_to_bytes(int('0x00', 16), record)

# Add descriptor
int_to_bytes(int('0xA5A5A5A5', 16), fica_table)

# Add Version
int_to_bytes(int(FICA_VERSION, 16), fica_table)

# Add default communication field
int_to_bytes(int('0x30', 16), fica_table)

# Add default 'current application' type
int_to_bytes(int('0x01', 16), fica_table)

# Add default 'new application' type
int_to_bytes(int('0xFFFFFFFF', 16), fica_table)

# Add default 'boot' type
int_to_bytes(int('0x00', 16), fica_table)

# Boot record
boot_record = []

# Write bootloader record
write_record(BOOTLOADER_TYPE, bootLen, bootSigFile, boot_record)

# Append boot record to records
fica_records.append(boot_record)

# Boot record
app_a_record = []

# Write app a record record
write_record(APP_A_TYPE, appALen, appASigFile, app_a_record)

# Append boot record to records
fica_records.append(app_a_record)

# Boot record
empty_b_record = []

# Write app b record record
write_record(APP_B_TYPE, 0, None, empty_b_record)

# Append boot record to records
fica_records.append(empty_b_record)

# Save FICA to a binary file
with open('fica_table.bin', "w+b") as f:
    table = bytearray(fica_table)
    f.write(table)

    for r in fica_records:
        record = bytearray(r)
        f.write(record)

    f.close()
