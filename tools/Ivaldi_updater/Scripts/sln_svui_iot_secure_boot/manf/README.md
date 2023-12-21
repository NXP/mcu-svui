# Manufacturing Line Scripts

Scripts intended to serve as examples for production line programming

## Device Hardware Configuration

SLN-SVUI-IOT platform supports 2 hardware configuration. Each of these configurations are described in the below files:
- Scripts/sln_platforms_config/sln_svui_iot_config/board_config.py
- Scripts/sln_platforms_config/sln_svui_rd_config/board_config.py

Depending on the hardware, specify one of the available configurations to the programming script (example: python prog_sec_app.py -cf ../../sln_platforms_config/sln_svui_iot_config/).

## Scripts options
```
enable_hab.py has the following parameters:
    -cf, --config-folder    [CONFIG_FOLDER_PATH] Specify the folder that contains the board_config.py file for the current platform.
                            (default CONFIG_FOLDER_PATH = "../../sln_platforms_config/sln_svui_iot_config/")

prog_sec_app.py has the following parameters:
    -s, --signed-only       Run signed rather than encrypted app.
                            Binaries will be flashed
                            either (-s specified):     as the board is in non eXIP mode,
                            either (-s NOT specified): as the board is in eXIP mode.
    -cf, --config-folder    [CONFIG_FOLDER_PATH] Specify the folder that contains the board_config.py file for the current platform.
                            (default CONFIG_FOLDER_PATH = "../../sln_platforms_config/sln_svui_iot_config/")
```

## Enable High Assurance Boot (HAB)
Required once for each device

**NOTE:** Should only be performed once per device. Should only be performed with known PKI (crts, keys)

With module unpowered, place jumpers on backside and frontside of voice module towards the interior of module

Apply power to voice module

```
(env) user@host:~/sln_imx_rt_prog_and_test/Scripts/sln_svui_iot_secure_boot/oem $ cd ../manf
(env) user@host:~/sln_imx_rt_prog_and_test/Scripts/sln_svui_iot_secure_boot/manf $ python enable_hab.py
Establishing connection...
SUCCESS: Communication established with device.
Loading flashloader...
SUCCESS: Flashloader loaded successfully.
Jumping to flashloader entry point...
SUCCESS: Device jumped to execute flashloader.
Waiting for device to be ready for blhost...
get-property
SUCCESS: Device is ready for blhost!
Reading device unique ID...
get-property
SUCCESS: Device serial number is Rin4ZdJBFg4=
Writing memory config option block...
fill-memory
SUCCESS: Config option block loaded into RAM.
Configuring FlexSPI...
configure-memory
SUCCESS: FlexSPI configured.
Erasing flash...
flash-erase-region
SUCCESS: Flash erased.
Loading secure boot file...
SUCCESS: Loaded secure boot file.
Resetting device...
reset
SUCCESS: Device Permanently Locked with HAB!
```

### Software Dependencies
- OPENSSL

## Program Device with Encrypted Application
Required for each version of Encrypted Application

With the module unpowered, place the jumpers on the front and back of voice module towards the interior of the module

Apply power to voice module

### 1. SLN-SVUI-IOT without eXIP

```
(env) user@host:~/sln_imx_rt_prog_and_test/Scripts/sln_svui_iot_secure_boot/manf $ python prog_sec_app.py -s

NOTE: Top jumper must be installed towards interior of the module

Establishing connection...
SUCCESS: Communication established with device.
Loading flashloader...
SUCCESS: Flashloader loaded successfully.
Jumping to flashloader entry point...
SUCCESS: Device jumped to execute flashloader.
Waiting for device to be ready for blhost...
get-property
SUCCESS: Device is ready for blhost!
Reading device unique ID...
get-property
SUCCESS: Device serial number is SSn...
Writing memory config option block...
fill-memory
SUCCESS: Config option block loaded into RAM.
Configuring FlexSPI...
configure-memory
SUCCESS: FlexSPI configured.
Erasing flash...
flash-erase-region
SUCCESS: Flash erased.
Programming flash with secure app file...
receive-sb-file
SUCCESS: Programmed flash with secure app file.

Unpower module, move top jumper towards exterior of the module, and restore power
```

### 2. SLN-SVUI-RD without eXIP

```
(env) user@host:~/sln_imx_rt_prog_and_test/Scripts/sln_svui_iot_secure_boot/manf $ python prog_sec_app.py -cf ../../sln_platforms_config/sln_svui_rd_config/ -s
```

