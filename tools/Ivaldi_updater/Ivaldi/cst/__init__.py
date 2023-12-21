"""

Copyright 2019, 2022 NXP.

This software is owned or controlled by NXP and may only be used strictly in accordance with the
license terms that accompany it. By expressly accepting such terms or by downloading, installing,
activating and/or otherwise using the software, you are agreeing that you have read, and that you
agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
applicable license terms, then you may not retain, install, activate or otherwise use the software.

File
++++
Ivaldi/cst/__init__.py

Brief
+++++
** Ivaldi cst wrapper **

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

class Cst(object):
    """
        Cst Class


    """

    def __init__(self):
        self._cst = 'cst' + self.__get_ext('cst')

    def __get_ext(self, prog_name):
        """
        """
        ext = '.exe' if useExe(prog_name) else ''
        return ext

    def __handle_return(self, ret):
        """
        """
        responseStr = ret.stdout if isinstance(ret.stdout, str) else str(ret.stdout, 'utf-8', 'ignore')

        if (0 == ret.returncode):
            logging.debug(responseStr)
            return {'ret': ret.returncode, 'response': responseStr}
        else:
            logging.debug("ERROR: Could not complete operation, return code: %d. ", ret.returncode)
            return {'ret': ret.returncode, 'response': [], 'status': {}}


    def gen_pki(self, key_script_dir, key_dir):
        """
            Generate new PKI Keys+Certificates and 4 Super Root Keys using RSA

            :returns: (dict) Status
        """

        existing_ca = 'n'
        use_ecc = 'n'
        kl = "2048"
        duration = "10"
        num_srk = '4'
        srk_ca = 'y'

        cmd_params = {
            "--existing_ca": existing_ca,
            "--use_ecc": existing_ca,
            "--duration": duration,
            "--kl": kl,
            "--num_srk": num_srk,
            "--srk_ca": srk_ca
        }

        key_script = "\\hab4_pki_tree.bat" if isWindows() else "/hab4_pki_tree.sh"
        dir_prefix = '\\'.join(key_script_dir.split('/')) if isWindows() else key_script_dir
        cmd = dir_prefix + key_script
        for keys, vals in cmd_params.items():
            cmd += ' ' + keys + ' ' + vals

        logging.debug("Cmdln: {}".format(cmd if isinstance(cmd,str) else ' '.join(cmd)))

        out = subprocess.run(cmd, 
                             cwd=key_dir, 
                             shell=True, 
                             stdout=subprocess.PIPE, 
                             stderr=subprocess.PIPE)

        return self.__handle_return(out)


    def gen_srk(self, table_path, fuse_path, srk_list):
        """
            Generate Super Root Keys

            :param table_path: path to srk table file
            :type table_path: string
            :param fuse_path: path to fuse image (.bin) file
            :type fuse_path: string
            :param srk_list: paths to SRK .pem files
            :type srk_list: list

            :returns: (dict) Status
        """
        # Note no spaces allowed between srk paths in srk_list
        cmd = ["srktool"+ self.__get_ext('srktool'), '-f', '1', '-h', '4', '-d', 'sha256'
               , '-t', table_path
               , '-e', fuse_path
               , '-c', ','.join(srk_list)]


        logging.debug("Cmdln: {}".format(cmd if isinstance(cmd,str) else ' '.join(cmd)))

        out = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        return self.__handle_return(out)

