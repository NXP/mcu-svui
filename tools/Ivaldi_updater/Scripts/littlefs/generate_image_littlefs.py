#!/usr/bin/env python3

'''

Copyright 2022-2023 NXP.

NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance with the
license terms that accompany it. By expressly accepting such terms or by downloading, installing,
activating and/or otherwise using the software, you are agreeing that you have read, and that you
agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
applicable license terms, then you may not retain, install, activate or otherwise use the software.

'''
import os
import argparse
import binascii
import sys
from littlefs import LittleFS

def littlefs_init(filesystemSize, blockSize, pageSize):
    global fs
    blockCount = int(filesystemSize/blockSize)
    print ("blocksize = " + str(blockSize) + ", block count = " + str(blockCount) + ", pagesize = " + str(pageSize))
    fs = LittleFS(block_size=blockSize, block_count=blockCount)

def littlefs_save(inFilePath, littlefsPath, attributes = 0):
    print("Saving file " + inFilePath + " in littlefs as " + littlefsPath)

    binary_array = []

    try:
        with open(inFilePath, "r") as f:
            contents = f.read()
            f.close()
    except UnicodeDecodeError:
        with open(inFilePath, "rb") as f:
            contents = f.read()
            f.close()

    for c in contents:
        try:
            binary_array.append(ord(c))
        except TypeError:
            binary_array.append(c)
    binary_array.append(0)

    # Create dir path
    dirs = littlefsPath.split('/')

    for i in range(len(dirs) - 1):
        if dirs[i] != '':
            try:
                if i == 0:
                    littlefs_mkdir(dirs[i])
                else:
                    # starting from the second folder in the chain, we have to recreate the path
                    dir_path = dirs[0]
                    for j in range(1, i+1):
                        dir_path += "/" + dirs[j]
                    littlefs_mkdir(dir_path)
            except Exception as e:
                pass # the exception could be printed here using print(e) for debugging purposes

    # Open a file and write some content
    with fs.open(littlefsPath, 'wb') as fh:
        fh.write(bytes(binary_array))

def littlefs_generate_bin(outPath):
    # Dump the filesystem content to a file
    print("Generating littlefs bin at address " + outPath)

    with open(outPath, 'wb') as fh:
        fh.write(fs.context.buffer)

def littlefs_mkdir(path):
    fs.mkdir(path)

def main():
    """ Parse the provided parameters """
    parser = argparse.ArgumentParser()

    parser.add_argument('-cf', '--config-folder', required=True, type=str, help="Specify the folder that contains board_config.py and files_list.py")
    parser.add_argument('-d', '--destination', default="./", type=str, help="Path to the folder where to store the binaries. Default path is to the current folder")
    parser.add_argument('-ivd', '--image-verification-disable', action='store_true', help="Disable Image Verification")

    args = parser.parse_args()

    global board_config

    """ Import file list that contains the files that need to be loaded """
    try:
        sys.path.append(args.config_folder)
        print('Importing files_list.py from ' + args.config_folder + ' folder')
        from littlefs_file_list import fileDictionary
    except ImportError:
        print('ERROR: Could not import files_list.py file from ' + args.config_folder + ' folder')
        sys.exit(1)
    """ Import board_config.py that contains the platform specs """
    try:
        print('Importing board_config.py from ' + args.config_folder + ' folder')
        sys.path.append(args.config_folder)
        import board_config
    except ImportError:
        print('ERROR: Could not import board_config.py file from ' + args.config_folder + ' folder')
        sys.exit(2)

    littlefs_init(int(board_config.FILE_SYS_SIZE, base = 16), int(board_config.SECTOR_SIZE, base = 16), int(board_config.PAGE_SIZE, base = 16))

    if board_config.PLATFORM_NAME == 'sln_svui_iot' or board_config.PLATFORM_NAME == 'sln_svui_rd':
        import littlefs_file_list
        fileDictionary = littlefs_file_list.create_file_list(args.config_folder, args.image_verification_disable)

    for files in fileDictionary:
        littlefs_save(inFilePath=files[0], littlefsPath=files[1])

    littlefs_generate_bin(args.destination + "littlefs.bin")

if __name__ == '__main__':
    main()