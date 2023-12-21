# SLN-SVUI-IOT Open Boot Scripts

Scripts for using on an open device (HAB not enabled)

## Device executable binaries

The following executable binaries are loaded into flash by the Open Boot script:

- sln_svui_iot_bootstrap.bin
    - Programmed as plain text
    - Must be built with XIP boot headers set to one
        - XIP_BOOT_HEADER_ENABLE=1
        - XIP_BOOT_HEADER_DCD_ENABLE=1
- sln_svui_iot_bootloader.bin
    - Programmed as plain text
- sln_svui_iot_local_demo_vit.bin
    - Programmed as plain text
- sln_svui_iot_local_demo_dsmt.bin
    - Programmed as plain text

## Device Hardware Configuration

SLN-SVUI-IOT platform supports 2 hardware configurations. Each of these configurations are described in the below files:
- Scripts/sln_platforms_config/sln_svui_iot_config/board_config.py
- Scripts/sln_platforms_config/sln_svui_rd_config/board_config.py

Depending on the hardware, specify one of the available configurations to the programming script (example: python prog_sec_app.py -cf ../../sln_platforms_config/sln_svui_rd_config/).

## Software Dependencies

- None

## Script options
```
open_prog_full.py has the following parameters:
    -cf, --config-folder    [CONFIG_FOLDER_PATH] Specify the folder that contains the board_config.py file for the current platform.
                            (default CONFIG_FOLDER_PATH = "../sln_platforms_config/sln_svui_iot_config/")
    -ivd, --image-verification-disable    Specify to disable Image Verification feature (no certificates for the IV feature required)
    -fbb,--flash-bank-b                   Flash DSMT demo in bank B. If this parameter is not used, only bank A VIT demo will be flashed.
```

## Program Open SLN-SVUI-IOT Application
There is a manufacturing tool for:
- Programming the certificates and key
- Programming the binaries (these file names may need to change if different)

### 1. SLN-SVUI-IOT

```
(env) user@host:~/sln_imx_rt_prog_and_test $ cd Scripts/sln_svui_iot_open_boot
(env) user@host:~/sln_imx_rt_prog_and_test/Scripts/sln_svui_iot_open_boot $ python open_prog_full.py -cf ../sln_platforms_config/sln_svui_iot_config/
```

### 2. SLN-SVUI-RD

```
(env) user@host:~/sln_imx_rt_prog_and_test $ cd Scripts/sln_svui_iot_open_boot
(env) user@host:~/sln_imx_rt_prog_and_test/Scripts/sln_svui_iot_open_boot $ python open_prog_full.py -cf ../sln_platforms_config/sln_svui_rd_config/
```
