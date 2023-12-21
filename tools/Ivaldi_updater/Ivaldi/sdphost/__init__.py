"""

Copyright 2019, 2022 NXP.

This software is owned or controlled by NXP and may only be used strictly in accordance with the
license terms that accompany it. By expressly accepting such terms or by downloading, installing,
activating and/or otherwise using the software, you are agreeing that you have read, and that you
agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
applicable license terms, then you may not retain, install, activate or otherwise use the software.

File
++++
Ivaldi/sdphost/__init__.py

Brief
+++++
** Ivaldi sdphost wrapper **

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
DEBUG_LOG = False

class SDPHost(object):
    """
        SDPHost Class


    """

    def __init__(self, vid, pid):
        """
            Sets VID and PID of USB device and executable call extention

            :param vid: USB VID of target device
            :type vid: Hex string, i.e., '0x1234'
            :param pid: USB PID of target device
            :type pid: Hex string, i.e., '0x1234'
            
            :returns: None
        """
        self._vid = vid
        self._pid = pid
        self._sdphost = 'sdphost' + self.__get_ext('sdphost')

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
        """

        if (0 == ret.returncode):
            if ret.stdout == b'':
                logging.error("Zero return code, but empty output from sdphost - returning -1")
                return {'ret': -1, 'response': [], 'status': {}}
            else:
                respsonseStr = str(ret.stdout, 'utf-8', 'ignore')
                if dbg:
                    print(respsonseStr)
                responseJson = json.loads(respsonseStr)
                return {'ret': ret.returncode, 'response': responseJson['response'], 'status': responseJson['status']}
        else:
            if dbg:
                #print(str(ret.stdout, 'utf-8', 'ignore')) # No JSON appears on error
                print('ERROR: Could not complete operation, return code: %d. ' % (ret.returncode))
            return {'ret': ret.returncode, 'response': [], 'status': {}}

    def error_status(self):
        """
            Get error status from device

            :returns: (dict) Status
        """

        # -j -u 0x1fc9,0x135 -- error-status
        cmd = [self._sdphost, '-j', '-u', self._vid + ',' + self._pid, '--', 'error-status']

        logging.debug("Cmdln: {}".format(' '.join(cmd)))

        out = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        return self.__handle_return(out, DEBUG_LOG)

    def write_file(self, address, path):
        """
            Write binary file to target at specificed address

            :param address: Absolute address to write binary file [hex]
            :type address: hexadecimal string
            :param path: File path to binary file to write
            :type path: string
            :returns: (dict) Status
        """

        # For USB access, must use .exe on WSL
        path = path if not hlpr.isWSL() else re.sub('^/mnt/c', 'C:', path, count=1)

        # -j -u 0x1fc9,0x135 -- write-file 0x20000000 file/path.bin
        cmd = [self._sdphost, '-j', '-u', self._vid + ',' + self._pid, '--', 'write-file', address, path]

        logging.debug("Cmdln: {}".format(' '.join(cmd)))

        out = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        logging.debug("Output: {}".format(out))

        return self.__handle_return(out, DEBUG_LOG)

    def jump_to_address(self, address):
        """
            Jump to specific address on target device

            :param address: Absolute address to write binary file [hex]
            :type address: hexadecimal string
            :returns: (dict) Status
        """

        # -j -u 0x1fc9,0x135 -- jump-address 0x20000400
        cmd = [self._sdphost, '-j', '-u', self._vid + ',' + self._pid, '--', 'jump-address', address]

        logging.debug("Cmdln: {}".format(' '.join(cmd)))

        out = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        return self.__handle_return(out, DEBUG_LOG)
