# Demo Scripts

Demonstration scripts for exercising the basic functionality of Ivaldi

## Running Scripts

### AWS Onboard Thing

```
(env) user@host:~/sln_imx_rt_prog_and_test/Scripts/demos $ python ./aws_onboard.py
```

### Read Serial Number

```
(env) user@host:~/sln_imx_rt_prog_and_test/Scripts/demos $ python ./read_serial.py
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
SUCCESS: Device serial number is uulvuVr6Z4I=
```

### Program 'hello_world'

```
(env) user@host:~/sln_imx_rt_prog_and_test/Scripts/demos $ python ./program_hello_world.py
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
SUCCESS: Device serial number is uulvuVr6Z4I=
Writing memory config option block...
fill-memory
SUCCESS: Config option block loaded into RAM.
Configuring FlexSPI...
configure-memory
SUCCESS: FlexSPI configured.
Erasing flash...
flash-erase-region
SUCCESS: Flash erased.
Programming flash...
write-memory
SUCCESS: File written to flash.
read-memory
SUCCESS: Application entry point at 0x60002309
read-memory
SUCCESS: Application entry point at 0x20020000
Attempting to execute application...
execute
SUCCESS: Application running.
```

After programming open a terminal, connect to virtual com port and press enter.

The following should appear in the console:

```
hello world.
```

### Erase Flash

```
(env) user@host:~/sln_imx_rt_prog_and_test/Scripts/demos $ python ./erase.py
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
SUCCESS: Device serial number is uulvuVr6Z4I=
Writing memory config option block...
fill-memory
SUCCESS: Config option block loaded into RAM.
Configuring FlexSPI...
configure-memory
SUCCESS: FlexSPI configured.
Erasing flash...
flash-erase-region
SUCCESS: Flash erased.
```

### Write Fuse

```
(env) user@host:~/sln_imx_rt_prog_and_test/Scripts/demos $ python write_fuse.py
Establishing connection...
SUCCESS: Communication established with device.
Loading flashloader...
SUCCESS: Flashloader loaded successfully.
Jumping to flashloader entry point...
SUCCESS: Device jumped to execute flashloader.
Waiting for device to be ready for blhost...
get-property
SUCCESS: Device is ready for blhost!
Reading fuse GP3_0...
efuse-read-once
SUCCESS: Device fuse GP3_0 reads 4 bytes as 0x80000001
Writing to fuse ...
efuse-program-once
... fuse write successful.
Reading back fuse GP3_0...
efuse-read-once
SUCCESS: Device fuse GP3_0 reads 4 bytes as 0x80000001
```

### Read Fuse

```
(env) user@host:~/sln_imx_rt_prog_and_test/Scripts/demos $ python read_fuse.py
Establishing connection...
SUCCESS: Communication established with device.
Loading flashloader...
SUCCESS: Flashloader loaded successfully.
Jumping to flashloader entry point...
SUCCESS: Device jumped to execute flashloader.
Waiting for device to be ready for blhost...
get-property
SUCCESS: Device is ready for blhost!
Reading fuse CFG3...
efuse-read-once
SUCCESS: Device fuse CFG3 reads 4 bytes as 0x420002
```
