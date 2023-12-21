"""

Copyright 2019-2021 NXP.

This software is owned or controlled by NXP and may only be used strictly in accordance with the
license terms that accompany it. By expressly accepting such terms or by downloading, installing,
activating and/or otherwise using the software, you are agreeing that you have read, and that you
agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
applicable license terms, then you may not retain, install, activate or otherwise use the software.

File
++++
Ivaldi/helpers/__init__.py

Brief
+++++
** helper functions for Ivaldi **

.. versionadded:: 0.0

API
+++

"""

import os.path
import platform
import base64
import binascii
import math
from crccheck.crc import Crc32Mpeg2

USB_TOOLS = ('sdphost', 'blhost')

def long_int_to_bytearray(long_int):
    """
        Converts an integer into a bytearray

        :param long_int: Integer to convert to bytearray
        :type long_int: Integer

        :returns: (bytearray) Byte array representation of integer
    """
    byte_arr = bytearray()
    while long_int:
        byte_arr.append(long_int & 0xFF)
        long_int >>= 8
    return byte_arr

def encode_unique_id_to_hex(resp_list):
    """
        Encodes device unique ID into hex string

        :param resp_list: List of hex strings that represent the unique ID, i.e, [0x1234ABCD, 0xABCD1234]
        :type resp_list: List

        :returns: (str) Hex string representation of unique ID
    """
    lwr = "{:08x}".format(resp_list[0])
    upr = "{:08x}".format(resp_list[1])

    return upr + lwr

def encode_unique_id(resp_list):
    """
        Encodes device unique ID into base64 string

        :param resp_list: List of hex strings that represent the unique ID, i.e, [0x1234ABCD, 0xABCD1234]
        :type resp_list: List

        :returns: (str) Base64 string representation of unique ID
    """
    lwr = hex(resp_list[0])[2:]
    upr = hex(resp_list[1])[2:]
    cmb = long_int_to_bytearray(int(upr + lwr, 16))
    return str(base64.b64encode(cmb), 'ascii', 'strict')

def prep_for_cloud_from_base64(inStr):
    """
        Modifies existing base64 input to be cloud friendly

        :param inStr: base64 input string to format
        :type inStr: String

        :returns: (str) Cloud friendly encoded string
    """

    try:
        # Test the string for base64 compatibility
        base64.b64decode(inStr)

        ret = ''
        for c in inStr:
            if c == '+':
                ret += '_'
            elif c == '/':
                ret += '-'
            elif c == '=':
                break #Break loop here this character is padding and we can leave function
            else:
                ret += c

        # Check if we had a padding character in an invalid place for single base64 string
        if len(ret) < (len(inStr) - 2):
            raise binascii.Error

        return ret
    except binascii.Error:
        print("ERROR: Invalid base64 encoding on input string.")
        return ''


def bytes_to_word(resp_list, num_bytes):
    """
        Converts a list of bytes into a hexadecimal word string

        :param resp_list: List of bytes, string hexadecimal without '0x' prefix
        :type resp_list: List
        :param num_bytes: Number of bytes to convert into a hex string
        :type num_bytes: Integer

        :returns: (str) Hexadecimal string representation of byte list with '0x' prefix
    """
    wrd = '0x'
    idx = num_bytes - 1
    while idx >= 0:
        wrd += resp_list[idx]
        idx -= 1
    return wrd


def swap_endian(byte_seq, num_bytes):
    """
    Function to extract and swap endianess
    of num_bytes bytes of byte sequence

    :param byte_seq: Input sequence of bytes
    :param num_bytes: Number of bytes to swap and return

    :returns: (bytearray) ret_seq

    """
    ret_seq = bytearray()
    idx = num_bytes - 1
    while idx >= 0:
        ret_seq.append(byte_seq[idx])
        idx -= 1
    return ret_seq


def isLinux():
    """
        Function to determine if executing on Linux platform

        :returns: (boolean)
    """
    # Check for absence of Microsoft to avoid WSL
    return 'Linux' == platform.system() and -1 == platform.release().find('Microsoft')


def isWSL():
    """
        Function to determine if executing on Windows Subsystem for Linux

        :returns: (boolean)
    """
    return 'Linux' == platform.system() and -1 < platform.release().find('Microsoft')

def isWindows():
    """
        Function to determine if executing on Windows platform

        :returns: (boolean)
    """
    return 'Windows' == platform.system()

def useExe(prog_name):
    """
        Function to determine if need ".exe" suffix on programs

        :returns: (boolean)
    """
    # Must use .exe for USB connection on WSL
    return isWindows() or isWSL() and prog_name in USB_TOOLS

