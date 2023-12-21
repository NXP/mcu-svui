"""

Copyright 2019, 2021-2022 NXP.

This software is owned or controlled by NXP and may only be used strictly in accordance with the
license terms that accompany it. By expressly accepting such terms or by downloading, installing,
activating and/or otherwise using the software, you are agreeing that you have read, and that you
agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
applicable license terms, then you may not retain, install, activate or otherwise use the software.

File
++++
Ivaldi/blhost/__init__.py

Brief
+++++
** Ivaldi blhost wrapper **

.. versionadded:: 0.0

API
+++

"""

import os
import platform
import subprocess
import json
import logging
import re

import Ivaldi.helpers as hlpr
from Ivaldi.helpers import useExe

#logging.basicConfig(level=logging.DEBUG)

FUSE_MAP = \
{\
    'RT106A': \
    {\
        'LOCK': '0x00',\
        'CFG0': '0x01',\
        'CFG1': '0x02',\
        'CFG2': '0x03',\
        'CFG3': '0x04',\
        'CFG4': '0x05',\
        'CFG5': '0x06',\
        'CFG6': '0x07',\
        'MEM0': '0x08',\
        'MEM1': '0x09',\
        'MEM2': '0x0A',\
        'MEM3': '0x0B',\
        'MEM4': '0x0C',\
        'ANA0': '0x0D',\
        'ANA1': '0x0E',\
        'ANA2': '0x0F',\
        'SRK0': '0x18',\
        'SRK1': '0x19',\
        'SRK2': '0x1A',\
        'SRK3': '0x1B',\
        'SRK4': '0x1C',\
        'SRK5': '0x1D',\
        'SRK6': '0x1E',\
        'SRK7': '0x1F',\
        'SJC0': '0x20',\
        'SJC1': '0x21',\
        'MAC0': '0x22',\
        'MAC1': '0x23',\
        'MAC2': '0x24',\
        'GP1': '0x26',\
        'GP2': '0x27',\
        'SW_GP1': '0x28',\
        'SW_GP2_0': '0x29',\
        'SW_GP2_1': '0x2A',\
        'SW_GP2_2': '0x2B',\
        'SW_GP2_3': '0x2C',\
        'MISC_CONF0': '0x2D',\
        'MISC_CONF1': '0x2E',\
        'SRK_REVOKE': '0x2F',\
        'GP3_0': '0x38',\
        'GP3_1': '0x39',\
        'GP3_2': '0x3A',\
        'GP3_3': '0x3B',\
        'GP4_0': '0x3C',\
        'GP4_1': '0x3D',\
        'GP4_2': '0x3E',\
        'GP4_3': '0x3F',\
    }\
}

DEBUG_LOG = False

