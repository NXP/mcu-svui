#!/usr/bin/env python3

'''
Copyright 2019-2021, 2023 NXP.

NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly
in accordance with the applicable license terms. By expressly accepting such terms or by downloading,
installing, activating and/or otherwise using the software, you are agreeing that you have read, and
that you agree to comply with and are bound by, such license terms. If you do not agree to be bound by
the applicable license terms, then you may not retain, install, activate or otherwise use the software.

'''

import socket
import sys
import json
import time
import base64
import os
import serial
import libscrc

FWUPDATE_ENABLE_SIGNATURE = 0
MAX_TIMES_TO_START = 3

BLOCK_SIZE = 4096

def check_ip(ip):
    import ipaddress
    try:
        ipaddress.ip_address(ip)
        return 0
    except ValueError:
        return -1

def print_usage():
    print("Usage:")
    print("       fwupdate.py method port appFile appSignFile")
    print("")
    print("       method: Firmware update method <OTW>")
    print("       PORT: The appropriate COM or TTY port <COMport_number/(/dev/ttyname)>")
    print("       appFile: File to update")
    print("       appSignFile: File signature or None if not used", flush=True)
    
if (len(sys.argv) != 5):
    print_usage()
    sys.exit(1)     
else:
    FWUPDATE_METHOD = sys.argv[1]
    APP_IMAGE = sys.argv[3]

    if ((len(sys.argv) == 5) and (sys.argv[4] != "None")):
        APP_SIGNATURE = sys.argv[4]
        FWUPDATE_ENABLE_SIGNATURE = 1

    if (FWUPDATE_METHOD != "OTW"):
        print_usage()
        sys.exit(1)

if FWUPDATE_METHOD == "OTW":
    COM_PORT=sys.argv[2]
else:
    print("Unsupported FWUPDATE method", flush=True)
    print_usage()
    sys.exit(1)

APP_IMAGE_LEN = os.path.getsize(APP_IMAGE)

def unit_test_get_audio_state(server_socket):

    print("unit_test_get_ais_state", flush=True)

    message = b'{"messageType":0, "ais_message":{"messageType":0}}'
    number_of_bytes = len(message)

    server_socket.sendall(number_of_bytes.to_bytes(4, byteorder="little"))
    server_socket.sendall(message)

    size = int.from_bytes(server_socket.recv(4), "little")
    data = server_socket.recv(size)

    json_recv = json.loads(data)
    
    if json_recv["error"]:
        print(str(json_recv["status"]), flush=True)
    
    print(str(json_recv["error"]), flush=True)

def unit_test_get_registration_state(server_socket):

    print("unit_test_get_registration_state", flush=True)

    message = b'{"messageType":0, "ais_message":{"messageType":1}}'
    number_of_bytes = len(message)

    server_socket.sendall(number_of_bytes.to_bytes(4, byteorder="little"))
    server_socket.sendall(message)

    size = int.from_bytes(server_socket.recv(4), "little")
    data = server_socket.recv(size)

    json_recv = json.loads(data)
    
    if json_recv["error"]:
        print(str(json_recv["status"]), flush=True)
    
    print(str(json_recv["error"]), flush=True)

def unit_test_get_attention_state(server_socket):

    print("unit_test_get_attention_state", flush=True)

    message = b'{"messageType":0, "ais_message":{"messageType":2}}'
    number_of_bytes = len(message)

    server_socket.sendall(number_of_bytes.to_bytes(4, byteorder="little"))
    server_socket.sendall(message)

    size = int.from_bytes(server_socket.recv(4), "little")
    data = server_socket.recv(size)

    json_recv = json.loads(data)
    
    if json_recv["error"]:
        print(str(json_recv["status"]), flush=True)
    
    print(str(json_recv["error"]), flush=True)

def unit_test_fwupdate_clean_req(connection):
    print("unit_test_fwupdate_clean_req", flush=True)
    time.sleep(0.1)

    if FWUPDATE_METHOD == "OTW":
        connection_write = connection.write
        connection_read = connection.read

    signature = bytearray()

    if FWUPDATE_ENABLE_SIGNATURE:
        with open(APP_SIGNATURE, "rb") as sig:
            signature = sig.read()

    data = {
        "messageType":1,
        "fwupdate_message": {
            "messageType":0,
            "fwupdate_common_message": {
                "messageType":2,
            }
        }
    }

    json_string = json.dumps(data)
    message = bytes(json_string, encoding='utf-8')

    number_of_bytes = len(message)

    connection_write(number_of_bytes.to_bytes(4, byteorder="little"))
    connection_write(message)

    size = int.from_bytes(connection_read(4), "little")
    data = connection_read(size)

    json_recv = json.loads(data)

    print(str(json_recv["error"]), flush=True)
    if json_recv["error"] != 0:
        print("ERROR received: " + str(json_recv["error"]), flush=True)
        sys.exit(1)

