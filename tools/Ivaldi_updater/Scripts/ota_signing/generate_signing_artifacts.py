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
import oschmod

if len(sys.argv) < 5:
    print("Usage:")
    print("       generate_signing_artifacts.py ca_name country code country_name state organization")
    print("")
    print("       ca_name: Name of CA for image signature chain of trust")
    print("")
    print("       country code: GB/US")
    print("")
    print("       country_name: CA Country Name")
    print("")
    print("       state: CA Country State")
    print("")
    print("       organization: CA Company Organization")
    print("")
    sys.exit(-1)
else:

    CA_NAME = sys.argv[1]
    COUNTRY_CODE = sys.argv[2]
    COUNTRY_NAME = sys.argv[3]
    STATE_NAME = sys.argv[4]
    ORG_NAME = sys.argv[5]

    # Get all the credentials for signing/verification entity
    # OpenSSL on Windows does not accept '\' in paths
    CA_ROOT_FOLDER_AUX = os.path.dirname(os.path.realpath(__file__)) + '/ca'
    CA_ROOT_FOLDER = CA_ROOT_FOLDER_AUX.replace("\\", "/")

    print("\r\n========== Setup ==========")

    # Creating main directory
    print("Creating directories...")
    if not os.path.exists('ca'):
        try:
            os.makedirs('ca')
        except:
            print("ERROR: failed to create 'ca' folder")
            sys.exit(-1)
    else:
        print("ERROR: 'ca' folder already exists")
        sys.exit(-1)

    print("SUCCESS: Successfully created 'ca' folder")

    os.chdir('ca')

    # Prepare the directories for the CA/key/cert generation
    try:
        os.makedirs('certs')
        os.makedirs('crl')
        os.makedirs('newcerts')
        os.makedirs('private')
        os.makedirs('csr')

        oschmod.set_mode("private", "700")

        open("index.txt", 'w').close()
        open("serial", 'w').close()
        open("crlnumber", 'w').close()
        open("index.txt.attr", 'w').close()
    except:
        print("ERROR: Failed to prepare the file hierarchy")
        sys.exit(-1)

    print("SUCCESS: Successfully prepared the file hierarchy")


    print("Creating Serial File...")
    with open('./serial', 'w+') as serial_file:
        serial_file.write('1000')
        serial_file.close()

    print("Modifying contents for local path...")

    # Modifying openssl.cnf
    openssl_contents = []
    with open('../openssl.cnf.tmp', 'r') as openssl:
        openssl_contents = openssl.readlines()
        openssl.close()
    do_write = True
    with open('./openssl.cnf', 'w+') as openssl:
        for line in openssl_contents:
            if 'dir               = /root/ca' in line:
                do_write = False
                openssl.write('dir               = ' + CA_ROOT_FOLDER + '\n')
            if 'private_key       = $dir/private/ca.key.pem' in line:
                do_write = False
                openssl.write('private_key       = $dir/private/'+ CA_NAME + '.root.ca.key.pem\n')

            if 'certificate       = $dir/certs/ca.cert.pem' in line:
                do_write = False
                openssl.write('certificate       = $dir/certs/' + CA_NAME + '.root.ca.crt.pem\n')
            if not do_write:
                do_write = True
            else:
                openssl.write(line)
        openssl.close()
    print("SUCCESS: openssl.cnf copied.\r\n")



    print("\r\n========== Root ==========")

    # ========== Root Key ==========
    print("1. Creating Root Key...")
    cmd6 = ['openssl', 'genrsa', '-aes256', '-out', 'private/' + CA_NAME + '.root.ca.key.pem', '4096']
    out = subprocess.run(cmd6, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if out.returncode == 0:
        print("SUCCESS: Created Root Key")
    else:
        print("ERROR: Failed to Create Root Key")
        print(str(out.args))
        print(str(out.stdout.strip(), 'utf-8', 'ignore'))
        print(str(out.stderr.strip(), 'utf-8', 'ignore'))
        sys.exit(-1)

    print("Changing Root Key permissions...\r\n")
    oschmod.set_mode("private/" + CA_NAME + ".root.ca.key.pem", "400")


    # ========== Root Certificate ==========
    print("2. Creating Root Certificate...")
    cmd8 = ['openssl', 'req', '-config', 'openssl.cnf', '-key', 'private/' + CA_NAME + '.root.ca.key.pem', '-new', '-x509', '-days', '7300', '-sha256' , '-extensions' , 'v3_ca' , '-out' , 'certs/' + CA_NAME + '.root.ca.crt.pem', '-subj', '/C=' + COUNTRY_CODE + '/ST=' + STATE_NAME + '/L=' + COUNTRY_NAME + '/O=' + ORG_NAME + '/CN=' + CA_NAME + ' CA Root' ]
    out = subprocess.run(cmd8, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if out.returncode == 0:
        print("SUCCESS: Created Root Certificate")
    else:
        print("ERROR: Failed to Create Root Certificate")
        print(str(out.args))
        print(str(out.stdout.strip(), 'utf-8', 'ignore'))
        print(str(out.stderr.strip(), 'utf-8', 'ignore'))
        sys.exit(-1)

    print("Changing Root Certificate permissions...\r\n")
    oschmod.set_mode("certs/" + CA_NAME + ".root.ca.crt.pem", "444")



    print("\r\n========== Bank A ==========")

    # ========== Bank A Key ==========
    print("1. Creating Bank A Key...")
    cmd11 = ['openssl', 'genrsa', '-aes256', '-out', 'private/' + CA_NAME + '.app.a.key.pem', '2048']
    out = subprocess.run(cmd11, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if out.returncode == 0:
        print("SUCCESS: Created Bank A Key")
    else:
        print("ERROR: Failed to create Bank A Key")
        print(str(out.args))
        print(str(out.stdout.strip(), 'utf-8', 'ignore'))
        print(str(out.stderr.strip(), 'utf-8', 'ignore'))
        sys.exit(-1)

    print("Changing Bank A Key permissions...\r\n")
    oschmod.set_mode("private/" + CA_NAME + ".app.a.key.pem", "400")


    # ========== Bank A Certificate ==========
    print("2. Creating Bank A Certificate..")
    cmd13 = ['openssl', 'req', '-config', 'openssl.cnf', '-key', 'private/' + CA_NAME + '.app.a.key.pem', '-new', '-sha256', '-out', 'csr/' + CA_NAME + '.app.a.csr.pem', '-subj', '/C=' + COUNTRY_CODE + '/ST=' + STATE_NAME + '/L=' + COUNTRY_NAME + '/O=' + ORG_NAME + '/CN=' + CA_NAME + ' Application A' ]
    out = subprocess.run(cmd13, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if out.returncode == 0:
        print("SUCCESS: Created Bank A Certificate")
    else:
        print("ERROR: Failed to create Bank A Certificate")
        print(str(out.args))
        print(str(out.stdout.strip(), 'utf-8', 'ignore'))
        print(str(out.stderr.strip(), 'utf-8', 'ignore'))
        sys.exit(-1)

    print("3. Sign the Bank A CSR..")
    cmd14 = ['openssl', 'ca', '-batch', '-config', 'openssl.cnf', '-extensions', 'server_cert', '-days', '375', '-notext', '-md', 'sha256', '-in', 'csr/' + CA_NAME + '.app.a.csr.pem', '-out', 'certs/' + CA_NAME + '.app.a.crt.pem']
    out = subprocess.run(cmd14, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if out.returncode == 0:
        print("SUCCESS: Signed the Bank A CSR")
    else:
        print("ERROR: Failed to sign the Bank A CSR")
        print(str(out.args))
        print(str(out.stdout.strip(), 'utf-8', 'ignore'))
        print(str(out.stderr.strip(), 'utf-8', 'ignore'))
        sys.exit(-1)

    print("Changing Bank A Certificate permissions...\r\n")
    oschmod.set_mode("certs/" + CA_NAME + ".app.a.crt.pem", "444")



    print("\r\n========== Bank B ==========")

    # ========== Bank B Key ==========
    print("1. Creating Bank B Key...")
    cmd11 = ['openssl', 'genrsa', '-aes256', '-out', 'private/' + CA_NAME + '.app.b.key.pem', '2048']
    out = subprocess.run(cmd11, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if out.returncode == 0:
        print("SUCCESS: Created Bank B Key")
    else:
        print("ERROR: Failed to create Bank B Key")
        print(str(out.args))
        print(str(out.stdout.strip(), 'utf-8', 'ignore'))
        print(str(out.stderr.strip(), 'utf-8', 'ignore'))
        sys.exit(-1)

    print("Changing Bank B Key permissions...\r\n")
    oschmod.set_mode("private/" + CA_NAME + ".app.b.key.pem", "400")


    # ========== Bank B Certificate ==========
    print("2. Creating Bank B Certificate..")
    cmd13 = ['openssl', 'req', '-config', 'openssl.cnf', '-key', 'private/' + CA_NAME + '.app.b.key.pem', '-new', '-sha256', '-out', 'csr/' + CA_NAME + '.app.b.csr.pem', '-subj', '/C=' + COUNTRY_CODE + '/ST=' + STATE_NAME + '/L=' + COUNTRY_NAME + '/O=' + ORG_NAME + '/CN=' + CA_NAME + ' Application B' ]
    out = subprocess.run(cmd13, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if out.returncode == 0:
        print("SUCCESS: Created Bank B Certificate")
    else:
        print("ERROR: Failed to create Bank B Certificate")
        print(str(out.args))
        print(str(out.stdout.strip(), 'utf-8', 'ignore'))
        print(str(out.stderr.strip(), 'utf-8', 'ignore'))
        sys.exit(-1)

    print("3. Sign the Bank B CSR..")
    cmd14 = ['openssl', 'ca', '-batch', '-config', 'openssl.cnf', '-extensions', 'server_cert', '-days', '375', '-notext', '-md', 'sha256', '-in', 'csr/' + CA_NAME + '.app.b.csr.pem', '-out', 'certs/' + CA_NAME + '.app.b.crt.pem']
    out = subprocess.run(cmd14, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if out.returncode == 0:
        print("SUCCESS: Signed the Bank B CSR")
    else:
        print("ERROR: Failed to sign the Bank B CSR")
        print(str(out.args))
        print(str(out.stdout.strip(), 'utf-8', 'ignore'))
        print(str(out.stderr.strip(), 'utf-8', 'ignore'))
        sys.exit(-1)

    print("Changing Bank B Certificate permissions...\r\n")
    oschmod.set_mode("certs/" + CA_NAME + ".app.b.crt.pem", "444")
