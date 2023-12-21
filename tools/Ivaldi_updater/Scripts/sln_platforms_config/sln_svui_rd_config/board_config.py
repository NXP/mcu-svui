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
/Scripts/sln_platforms_config/sln_svui_rd_config/board_config.py

Brief
+++++
** Defines the board config of the SLN-SVUI-RD project. **

.. versionadded:: 0.0

"""

PLATFORM_NAME        = 'sln_svui_rd'
"""Platform name"""

PLATFORM_PREFIX      = PLATFORM_NAME + '_'
"""Platform prefix"""

BOOTSTRAP_NAME       = 'bootstrap'
"""Bootstrap name without platform prefix"""

BOOTLOADER_NAME      = 'bootloader'
"""Bootloader name without platform prefix"""

MAIN_APP_NAME        = 'local_demo_vit'
"""Main application name without platform prefix"""

SECOND_APP_NAME       = 'local_demo_dsmt'
"""Second application name without platform prefix"""

ROOT_FOLDER          = 'sln_platforms_config/' + PLATFORM_PREFIX + 'config/'
"""Current folder name"""

BOOTSTRAP_BIN        = PLATFORM_PREFIX + BOOTSTRAP_NAME + '.bin'
"""Bootstrap binary file name"""

BOOTSTRAP_SREC       = PLATFORM_PREFIX + BOOTSTRAP_NAME + '.srec'
"""Bootstrap srec file name"""

BOOTLOADER_BIN       = PLATFORM_PREFIX + BOOTLOADER_NAME + '.bin'
"""Bootloader binary file name"""

APP_A_BIN            = PLATFORM_PREFIX + MAIN_APP_NAME + '.bin'
"""Main application binary file name"""

APP_B_BIN            = PLATFORM_PREFIX + SECOND_APP_NAME + '.bin'
"""Second application binary file name"""

ROOT_CERT_BIN        = 'ca_crt_qspi.bin'
"""Root certificate file name"""

APP_A_CERT_BIN       = 'app_crt_qspi.bin'
"""Main application certificate file name"""

BD_GEN_SIGN_SB_FILE  = 'program_flexspinor_image_qspi.bd'
"""BD file to be used for secure programming"""

BD_GEN_CRYPT_SB_FILE = 'svui_rd_program_flexspinor_image_qspi_encrypt.bd'
"""BD file to be used for secure and eXIP programming"""

AUDIO_DIR = '../../Image_Binaries/svui_audio_files'
"""Path to audio files"""

FICA_VERSION                 = '0x03'
"""fica_definition.h version"""

FLASH_TYPE                   = 'QSPI'
"""Type of flash"""

FLASH_START_ADDR             = '0x60000000'
"""Start address of the flash"""

FLASH_SIZE                   = '0x01000000'
"""Flash size"""

PAGE_SIZE                    = '0x100'
"""PAGE size"""

SECTOR_SIZE                  = '0x1000'
"""Sector size"""

FILE_SYS_SIZE                = '0x400000'
"""Filesystem size"""

BOOTLOADER_OFFSET     = '0x00040000'
APP_A_OFFSET          = '0x00200000'
APP_B_OFFSET          = '0x00680000'
FILESYSTEM_OFFSET     = '0x00B00000'

BOOTSTRAP_ADDR        = '0x60000000'
BOOTLOADER_ADDR       = str(hex( int(FLASH_START_ADDR, 16) + int(BOOTLOADER_OFFSET, 16)))
APP_A_ADDR            = str(hex( int(FLASH_START_ADDR, 16) + int(APP_A_OFFSET, 16)))
APP_B_ADDR            = str(hex( int(FLASH_START_ADDR, 16) + int(APP_B_OFFSET, 16)))
FILESYSTEM_ADDR       = str(hex( int(FLASH_START_ADDR, 16) + int(FILESYSTEM_OFFSET, 16)))


FICA_TABLE_ADDR       = '0x60FFF000'

BANK_SIZE             = '0x00480000'
"""Bank A and Bank B size"""

OPTION_BLOCK_MASK     = '0xC0000006'
"""Memory config option"""

JUMPER_SERIAL_NR      = "J61"
"""Serial mode jumper ID"""

JUMPER_SERIAL_ON      = "23"
"""Serial mode jumper state ON"""

JUMPER_SERIAL_OFF     = "12"
"""Serial mode jumper state OFF"""
