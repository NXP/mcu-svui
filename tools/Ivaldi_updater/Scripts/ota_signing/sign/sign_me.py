#!/usr/bin/env python3

'''
Copyright 2019, 2022 NXP.

This software is owned or controlled by NXP and may only be used strictly in accordance with the
license terms that accompany it. By expressly accepting such terms or by downloading, installing,
activating and/or otherwise using the software, you are agreeing that you have read, and that you
agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
applicable license terms, then you may not retain, install, activate or otherwise use the software.

'''

import base64
import subprocess
import sys
import os
import math

from sign_pass import get_signing_passphrase

if len(sys.argv) < 3:
    print("Usage:")
    print("       sign_me.py input cert_name")
    print("")
    print("       input: File to sign")
    print("")
    print("       cert_name: Name of signing entity")
    print("")
    sys.exit(1)
else:

    print("Generating signature for %s" % sys.argv[1])

    IN_FILE = sys.argv[1]
    print("File size: %d" % os.path.getsize(IN_FILE))
    OUT_DIR_NAME = './output'
    BASE_NAME = os.path.basename(IN_FILE)
    SIG_RAW_FILE = OUT_DIR_NAME + '/' + BASE_NAME + '.sha256'
    SIG_ENC_FILE = SIG_RAW_FILE + '.txt'
    IMG_BUNDLE = OUT_DIR_NAME + '/bundle.' + BASE_NAME

    # Get all the credentials for signing/verification entity
    SIGN_ENTITY = sys.argv[2]
    SIGN_ENTITY_KEY_DIR = '../ca/private/'
    SIGN_ENTITY_CRT_DIR = '../ca/certs/'
    SIGN_KEY = SIGN_ENTITY_KEY_DIR + SIGN_ENTITY + '.key.pem'
    SIGN_CRT = SIGN_ENTITY_CRT_DIR + SIGN_ENTITY + '.crt.pem'

    PKEY_PASS = get_signing_passphrase(SIGN_ENTITY)

    # Sign the file and output the signature raw and base64
    print("Signing...")

    cmd3 = ['openssl', 'dgst', '-sha256', '-sign', SIGN_KEY, '-passin', 'pass:' + PKEY_PASS, '-out', SIG_RAW_FILE, IN_FILE]
    out = subprocess.run(cmd3, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if out.returncode == 0:
        print("SUCCESS: Signature created.")
    else:
        print("ERROR: Could not sign.")
        print(str(out.stdout.strip(), 'utf-8', 'ignore'))
        print(str(out.stderr.strip(), 'utf-8', 'ignore'))
        sys.exit(1)

    # Verify the signint process
    print("Verifying...")

    # Extract public key from certificate
    cmd4 = ['openssl', 'x509', '-in', SIGN_CRT, '-pubkey', '-noout']
    out = subprocess.run(cmd4, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if out.returncode == 0:
        with open('./foo.pem', 'w+') as pk:
            pk.write(str(out.stdout, 'utf-8', 'ignore'))
            pk.close()
        print("SUCCESS: Public Key extracted.")
    else:
        print("ERROR: Could not extract public key.")
        print(str(out.stdout.strip(), 'utf-8', 'ignore'))
        print(str(out.stderr.strip(), 'utf-8', 'ignore'))
        sys.exit(1)

    # Verify signature
    cmd5 = ['openssl', 'dgst', '-sha256', '-verify', './foo.pem', '-signature', SIG_RAW_FILE, IN_FILE]
    out = subprocess.run(cmd5, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if out.returncode == 0:
        print("SUCCESS: Signature verified.")
        os.remove('./foo.pem')
    else:
        print("ERROR: Could not verify.")
        print(str(out.stdout.strip(), 'utf-8', 'ignore'))
        print(str(out.stderr.strip(), 'utf-8', 'ignore'))
        os.remove('./foo.pem')
        sys.exit(1)

    print("Writing signature...")
    #(echo -n -e "$SIG_RAW_FILE" | base64) > $SIG_ENC_FILE
    raw_sig = bytearray()
    with open(SIG_RAW_FILE, 'rb') as rSig:
        raw_sig += rSig.read()
        rSig.close()
    with open(SIG_ENC_FILE, 'wb') as bSig:
        enc_sig = base64.b64encode(raw_sig)
        bSig.write(enc_sig)
        bSig.close()


    # Open up the bundle file
    bunFile = open(IMG_BUNDLE, 'w+b')
    bunFile.truncate(0)

    # Copy original contents to file
    IN_TXT = bytearray()
    with open(IN_FILE, 'rb') as inF:
        IN_TXT = inF.read()
        inF.close()

    bunFile.write(IN_TXT)

    # Append certificate to file
    CERT_TXT = bytearray()
    with open(SIGN_CRT, 'rb') as inF:
        CERT_TXT = inF.read()
        inF.close()

    bunFile.write(CERT_TXT)

    # Calculate remaining padding to add
    PADS = 2048 - os.path.getsize(SIGN_CRT)
    print(PADS)

    # Pad a zero
    bunFile.write(int('00', 16).to_bytes(1, byteorder='little'))
    PADS = PADS - 1

    # Pad ones
    while PADS:
        bunFile.write(int('FF', 16).to_bytes(1, byteorder='little'))
        PADS = PADS - 1

    bunFile.close()
    sys.exit(0)
