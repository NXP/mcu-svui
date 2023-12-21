# SLN-IOT Utility Scripts

## File Format

There are 2 types of file systems (defined in sln_flash_mgmt) for 2 types of flash memories: HyperFlash and QSPI.

To convert a file for HyperFlash file system:
```
(env) user@host:~/sln_imx_rt_prog_and_test/Scripts/sln_iot_utils $ python file_format.py -if foo.txt -of foo.bin -ft H
Converting for HyperFlash
File size 13
File CRC 0xe9a85eb9
```

To convert a file for QSPI file system:
```
(env) user@host:~/sln_imx_rt_prog_and_test/Scripts/sln_iot_utils $ python file_format.py -if foo.txt -of foo.bin -ft Q
Converting for QSPI
File size 13
File CRC 0xe9a85eb9
```