class BLHost(object):
    """
        BLHost Class


    """

    def __init__(self, dev='RT106A'):
        """
            Sets appropriate call to executable and target device.

            Default to use RT106A fuse mappping.

            :param dev: Target device type
            :type dev: String

            :returns: None
        """
        self._blhost = 'blhost' + self.__get_ext('blhost')
        self._fuse_map = FUSE_MAP[dev]

    def __get_ext(self, prog_name):
        """
            Private function to determine if an extension must be added to executable calls

            :returns: (str) Appropriate file extension, if needed
        """
        ext = '.exe' if useExe(prog_name) else ''
        return ext

    def __handle_return(self, ret, dbg):
        """
            Private function to handle executable call returns.

            Parses JSON and builds dictionary response

            :param ret: JSON Return string from executable call
            :type ret: JSON String
            :param dbg: Flag to turn on or off printing the response
            :type dbg: Boolean

            :returns: (dict) {'ret': <return code>, 'response': <response>, 'status': <status>}
        """
        respsonseStr = str(ret.stdout, 'utf-8', 'ignore')
        if dbg:
            print(respsonseStr)
        try:
            responseJson = json.loads(respsonseStr[respsonseStr.find('{'):])
            print(responseJson['command'])
            if responseJson['command'] == 'read-memory':
                # Read memory commands put read return values before JSON
                byte_read = respsonseStr[:respsonseStr.find('{')]
                resp_bytes = byte_read.replace('\r\n', ' ').replace('\r', '').split(' ')
                return {'ret': ret.returncode, 'response': resp_bytes, 'status': responseJson['status']}
            else:
                return {'ret': ret.returncode, 'response': responseJson['response'], 'status': responseJson['status']}
        except:
            return {'ret': ret.returncode, 'response': [], 'status': {}}

    def check_connection(self, vid=None, pid=None):
        """
            Check connection with the board before loading the image

            :param vid: Vendor id
            :type vid: hexadecimal string
            :param pid: Product id
            :type pid: hexadecimal string

            :returns: (dict) Status [See __handle_return]
        """

        if ((vid is not None) and (pid is not None)):
            # -u <vid>,<pid> -j -- get-property 1
            cmd = [self._blhost, '-u', vid + ',' + pid, '-j', '--', 'get-property', '1']
        else:
            # -u -j -- get-property 1
            cmd = [self._blhost, '-u', '-j', '--', 'get-property', '1']

        logging.debug("Cmdln: {}".format(' '.join(cmd)))

        out = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        return self.__handle_return(out, DEBUG_LOG)

    def load_image(self, img, vid=None, pid=None):
        """
            Load binary in internal memory. Substitute for sdphost write_file
            If no vid and pid mentioned the default one is vid = 0x15a2 pid = 0x73

            :param img: Path to the img to be loaded
            :type img: string
            :param vid: Vendor id
            :type vid: hexadecimal string
            :param pid: Product id
            :type pid: hexadecimal string

            :returns: (dict) Status [See __handle_return]
        """
        
        if ((vid is not None) and (pid is not None)):
            # -u <vid>,<pid> -j -- load-image <bin>
            cmd = [self._blhost, '-u', vid + ',' + pid, '-j', '--', 'load-image', img]
        else:
            # -u -j -- load-image <bin>
            cmd = [self._blhost, '-u', '-j', '--', 'load-image', img]

        logging.debug("Cmdln: {}".format(' '.join(cmd)))

        out = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        return self.__handle_return(out, DEBUG_LOG)

    def get_property(self, tag):
        """
            Query bootloader to get various properties and settings

            :param tag: Property type to get
            :type tag: hexadecimal string

            :returns: (dict) Status [See __handle_return]
        """

        # -u -j -- get_property <tag>
        cmd = [self._blhost, '-u', '-j', '--', 'get-property', tag]

        logging.debug("Cmdln: {}".format(' '.join(cmd)))

        out = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        return self.__handle_return(out, DEBUG_LOG)

    def fill_memory(self, address, byteCount, pattern):
        """
            Fill device memory start at specified address

            :param address: Absolute address to write data
            :type address: hexadecimal string
            :param byteCount: Byte count for fill
            :type byteCount: hexadecimal string
            :param pattern: Pattern to fill memory with
            :type pattern: hexadecimal string

            :returns: (dict) Status [See __handle_return]
        """

        # -u -j -- fill-memory 0x2000 0x04 0xc0333006
        cmd = [self._blhost, '-u', '-j', '--', 'fill-memory', address, byteCount, pattern]

        logging.debug("Cmdln: {}".format(' '.join(cmd)))

        out = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        return self.__handle_return(out, DEBUG_LOG)

    def configure_memory(self, memoryID, address):
        """
            Apply configuration to memory with specified ID

            :param memoryID: Memory ID to set configuration
            :type memoryID: hexadecimal string
            :param address: Absolute address to configure
            :type address: hexadecimal string

            :returns: (dict) Status [See __handle_return]
        """

        # -u -j -- configure-memory 0x09 0x2000
        cmd = [self._blhost, '-u', '-j', '--', 'configure-memory', memoryID, address]

        logging.debug("Cmdln: {}".format(' '.join(cmd)))

        out = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        return self.__handle_return(out, DEBUG_LOG)

    def efuse_get_available_fuses(self):
        """
            Prints out available fuses, name and address value

            :returns: None
        """

        for key, val in self._fuse_map.items():
            print(key, val)

    def efuse_program(self, address=None, fuse=None, data=None):
        """
            Program a one-time-programmable fuse on target device using fuse address or name.

            :param address: Fuse address to program
            :type address: Integer String or Hex String with prefix, i.e., '38' or '0x26'
            :param fuse: Named fuse to program
            :type fuse: String
            :param data: Hexadecimal data to program to fuse
            :type data: Hex string; no prefix, i.e., '12AB34CD'

            :returns: (dict) Status [See __handle_return]
        """

        addr = ''

        if fuse is not None:
            addr += self._fuse_map[fuse]
        elif address is not None:
            addr += address

        # -u -j -- efuse-program-once 6 04030201
        cmd = [self._blhost, '-u', '-j', '--', 'efuse-program-once', addr, data]

        out = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        return self.__handle_return(out, DEBUG_LOG)

    def efuse_read(self, address=None, fuse=None):
        """
            Read the value from a one-time-programmable fuse by address or name.

            :param address: Fuse address to read
            :type address: Integer String or Hex String with prefix, i.e., '38' or '0x26'
            :param fuse: Fuse name to read
            :type fuse: String

            :returns: (dict) Status [See __handle_return]
        """

        addr = ''

        if fuse is not None:
            addr += self._fuse_map[fuse]
        elif address is not None:
            addr += address

        # -u -j -- efuse-read-once 6
        cmd = [self._blhost, '-u', '-j', '--', 'efuse-read-once', addr]

        out = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        return self.__handle_return(out, DEBUG_LOG)

    def efuse_get_available_fuses(self):
        """
            Prints out available fuses, name and address value

            :returns: None
        """

        for key, val in self._fuse_map.items():
            print(key, val)

    def efuse_program(self, address=None, fuse=None, data=None):
        """
            Program a one-time-programmable fuse on target device using fuse address or name.

            :param address: Fuse address to program
            :type address: Integer String or Hex String with prefix, i.e., '38' or '0x26'
            :param fuse: Named fuse to program
            :type fuse: String
            :param data: Hexadecimal data to program to fuse
            :type data: Hex string; no prefix, i.e., '12AB34CD'
        
            :returns: (dict) Status [See __handle_return]
        """

        addr = ''

        if fuse is not None:
            addr += self._fuse_map[fuse]
        elif address is not None:
            addr += address

        # -u -j -- efuse-program-once 6 04030201
        cmd = [self._blhost, '-u', '-j', '--', 'efuse-program-once', addr, data]

        out = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        return self.__handle_return(out, DEBUG_LOG)

    def efuse_read(self, address=None, fuse=None):
        """
            Read the value from a one-time-programmable fuse by address or name.

            :param address: Fuse address to read
            :type address: Integer String or Hex String with prefix, i.e., '38' or '0x26'
            :param fuse: Fuse name to read
            :type fuse: String

            :returns: (dict) Status [See __handle_return]
        """

        addr = ''

        if fuse is not None:
            addr += self._fuse_map[fuse]
        elif address is not None:
            addr += address

        # -u -j -- efuse-read-once 6
        cmd = [self._blhost, '-u', '-j', '--', 'efuse-read-once', addr]

        out = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        return self.__handle_return(out, DEBUG_LOG)

    def flash_erase_all(self, memoryID='9', timeout_ms=120000):
        """
            Erase memory completely based on the memory id. 
            For flexspi nor memory default value is 9  

            :param memoryID: Identification number of the memory in the flashloader application.
            :type memoryID: hexadecimal string
            :param timeout_ms: Time in ms to wait for timeout, default 120000 ms
            :type timeout_ms: Integer

            :returns: (dict) Status [See __handle_return]
        """

        # -u -j -t <timeout_ms> -- flash-erase-all <memoryID>
        cmd = [self._blhost, '-u', '-j', '-t', str(timeout_ms), '--', 'flash-erase-all', memoryID]

        logging.debug("Cmdln: {}".format(' '.join(cmd)))

        out = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        return self.__handle_return(out, DEBUG_LOG)

    def flash_erase_region(self, start, size, memoryID=None, timeout_ms=120000):
        """
            Erase memory region from start address for specified size, with specified timeout.
            If memoryID not specified, flashloader detects the memory id automatically.

            :param start: Absolute address to begin erase
            :type start: hexadecimal string
            :param size: Size of region to erase in bytes
            :type size: hexadecimal string
            :param memoryID: Identification number of the memory in the flashloader application.
            :type memoryID: hexadecimal string
            :param timeout_ms: Time in ms to wait for timeout, default 120000 ms
            :type timeout_ms: Integer

            :returns: (dict) Status [See __handle_return]
        """

        if memoryID is None:
            # -u -j -t <timeout_ms> -- flash-erase-region <start> <size>
            cmd = [self._blhost, '-u', '-j', '-t', str(timeout_ms), '--', 'flash-erase-region', start, size]
        else:
            # -u -j -t <timeout_ms> -- flash-erase-region <start> <size> <memoryID>
            cmd = [self._blhost, '-u', '-j', '-t', str(timeout_ms), '--', 'flash-erase-region', start, size, memoryID]

        out = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        return self.__handle_return(out, DEBUG_LOG)

    def write_memory(self, address, path):
        """
            Write file to memory at specified address

            :param address: Absolute address to begin write
            :type address: hexadecimal string
            :param path: File path of file to write
            :type path: string

            :returns: (dict) Status [See __handle_return]
        """

        # For USB access, must use .exe on WSL
        path = path if not hlpr.isWSL() else re.sub('^/mnt/c', 'C:', path, count=1)

        # -u -j -- write-memory 0x60000000 "file/path.bin"
        cmd = [self._blhost, '-u', '-j', '--', 'write-memory', address, path]

        logging.debug("Cmdln: {}".format(' '.join(cmd)))

        out = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        return self.__handle_return(out, DEBUG_LOG)

    def receive_sb_file(self, path):
        """
            Download secure boot file

            :param path: File path of secure boot file
            :type path: string
            :returns: (dict) Status
        """

        # For USB access, must use .exe on WSL
        path = path if not hlpr.isWSL() else re.sub('^/mnt/c', 'C:', path, count=1)

        # -u -j -- receive-sb-file "file/path.sb"
        cmd = [self._blhost, '-u', '-j', '--', 'receive-sb-file', path]

        logging.debug("Cmdln: {}".format(' '.join(cmd)))

        out = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        return self.__handle_return(out, DEBUG_LOG)

    def read_memory(self, address, number_of_bytes):
        """
            read address from memory at specified address

            :param address: Absolute address to begin read
            :type address: hexadecimal string
            :param bytes: number of bytes to read

            :returns: (dict) Status [See __handle_return]
        """

        # -u -j -- read-memory 0x60000000 "number_of_bytes"
        cmd = [self._blhost, '-u', '-j', '--', 'read-memory', address, number_of_bytes]

        logging.debug("Cmdln: {}".format(' '.join(cmd)))

        out = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        return self.__handle_return(out, DEBUG_LOG)

    def reset(self):
        """
            Reset device

            :returns: (dict) Status
        """

        # -u -- reset
        cmd = [self._blhost, '-u', '-j', '--', 'reset']

        logging.debug("Cmdln: {}".format(' '.join(cmd)))

        out = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        return self.__handle_return(out, DEBUG_LOG)

    def execute(self, entry, param, stack):
        """
            Execute application at specified entry point

            :param entry: Absolute address of app entry point
            :type entry: hexadecimal string
            :param param: Parmerter to pass into R0
            :type param: hexadecimal string
            :param stack: Initial stack pointer address
            :type stack: hexadecimal string

            :returns: (dict) Status [See __handle_return]
        """

        # -u -- execute 0x60002325 0 0x20208000
        cmd = [self._blhost, '-u', '-j', '--', 'execute', entry, param, stack]

        logging.debug("Cmdln: {}".format(' '.join(cmd)))

        out = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        return self.__handle_return(out, DEBUG_LOG)