def unit_test_fwupdate_start_req(connection):
    print("unit_test_fwupdate_start_req", flush=True)
    status = -1
    start_tries = 0

    if FWUPDATE_METHOD == "OTW":
        connection_write = connection.write
        connection_read = connection.read

    signature = bytearray()

    if FWUPDATE_ENABLE_SIGNATURE:
        with open(APP_SIGNATURE, "rb") as sig:
            signature = sig.read()

    data_out = {
        "messageType":1,
        "fwupdate_message": {
            "messageType":0,
            "fwupdate_common_message": {
                "messageType":0,
                "job_id": "Test_FwUpdate",
                "signature": signature.decode('utf-8'),
                "image_size": APP_IMAGE_LEN,
            }
        }
    }

    json_string = json.dumps(data_out)
    message = bytes(json_string, encoding='utf-8')
    number_of_bytes = len(message)

    while status != 0:
        if start_tries == MAX_TIMES_TO_START:
            print("ERROR Could not connect because of another session", flush=True)
            sys.exit(1)
        else:
            start_tries = start_tries + 1

        print("Sending Start Request", flush=True)
        time.sleep(0.1)

        connection_write(number_of_bytes.to_bytes(4, byteorder="little"))
        connection_write(message)

        size = int.from_bytes(connection_read(4), "little")
        data = connection_read(size)
        json_recv = json.loads(data)

        status = json_recv["error"]
        print(str(json_recv["error"]), flush=True)
        if json_recv["error"] != 0:
            if json_recv["error"] != 4:
                print("ERROR received: " + str(json_recv["error"]), flush=True)
                sys.exit(1)
            else:
                print("Another session already started, need to stop it.", flush=True)
                unit_test_fwupdate_clean_req(connection)

def unit_test_fwupdate_block_transfer(connection):
    print("unit_test_fwupdate_block_transfer", flush=True)

    if FWUPDATE_METHOD == "OTW":
        connection_write = connection.write
        connection_read = connection.read
    
    block = bytearray()

    with open(APP_IMAGE, "rb") as f:
        offset = 0
        block += f.read(BLOCK_SIZE)

        #my_json_string = '{"messageType":1, "ota_message":{"messageType":1, "ota_server_message":{"messageType":0, "block":"' + base64.encodebytes(block).decode("utf-8").replace('\n', '') + '", "block_size":' + str(len(base64.encodebytes(block).decode("utf-8").replace('\n', ''))) + '}}}'

        #message = bytes(my_json_string, encoding='utf-8')

        while block:
            data = {
                "messageType":1,
                "fwupdate_message": {
                    "messageType":1,
                    "fwupdate_server_message": {
                        "messageType":0,
                        #"block": base64.encodebytes(block).decode("utf-8").replace('\n', ''),
                        #"block": base64.encodebytes(block.decode("utf-8").replace('\n', '').encodebytes).decode("utf-8").replace('\n', ''),
                        "block": base64.b64encode(block).decode('utf-8'),
                        #"encoded_size": len(base64.encodebytes(block).decode("utf-8").replace('\n', '')),
                        "encoded_size": len(base64.b64encode(block)),
                        "block_size": len(block),
                        "offset": offset
                    }
                }
            }
            
            #w.write(block)

            #file_crc32 = Crc32Mpeg2.calc(bytearray(binary_array)) # Why MPEG2? \\__(`_`)_//

            #w.write(base64.b64decode(data['ota_message']['ota_server_message']['block'].encode('utf-8')))

            json_string = json.dumps(data)
            message = bytes(json_string, encoding='utf-8')

            #Zero padding to a 32-bit boundary (Board DCP requirement)
            #Avoid buffer sizes multiple of 64 because the CRC doesn't match
            while ((len(message) % 4 != 0) or (len(message) % 64 == 0)):
                message += b'\x00'
            # Append MPEG2 CRC32 to the message
            crc32 = libscrc.mpeg2(message)
            message += crc32.to_bytes(4, byteorder='big')

            #print('0x', hex(crc32))
            #print(json_string)
            number_of_bytes = len(message)

            # Add delay because the transmission should not start before the board is ready to receive
            time.sleep(0.1)
            connection_write(number_of_bytes.to_bytes(4, byteorder="little"))
            connection_write(message)
            
            size = int.from_bytes(connection_read(4), "little")
            data = connection_read(size)

            json_recv = json.loads(data)
            
            print(str(json_recv["error"]), flush=True)
            if(json_recv["error"] != 0):
                print("ERROR received: "+str(json_recv["error"]), flush=True)
                sys.exit(1)

            offset += len(block)
            block = bytearray()
            block += f.read(BLOCK_SIZE)

            progress = (offset * 100)/APP_IMAGE_LEN
            print("Firmware Update Progress (" + str(round(progress, 2)) + "%): " + str(offset) + "/" + str(APP_IMAGE_LEN), flush=True)

        f.close()
            
