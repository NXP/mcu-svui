# Offline Programming Scripts

Example scripts demonstrating how to configure PKI infrastructure and generate a secure binary

## Device Hardware Configuration

SLN-SVUI-IOT platform supports 2 hardware configuration. Each of these configurations are described in the below files:
- Scripts/sln_platforms_config/sln_svui_iot_config/board_config.py
- Scripts/sln_platforms_config/sln_svui_rd_config/board_config.py

Depending on the hardware, specify one of the available configurations to the programming script (example: python prog_sec_app.py -cf ../../sln_platforms_config/sln_svui_iot_config/).

## Scripts options
```
secure_app.py has the following parameters:
    -s, --signed-only       Run signed rather than encrypted app.
                            Binaries will be flashed
                            either (-s specified):     as the board is in non eXIP mode,
                            either (-s NOT specified): as the board is in eXIP mode.
    -cf, --config-folder    [CONFIG_FOLDER_PATH] Specify the folder that contains the board_config.py file for the current platform.
                            (default CONFIG_FOLDER_PATH = "../../sln_platforms_config/sln_svui_iot_config/")
```

## Setup High Assurance Boot PKI Infrastructure

Typically utilized once or once for each product family.

**NOTE:** The files generated from this operation are critical. They must be backed up to prevent bricking devices.

```
(env) user@host:~/sln_imx_rt_prog_and_test $ cd Scripts/sln_svui_iot_secure_boot/oem
(env) user@host:~/sln_imx_rt_prog_and_test/Scripts/sln_svui_iot_secure_boot/oem $ python setup_hab.py
This operation will delete all previous keys. Continue? [y,n]
y
Cleaning keys and certificate directories...
SUCCESS: Cleaned keys and certificate directories...
Generating PKI tree...
SUCCESS: Created PKI tree.
Generating Super Root Keys (SRK)s...
SUCCESS: Generated SRKs.
Generating boot directive file to enable HAB...
SUCCESS: Generated boot directive file.
Generating secure boot(.sb) file to enable HAB...
SUCCESS: Created secure boot file to enable HAB.
Cryptographically signing flashloader image ...
SUCCESS: Created signed flashloader image.
```

## Configure Image for Encrypted Execute in Place (XIP)

Required for each Application change (bootstrap, bootloader, or main application)

### 1. SLN_SVUI_IOT without exip

```
(env) user@host:~/sln_imx_rt_prog_and_test/scripts/sln_svui_iot_secure_boot/oem $ python secure_app.py -s
Encrypting app image ...
SUCCESS: Created encrypted image.
Creating encrypted app file ...
SUCCESS: Created encrypted app file.
```

### 2. SLN-SVUI-RD without eXIP

```
(env) user@host:~/sln_imx_rt_prog_and_test/Scripts/sln_svui_iot_secure_boot/manf $ python prog_sec_app.py -cf ../../sln_platforms_config/sln_svui_rd_config/ -s
```
