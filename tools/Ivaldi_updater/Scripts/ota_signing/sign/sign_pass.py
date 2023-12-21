#!/usr/bin/env python3

"""

Copyright 2021-2022 NXP.

This software is owned or controlled by NXP and may only be used strictly in accordance with the
license terms that accompany it. By expressly accepting such terms or by downloading, installing,
activating and/or otherwise using the software, you are agreeing that you have read, and that you
agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
applicable license terms, then you may not retain, install, activate or otherwise use the software.

File
++++
/Scripts/ota_signing/sign/sign_pass.py

Brief
+++++
** Returns the password required for image signing.
   This file should be adjusted by the clients according to their security requirements. **

.. versionadded:: 0.0

"""

import base64
import subprocess
import os

def get_signing_passphrase(SIGN_ENTITY):
    # TODO: Add a mechanism for storing your PRIVATE KEY PASSWORD inside PKEY_PASS variable.
    # For testing, your passphrase can be stored in plaintext here (just assign it to PKEY_PASS).
    # For production, a safer passphrase storing mechanism is recommended.
    PKEY_PASS = ''


    return PKEY_PASS