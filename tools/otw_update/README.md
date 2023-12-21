# Firmware Update Test Client Example

FW Update Test Client Example is a python script that tests FWUPDATE-API communication to the SLN-ALEXA-IOT or SLN-VIZN-IOT kit

**NOTE:** This script is intended as an example, and not inteded to be run 'as-is' in a production environment.

## Software Requirements

Python 3.6 or greater
PySerial
libscrc

### Linux

```
user@host:~$ pip3 install pyserial
user@host:~$ pip3 install libscrc
```

### Windows

```
C:\> pip install pyserial
C:\> pip install libscrc
```

## OTW

This executes OTW image update over an UART port

### Hardware Configuration

Program the Bootloader and Main Application firmware with the Comms Handler installation.

### Usage

#### Linux

```
user@host:~$ python3 fwupdate_client.py OTW /dev/ttyUSB0 bundle.sln_svui_iot_local_demo_vit.bin sln_svui_iot_local_demo_vit.bin.sha256.txt
```

#### Windows

```
C:\> python fwupdate_client.py OTW COM12 bundle.sln_svui_iot_local_demo_vit.bin sln_svui_iot_local_demo_vit.bin.sha256.txt
```