def unit_test_fwupdate_complete(connection):
    print("unit_test_fwupdate_complete_req", flush=True)

    if FWUPDATE_METHOD == "OTW":
        connection_write = connection.write
        connection_read = connection.read

    data = {
        "messageType":1,
        "fwupdate_message": {
            "messageType":1,
            "fwupdate_server_message": {
                "messageType":1
            }
        }
    }
    
    json_string = json.dumps(data)
    message = bytes(json_string, encoding='utf-8')
    print(json_string, flush=True)
    number_of_bytes = len(message)
    
    connection_write(number_of_bytes.to_bytes(4, byteorder="little"))
    connection_write(message)

    size = int.from_bytes(connection_read(4), "little")
    data = connection_read(size)

    json_recv = json.loads(data)

    print(str(json_recv["error"]), flush=True)
    if(json_recv["error"] != 0):
        print("ERROR received: "+str(json_recv["error"]), flush=True)
        sys.exit(1)

def unit_test_fwupdate_self_test_start(connection):
    print("unit_test_fwupdate_self_test_start", flush=True)

    if FWUPDATE_METHOD == "OTW":
        connection_write = connection.write
        connection_read = connection.read

    data = {
        "messageType":1,
        "fwupdate_message": {
            "messageType":1,
            "fwupdate_server_message": {
                "messageType":2
            }
        }
    }

    json_string = json.dumps(data)
    message = bytes(json_string, encoding='utf-8')
    print(json_string, flush=True)
    number_of_bytes = len(message)
    
    connection_write(number_of_bytes.to_bytes(4, byteorder="little"))
    connection_write(message)

    size = int.from_bytes(connection_read(4), "little")
    data = connection_read(size)

    json_recv = json.loads(data)
    
    print(str(json_recv["error"]), flush=True)
    if(json_recv["error"] != 0):
        print("ERROR received: "+str(json_recv["error"]), flush=True)
        sys.exit(1)

def unit_test_fwupdate_activate_img(connection):
    print("unit_test_fwupdate_activate_img", flush=True)
    
    if FWUPDATE_METHOD == "OTW":
        connection_write = connection.write
        connection_read = connection.read

    data = {
        "messageType":1,
        "fwupdate_message": {
            "messageType":1,
            "fwupdate_server_message": {
                "messageType":3
            }
        }
    }

    json_string = json.dumps(data)
    message = bytes(json_string, encoding='utf-8')
    print(json_string, flush=True)
    number_of_bytes = len(message)
    
    connection_write(number_of_bytes.to_bytes(4, byteorder="little"))
    connection_write(message)

    size = int.from_bytes(connection_read(4), "little")
    data = connection_read(size)

    json_recv = json.loads(data)
    
    print(str(json_recv["error"]), flush=True)
    if(json_recv["error"] != 0):
        print("ERROR received: "+str(json_recv["error"]), flush=True)
        sys.exit(1)

def unit_test_launch_otw():
    print("Start OTW ...", flush=True)
    connected = False
    while not connected:
        connected = True
        print("Attempt to connect ...", flush=True)
        try:
            serial_conn = serial.Serial(COM_PORT, 115200, parity=serial.PARITY_NONE)

        except Exception as e:
            print(e, flush=True)
            time.sleep(1)
            connected = False

    print("Connected to " + COM_PORT, flush=True)

    try:
        # Trigger the OTW from the main app
        serial_conn.close()
        serial_conn.open()
        serial_conn.reset_output_buffer()
        serial_conn.reset_input_buffer()

        # Start OTW
        unit_test_fwupdate_start_req(serial_conn)

        # Transfer the firmware
        time.sleep(1)
        unit_test_fwupdate_block_transfer(serial_conn)

        # Stop the fw transfer
        time.sleep(1)
        unit_test_fwupdate_complete(serial_conn)

        # Activate the firmware
        time.sleep(1)
        unit_test_fwupdate_activate_img(serial_conn)

        # Validate and run the firmware
        time.sleep(4)
        serial_conn.reset_input_buffer()
        serial_conn.reset_output_buffer()
        unit_test_fwupdate_self_test_start(serial_conn)

        print("Firmware Update succeeded", flush=True)
    finally:
        print("\r\n")
        serial_conn.close

if FWUPDATE_METHOD == "OTW":
    unit_test_launch_otw()

