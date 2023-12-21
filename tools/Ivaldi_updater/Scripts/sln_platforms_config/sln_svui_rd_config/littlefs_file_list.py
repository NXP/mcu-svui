#!/usr/bin/env python3

"""

Copyright 2023 NXP.

NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance with the
license terms that accompany it. By expressly accepting such terms or by downloading, installing,
activating and/or otherwise using the software, you are agreeing that you have read, and that you
agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
applicable license terms, then you may not retain, install, activate or otherwise use the software.

File
++++
/Scripts/sln_platforms_config/sln_svui_rd_config/littlefs_file_list.py

Brief
+++++
** Defines a dictionary that contains files to be added in the filesystem, key
dictionary [(FilePath, LittlefsFilePath, encrypted)]
**

"""
# Note. Current version of littlefs-python does not support adding attribute. Encrypt is not being used

import os
import argparse
import sys

cert_list = [
    ("../ota_signing/ca/certs/prod.root.ca.crt.pem", "ca_root.dat", False),
    ("../ota_signing/ca/certs/prod.app.a.crt.pem", "app_a_sign_cert.dat", False),
    ("../ota_signing/ca/certs/prod.app.a.crt.pem", "cred_sign_cert.dat", False),
]
fileDictionary = []

def create_file_list(config_folder, ivd=True):

    """ Import board_config.py that contains the platform specs """
    try:
        print('Importing board_config.py from ' + config_folder + ' folder')
        sys.path.append(config_folder)
        import board_config
    except ImportError:
        print('ERROR: Could not import board_config.py file from ' + config_folder + ' folder')
        sys.exit(2)

    global board_config

    fileDictionary = []

    if not ivd:
        fileDictionary += cert_list

    audio_dir_path = board_config.AUDIO_DIR
    for filename in os.listdir(audio_dir_path):

        demo_dir = os.path.join(audio_dir_path, filename)

        if os.path.isdir(demo_dir):
            for lang_dir_name in os.listdir(demo_dir):
                lang_dir = os.path.join(demo_dir, lang_dir_name)

                if os.path.isdir(lang_dir):

                    for f in os.listdir(lang_dir):
                        f_path = os.path.join(lang_dir, f)
                        lfs_path = filename + "/" + lang_dir_name + "/" + f

                        fileDictionary.append((f_path, lfs_path))

    return fileDictionary
