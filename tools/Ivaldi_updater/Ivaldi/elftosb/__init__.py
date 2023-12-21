"""

Copyright 2019, 2022 NXP.

This software is owned or controlled by NXP and may only be used strictly in accordance with the
license terms that accompany it. By expressly accepting such terms or by downloading, installing,
activating and/or otherwise using the software, you are agreeing that you have read, and that you
agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
applicable license terms, then you may not retain, install, activate or otherwise use the software.

File
++++
Ivaldi/elftosb/__init__.py

Brief
+++++
** Ivaldi elftosb wrapper **

.. versionadded:: 0.0

API
+++

"""

import os
import platform
import subprocess
import json
import sys
import logging

from Ivaldi.helpers import useExe, isWindows

#logging.basicConfig(level=logging.DEBUG)

class Elftosb(object):
    """
        Elftosb Class


    """

    def __init__(self):
        self._elftosb = 'elftosb' + self.__get_ext('elftosb')

    def __get_ext(self, prog_name):
        """
        """
        ext = '.exe' if useExe(prog_name) else ''
        return ext

    def __handle_return(self, ret):
        """
        """
        responseStr = str(ret.stdout, 'utf-8', 'ignore')

        if (0 == ret.returncode):
            logging.debug(responseStr)
            return {'ret': ret.returncode, 'response': responseStr}
        else:
            logging.debug("ERROR: Could not complete operation, return code: %d. ", ret.returncode)
            return {'ret': ret.returncode, 'response': [], 'status': {}}


    def help(self):
        """
            Test invocation of elftosb

            :returns: (dict) Status
        """
        cmd = [self._elftosb, '--help']

        logging.debug("Cmdln: {}".format(' '.join(cmd)))

        out = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        return self.__handle_return(out)


    def srk_sb(self, bd_path, sb_path):
        """
            Create sb file for burning SRK hash into target fuses

            :param bd_path: path to boot directive file
            :type bd_path: string
            :param sb_path: path to secure boot file
            :type sb_path: string

            :returns: (dict) Status
        """

        cmd = [self._elftosb, '-f', 'kinetis', '-V', '-c', bd_path, '-o', sb_path]

        logging.debug("Cmdln: {}".format(' '.join(cmd)))

        out = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        return self.__handle_return(out)


    def sign_srec(self, bd_path, bin_path, srec_dir, srec_file, cst_dir="./Tools/cst"):
        """
            Create signed image (.bin) file from srec file

            :param bd_path: path to boot directive file
            :type bd_path: string
            :param bin_path: path to signed image (.bin) file
            :type bin_path: string
            :param srec_dir: path to directory containing srec file
            :type srec_dir: string

            :returns: (dict) Status
        """

        cmd = [self._elftosb, '-f', 'imx', '-V'
               , '-O', 'cstFolderPath={}'.format(cst_dir)
               , '-c', bd_path
               , '-o', bin_path
               , '-p', srec_dir
               , srec_file]

        logging.debug("Cmdln: {}".format(' '.join(cmd)))

        out = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        return self.__handle_return(out)


    def create_sb(self, bd_path, sb_path, bin_dir, bin_files, entry_pt=None, cst_dir="./Tools/cst"):
        """
            Create secure boot (.sb) file from signed image (.bin) file

            :param bd_path: path to boot directive file
            :type bd_path: string
            :param sb_path: path to secure boot file
            :type sb_path: string
            :param bin_file: list of paths to image files
            :type bin_files: list of strings
            :param bin_dir: path to directory containing image file
            :type bin_dir: string
            :param entry_pt: entry point address from app srec file
            :type entry_pt: string

            :returns: (dict) Status
        """

        cmd = [self._elftosb, '-f', 'kinetis', '-V'
               , '-O', 'cstFolderPath={}'.format(cst_dir)
               , '-c', bd_path
               , '-o', sb_path
               , '-p', bin_dir
               ]
        cmd.extend(bin_files)

        if entry_pt is not None:
            cmd.extend(['-O', 'entryPointAddress={}'.format(entry_pt)])

        logging.debug("Cmdln: {}".format(' '.join(cmd)))

        out = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        return self.__handle_return(out)

