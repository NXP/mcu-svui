# SLN-SVUI-IOT Secure Boot Scripts

Scripts to enable secure boot and encrypted XIP on SLN-SVUI-IOT

## Directory Structure

```
./sln_svui_iot_secure_boot
|── README.md
├── manf
│   ├── README.md
│   ├── prog_sec_app.py
│   ├── enable_hab.py
│   └── lock_device.py
└── oem
    ├── README.md
    ├── secure_app.py
    └── setup_hab.py
```

## Device executable binaries

The following executable binaries are loaded into flash by the Secure Boot scripts:

- sln_svui_iot_bootstrap.srec
    - Programmed as plain text
    - Must be built with XIP boot headers set to zero
        - XIP_BOOT_HEADER_ENABLE=0
        - XIP_BOOT_HEADER_DCD_ENABLE=0
- sln_svui_iot_bootloader.bin
    - Encrypted by scripts if -s option is NOT provided
- sln_svui_iot_local_demo_vit.bin
    - Encrypted by scripts if -s option is NOT provided
    - sln_svui_iot_local_demo_dsmt.bin
    - Encrypted by scripts if -s option is NOT provided

## Device Hardware Configuration

SLN-SVUI-IOT platform supports 2 hardware configuration. Each of these configurations are described in the below files:
- Scripts/sln_platforms_config/sln_svui_iot_config/board_config.py
- Scripts/sln_platforms_config/sln_svui_rd_config/board_config.py

Depending on the hardware, specify one of the available configurations to the programming script (example: python prog_sec_app.py -cf ../../sln_platforms_config/sln_svui_rd_config/).

## Scripts options
```
secure_app.py has the following parameters:
    -s, --signed-only       Choose between signing bootsstrap only, or encrypted XIP.
                            -s specified:     signing bootstrap only (no encrypted XIP).
                            -s NOT specified: encrypted XIP.
    -cf, --config-folder    [CONFIG_FOLDER_PATH] Specify the folder that contains the board_config.py file for the current platform.
                            (default CONFIG_FOLDER_PATH = "../../sln_platforms_config/sln_svui_iot_config/")
                            (all options for CONFIG_FOLDER_PATH: "../../sln_platforms_config/sln_svui_iot_config/", "../../sln_platforms_config/sln_svui_rd_config/")

enable_hab.py has the following parameters:
    -cf, --config-folder    [CONFIG_FOLDER_PATH] Specify the folder that contains the board_config.py file for the current platform.
                            (default CONFIG_FOLDER_PATH = "../../sln_platforms_config/sln_svui_iot_config/")
                            (all options for CONFIG_FOLDER_PATH: "../../sln_platforms_config/sln_svui_iot_config/", "../../sln_platforms_config/sln_svui_rd_config/")

prog_sec_app.py has the following parameters:
    -s, --signed-only       Choose between signing bootsstrap only, or encrypted XIP.
                            -s specified:     signing bootstrap only (no encrypted XIP).
                            -s NOT specified: encrypted XIP.
    -cf, --config-folder    [CONFIG_FOLDER_PATH] Specify the folder that contains the board_config.py file for the current platform.
                            (default CONFIG_FOLDER_PATH = "../../sln_platforms_config/sln_svui_iot_config/")
                            (all options for CONFIG_FOLDER_PATH: "../../sln_platforms_config/sln_svui_iot_config/", "../../sln_platforms_config/sln_svui_rd_config/")
```


## Key Files for Host Migration

If the Secure Boot eco-system needs to be migrated to a new host, the following files *must* be migrated.

- /keys/*.* [All files]
- /crts/*.* [All files]
- /ca/*.* [All files]
- /Image_Binaries/ivt_flashloader_signed_nopadding.bin
- /Image_Binaries/ivt_flashloader_signed.bin
