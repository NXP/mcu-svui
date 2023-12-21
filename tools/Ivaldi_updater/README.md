# Preconfigured Ivaldi

NXP provides a package of scripts that can be used for manufacturing programming and reprogramming
devices on the production line without the J-Link. This collection of scripts is called Ivaldi. The Ivaldi package
allows developers to program all the required firmware binaries into a flash device using a single command.

The tool is preconfigured by default and contains Python3.9, latest image binaries, audio files and an already configured environment.
Its main purpose is to flash the board with the latest NXP software by running **FLASH_SVUI_BOARD.bat**.

# Setup a new environment

First, delete the existent configuration by removing the following folders:

-env
-Ivaldi.egg-info
-Python3.9

To setup a new enviorment, follow the steps below.

# Ivaldi Getting Started

IoT & Security Solutions: i.MX RT Programming and Test Scripts

Modules contained in Ivaldi (Sons of Ivaldi):

- blhost
- cst
- elftosb
- helpers
- onboard
    - aws
- sdphost

## Supported Platforms

- Windows Power Shell and Command Prompt
    - Python3 (Tested with Python 3.10.11)
- Linux
    - Bash (NOTE: Requires login as root user)
    - Python3 (Tested with Python 3.10.6)
- Windows Subsystem for Linux
    - Bash
    - Python3 (Tested with Python 3.8.3)
- Mac
    - Bash
    - Python3 (Tested with Python 3.7.6)
    - For macOS 10.15 Catalina make sure calling terminal has Input Monitoring permissions

**KNONW ISSUE**
If working **WSL** environment, there is a **restriction on the path** on which to clone this repository, ie. you don't want to clone under 'user' folder of linux ~/ .
**Prefer clonning under /mnt/c and keep a short path**.

## Dependencies

### Hardware

RT Device must be in serial download mode.

### Software

- Python 3.10.x
    - [NOTE: Might work with other Python 3.x versions, but untested. See above.]
- Pip for Python3

### Certificates

- Image verification / OTA:
    - Follow the Scripts/ota_signing/README.md

## Setup

Installing virtualenv

```
user@host:~/sln_imx_rt_prog_and_test $ pip install virtualenv
```

Starting virtualenv (inside the sln_imx_rt_prog_and_test folder)

```
user@host:~/sln_imx_rt_prog_and_test $ virtualenv env
```

**On Linux host only** login as root user (then you can continue with the next steps). NOTE: This step does not apply for Windows Subsystem for Linux.

```
user@host:~/sln_imx_rt_prog_and_test $ sudo su
```

Activating virtualenv in **Bash**

```
user@host:~/sln_imx_rt_prog_and_test $ source env/bin/activate
```

Activating virtualenv in **PowerShell**

```
PS C:\sln_imx_rt_prog_and_test> Set-ExecutionPolicy RemoteSigned
PS C:\sln_imx_rt_prog_and_test> cd env/Scripts
PS C:\sln_imx_rt_prog_and_test\env\Scripts> ./activate.ps1
(env) PS C:\sln_imx_rt_prog_and_test> cd ../..
```

Activating virtualenv in **CMD Prompt**

```
C:\sln_imx_rt_prog_and_test> env\Scripts\activate
```

Install requirements

```
(env) user@host:~/sln_imx_rt_prog_and_test $ pip install -r requirements.txt
```

Build Ivaldi API Docs

```
(env) user@host:~/sln_imx_rt_prog_and_test $ cd Docs/Ivaldi
(env) user@host:~/sln_imx_rt_prog_and_test $ make html
(env) user@host:~/sln_imx_rt_prog_and_test $ # Output in Docs/Ivaldi/_build/html
(env) user@host:~/sln_imx_rt_prog_and_test $ cd ../..  # Get back to parent dir
```

Installing Ivaldi package in **CMD Prompt**

```
(env) user@host:~/sln_imx_rt_prog_and_test $ pip install .
```

Installing Ivaldi package in **Bash**

```
(env) user@host:~/sln_imx_rt_prog_and_test $ python setup.py install
```

## Executing

- For **Open Boot** check:
    - Scripts/[PLATFORM]_open_boot/README.md
- For **Secure Boot** check:
    - Scripts/[PLATFORM]_secure_boot/README.md