def isMac():
    """
        Function to determine if executing on Macintosh platform

        :returns: (boolean)
    """
    return 'Darwin' == platform.system()

def script_dir():
    """
        Function to return path of directory containing file of presently executing script

        :returns: (string) Real path to script directory
    """

    return os.path.dirname(os.path.realpath(__file__))

def file_format(inFile, outFile, flash_type="HyperFlash"):
    """
        Formats a file for use with uvoice flash management

        :param inFile: Path to input file
        :type inFile: String
        :param outFile: Path to output file
        :type outFile: String

        :returns: None
    """

    contents = ""

    if flash_type == "QSPI":
        flash_page_size = 256.0
    elif flash_type == "HyperFlash":
        flash_page_size = 512.0
    else:
        flash_page_size = 512.0

    # Read original contents of file
    try:
        with open(inFile, "r") as f:
            contents = f.read()
            f.close()
    except UnicodeDecodeError:
        with open(inFile, "rb") as f:
            contents = f.read()
            f.close()

    # Create an array of the ord values of the file contents
    binary_array = []
    for c in contents:
        try:
            binary_array.append(ord(c))
        except TypeError:
            binary_array.append(c)

    # Append a null terminator to the file data
    binary_array.append(ord('\0'))

    # Calculate the number of pages the file will occupy
    page_count = int(math.ceil(len(binary_array) / flash_page_size))

    # Populate the file map to indicate the used pages
    file_map = []

    while page_count:
        file_map.append(int('AA', 16))
        page_count -= 1

    # Fill the remainder of the map with unused page code
    while len(file_map) < flash_page_size:
        file_map.append(int('FF', 16))

    # Populate file header with valid bit and file size

    '''
    uint32_t valid:1;         /*! Indicates this file is valid to read */
    uint32_t clean:1;         /*! Indicates this file has been updated in place, if 0 CRC is invalid in NVM */
    uint32_t sizeBytes:14;    /*! Limits to 16 KB file size */
    uint32_t isSize14bits:1;  /*! Indicates if the file size is on 14 bits (up to 16 KB file size) */
    uint32_t reserved:5;      /*! Padding */
    uint32_t extSizeBytes:10; /*! Up to 16 MB extended file size */
    uint32_t crc:32;          /*! 32 bits of CRC */
    '''

    file_header = []

    file_size = len(binary_array)
    print("File size %d" % file_size)

    # Calculate how many padding bytes we need for CRC32/MPEG-2 to match RT CRC32
    pad_num = 4 - (file_size % 4) if (file_size % 4) else 0
    while pad_num:
        binary_array.append(ord('\0'))
        pad_num -= 1

    file_crc32 = Crc32Mpeg2.calc(bytearray(binary_array)) # Why MPEG2? \\__(`_`)_//

    # Add valid, clean, plus size bits[1:6] fields
    file_header.append(int('0x03', 16) | ((file_size << 2) & int('FC', 16)))
    # Add size bits[7:14]
    file_header.append(((file_size << 2) & int('FF00', 16)) >> 8)

    # File size >= 16KB?
    if file_size >= 16 * 1024:
        # Reset isSize14bits bit, add 5 bits padding, plus size bits[15:16]
        file_header.append(int('0x00', 16) | (int('0x1F', 16) << 1) | ((file_size >> 8) & int('0xC0', 16)))
        # Add size bits[17:24]
        file_header.append((file_size >> 16) & int('0xFF', 16))
    else:
        # Add padding
        file_header.append((int('FF', 16)))
        file_header.append((int('FF', 16)))

    # Add CRC output CRC32/MPEG-2 in reverse order to match RT output
    file_header.append((file_crc32 & int('FF000000', 16)) >> 24)
    file_header.append((file_crc32 & int('FF0000', 16)) >> 16)
    file_header.append((file_crc32 & int('FF00', 16)) >> 8)
    file_header.append((file_crc32 & int('FF', 16)))

    print("File CRC 0x%s%s%s%s" % (hex(file_header[7])[2:], hex(file_header[6])[2:], hex(file_header[5])[2:], hex(file_header[4])[2:]))

    # Write the new binary file with meta data
    with open(outFile, "w+b") as f:
        bin_map = bytearray(file_map)
        f.write(bin_map)

        bin_hdr = bytearray(file_header)
        f.write(bin_hdr)

        binary_format = bytearray(binary_array)
        f.write(binary_format)
        f.close()
