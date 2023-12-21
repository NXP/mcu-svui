# OTA Signing

## Dependencies

- Bash
    - Windows 10, Windows Subsytem for Linux (WSL) [Tested on Ubuntu 18.04.1 LTS]
    - Linux [Untested]

- OpenSSL
    - Tested on v1.1.0g
- Python 3
    - Tested with 3.6.7

## Key Terms

- CA_NAME
    - This is the name given to the CA chain that will be used to sign the images
    - e.g., prod is the CA_NAME in the example below

- SIGNING_ENTITY
    - The signing entity is the name of the key used to sign an image
    - e.g., prod.app.a is the SIGNING_ENTITY name

## QUICK SETUP

### 1. Generate CA.

First parameter ("prod") should stay the same. Other parameters can be changed according to your need.

If you want to quickly test this, you can use the same password for all requests, but we recommend to use different passwords for maximum security.

```
user@host:~/Scripts/ota_signing $ python generate_signing_artifacts.py prod FR France Normandy NXP
```

### 2. User must define method of providing pass phrase.

If you want to quickly test this, you have to follow the below step (again, not ok from a security point of view).
Otherwise, you have to develop your own approach of passing the password to the scripts.

#### 2.1 Replace PKEY_PASS (~/Scripts/ota_signing/sign/sign_pass.py:34) with the unique password used at step1.

```
31    # TODO: Add a mechanism for storing your PRIVATE KEY PASSWORD inside PKEY_PASS variable.
32    # For testing, your passphrase can be stored in plaintext here (just assign it to PKEY_PASS).
33    # For production, a safer passphrase storing mechanism is recommended.
34    PKEY_PASS = ''
```

**We recommend to find a more secure solution for storing passphrases for the keys.**

### 3. Generate certificate bins appropriate for the file system of the board.

For simplicity, generate the certificates for both HyperFlash(app_crt.bin and ca_crt.bin) and QSPI(app_crt_qspi.bin and ca_crt_qspi.bin) even if you don`t use both platforms.
Use the below command to generate the certificates and move them to the ../../Image_Binaries/ folder.
```
user@host:~/Scripts/ota_signing $ python generate_image_crt_files.py -cn prod -ft ALL -d ../../Image_Binaries/
```

### 4. All set here. You can continue reading for more details.

## CA

Contains the CA config file and CA created entities

### Pre-requisits

For OTA signatature verification, a CA, a certificate and keys need to be generated. To do this follow the following guide.
https://jamielinux.com/docs/openssl-certificate-authority/index.html

### Directory Structure

```
./ca
├── ca.conf
├── certs
│   ├── CA_NAME.app.a.crt.pem
│   ├── CA_NAME.app.a.csr
│   ├── CA_NAME.app.b.crt.pem
│   ├── CA_NAME.app.b.csr
│   └── CA_NAME.root.ca.crt.pem
├── index.txt
├── index.txt.attr
├── index.txt.attr.old
├── index.txt.old
├── newcerts
│   ├── 01.pem
│   ├── 02.pem
│   └── 03.pem
├── private
│   ├── CA_NAME.app.a.key.pem
│   ├── CA_NAME.app.b.key.pem
│   └── CA_NAME.root.ca.key.pem
├── serial
└── serial.old
```

### CA Root entity

The CA root entity is 'CA_NAME.root.ca'. Where CA_NAME is defined by user.

- Certificate: /ca/certs/CA_NAME.root.ca.crt.pem
- Key: /ca/private/CA_NAME.root.ca.key.pem


### Signing/Verification Entities

Signing and Verification entities also have PEM encoded certificates and keys.

For application bank A:

- Certificate: /ca/cets/CA_NAME.app.a.crt.pem
- Key: /ca/private/CA_NAME.app.a.key.pem

For application bank B:

- Certificate: /ca/cets/CA_NAME.app.b.crt.pem
- Key: /ca/private/CA_NAME.app.b.key.pem



## Sign

The sign directory contains a python script for signing files using an entity from the ca directory.

This script also bundles the certificate used for sign with the original file. The certificate is appended to the end of the file data and padded to 2 KB.

### Directory Structure

```
./sign
├── output
├── bundle_generate.py
├── fica_maker.py
├── sign_me.py
├── sign_package.py
└── sign_pass.py
```

### Usage

Sign a file using the following:

```
user@host:~/ota_signing/sign $ python generate_signing_artifacts.py test.vf FR France Normandy NXP # To be changed according to your need
```

```
user@host:~/ota_signing/sign $ python sign_me.py application.bin test.vf.app.a # Sign application.bin with test.vf.app.a entity
```

**NOTE:** User must define method of providing pass phrase in sign_pass.py:34

```
31    # TODO: Add a mechanism for storing your PRIVATE KEY PASSWORD inside PKEY_PASS variable.
32    # For testing, your passphrase can be stored in plaintext here (just assign it to PKEY_PASS).
33    # For production, a safer passphrase storing mechanism is recommended.
34    PKEY_PASS = ''
```

### Output

Script outputs a .sha256 raw signature file and a .sha256.txt base64 encoded signature file:

```
user@host:~/ota_signing/sign $ cd ./output # Navigate into the output directory
user@host:~/ota_signing/sign/output $ ls
bundle.test.txt test.txt.sha256  test.txt.sha256.txt
user@host:~/ota_signing/sign/output $
```

**The content of .sha256.txt is the one supposed to be used in the signature field when an AWS OTA job is created.**

### Updater bundle generate script

This script creates a bundle containing the updater project firmware and multiple updater modules which can be specified as arguments.
The updater project, based on some metadata structures, validates the encapsulated modules and writes each one of them in its designated destination.

A representation of the bundle sections is diplayed below.

```
Updater bundle sections:
|--upd-code--|--upd-bundle-meta--|--upd-mod1-meta--|--upd-mod1--| ... |--upd-modN-meta--|--upd-modN--|--upd-code-size--|--upd-bundle-certificate--|
```

Updater bundle metadata structure is described below. This is placed immediately after the updater firmware.
```
typedef struct _bundle_metadata
{
    uint32_t       upd_start_addr;       /*!< Start address of the updater firmware  */
    uint32_t       upd_code_size;        /*!< Size of the updater firmware  */
    uint32_t       upd_bundle_size;      /*!< Size of the updater bundle */
    uint32_t       upd_mod_num;          /*!< Number of modules encapsulated into the bundle */
} bundle_meta_t;
```

Module metadata structure is described below.
```
typedef struct _module_metadata
{
    upd_mod_type_t upd_mod_type;         /*!< Module type */
    uint32_t       upd_mod_length;       /*!< Module length */
    uint8_t        modPkiSig[256];       /*!< Signature */
} mod_meta_t;
```

Two types of modules are supported: app image modules and certificate modules.
The first one contains, after the module metadata, the app image itsel, followed by the signing certificate.
Certificate modules, on the other hand, are supposed to be used when the user wants to update only the signing certificate associated with an app image and knows exactly the app image version deployed on a board. In this conditions, only the certificate and the new modPkiSig signature of the corresponding app image can be bundled in an updater package. No need to add the app image as well
Details about app image modules and certificate modules sections are following.

**NOTE:** When an app image module is created, the signing certificate will be appended at the end. This signing certificate will be used to update the signing certificate corresponding of the image on the device (if the image has a signing certificate saved in flash). So one can use updater app image modules to update both an image and its corresponding certificate at the same time. Or one can reuse a signing certificate in an updater app image module. If there is the a need for updating only the certificate, though, one can use updater certificate modules, as long as the version of the corresponding image is known.

Module metadata and module itself sections, signed app image case:
```
|--binary-type--|--binary-size--|--binary-signature--|--binary--|--signing-certificate--|
```

Module metadata and module itself sections, certificate module case:
```
|--cert-type--|--cert-size--|--binary-signature--|--certificate--|
```

### Usage

Below is the list of supported arguments for bundle_generate.py:

```
  -h, --help            show this help message and exit
  --updater_bank_a [UPDATER_BANK_A]
                        Path to the updater for bank a
  --updater_bank_b [UPDATER_BANK_B]
                        Path to the updater for bank b
  --bundle_name [BUNDLE_NAME]
                        Name of the bundle
  --bundle_cert [BUNDLE_CERT]
                        Name of signing entity for the bundle
  --bootstrap [BOOTSTRAP]
                        Path to the bootstrap binary / srec
  --bootstrap_cert [BOOTSTRAP_CERT]
                        Name of signing entity for bootstrap
  --bootloader [BOOTLOADER]
                        Path to the bootloader binary
  --bootloader_cert [BOOTLOADER_CERT]
                        Name of signing entity for bootloader
  --bootloader_cert_only
                        Create a bootloader certificate module. If missing, a
                        bootloader app module will be created
  --app_a [APP_A]       Path to the app a binary
  --app_a_cert [APP_A_CERT]
                        Name of signing entity for bank a
  --app_a_cert_only     Create an app_a certificate module. If missing, an
                        app_a app module will be created
  --app_b [APP_B]       Path to the app b binary
  --app_b_cert [APP_B_CERT]
                        Name of signing entity for bank b
  --app_b_cert_only     Create an app_b certificate module. If missing, an
                        app_b app module will be created
  --root_ca_cert [ROOT_CA_CERT]
                        Name of signing entity for root ca
  --secure_boot         Select secure boot. This will sign the bootstrap SREC
                        with the HAB key. If missing, a bootstrap bin will
                        simply be rsa signed
  --platform_name [PLATFORM_NAME]
                        Name of the platform.
                        Scripts/[PLATFORM_NAME]_config/board_config.py file
                        should exist for this platform.
```

**NOTE:** User must define method of providing pass phrase in sign_pass.py:34

```
31    # TODO: Add a mechanism for storing your PRIVATE KEY PASSWORD inside PKEY_PASS variable.
32    # For testing, your passphrase can be stored in plaintext here (just assign it to PKEY_PASS).
33    # For production, a safer passphrase storing mechanism is recommended.
34    PKEY_PASS = ''
```

Multiple usage examples are following.

## 1. Bundle with a new bootstrap image, open boot case

```
user@host:~/ota_signing/sign$ python bundle_generate.py --updater_bank_b sln_alexa_iot_bank_b_updater.bin --bundle_name bootstrap_update --bundle_cert prod.app.a --bootstrap sln_alexa_iot_bootstrap.bin --bootstrap_cert prod.app.b --platform_name sln_alexa_iot

Starting to create an updater bundle with 1 module ...

Writing file sln_alexa_iot_bank_b_updater.bin ...

Writing bootstrap module ...
Signing sln_alexa_iot_bootstrap.bin ...
SUCCESS: Signature created.
SUCCESS: Signature verified.
Writing signature of sln_alexa_iot_bootstrap.bin ...
```

## 2. Bundle with new bootstrap and bootloader images, open boot case

```
user@host:~/ota_signing/sign$ python bundle_generate.py --updater_bank_b sln_alexa_iot_bank_b_updater.bin --bundle_name bootstrap_bootloader_update --bundle_cert prod.app.a --bootstrap sln_alexa_iot_bootstrap.bin --bootstrap_cert prod.app.b --bootloader sln_alexa_iot_bootloader.bin --bootloader_cert prod.app.a --platform_name sln_alexa_iot

Starting to create an updater bundle with 2 modules ...

Writing file sln_alexa_iot_bank_b_updater.bin ...

Writing bootstrap module ...
Signing sln_alexa_iot_bootstrap.bin ...
SUCCESS: Signature created.
SUCCESS: Signature verified.
Writing signature of sln_alexa_iot_bootstrap.bin ...

Writing bootloader module ...
Signing sln_alexa_iot_bootloader.bin ...
SUCCESS: Signature created.
SUCCESS: Signature verified.
Writing signature of sln_alexa_iot_bootloader.bin ...
```

## 3. Bundle with a new bootstrap image and a new bootloader certificate, open boot case

```
user@host:~/ota_signing/sign$ python bundle_generate.py --updater_bank_b sln_alexa_iot_bank_b_updater.bin --bundle_name bootstrap_bootloader_cert_update --bundle_cert prod.app.a --bootstrap sln_alexa_iot_bootstrap.bin --bootstrap_cert prod.app.b --bootloader sln_alexa_iot_bootloader.bin --bootloader_cert prod.app.a --bootloader_cert_only --platform_name sln_alexa_iot

Starting to create an updater bundle with 2 modules ...

Writing file sln_alexa_iot_bank_b_updater.bin ...

Writing bootstrap module ...
Signing sln_alexa_iot_bootstrap.bin ...
SUCCESS: Signature created.
SUCCESS: Signature verified.
Writing signature of sln_alexa_iot_bootstrap.bin ...

Writing bootloader certificate module ...
Signing sln_alexa_iot_bootloader.bin ...
SUCCESS: Signature created.
SUCCESS: Signature verified.
Writing file ../ca/certs/prod.app.a.crt.pem ...
```

## 4. Bundle with new bootstrap, bootloader and app_a images, open boot case

```
user@host:~/ota_signing/sign$ python bundle_generate.py --updater_bank_b sln_alexa_iot_bank_b_updater.bin --bundle_name bootstrap_bootloader_app_a_update --bundle_cert prod.app.a --bootstrap sln_alexa_iot_bootstrap.bin --bootstrap_cert prod.app.b --bootloader sln_alexa_iot_bootloader.bin --bootloader_cert prod.app.a --app_a sln_alexa_iot_ais_ffs_demo.bin --app_a_cert prod.app.b --platform_name sln_alexa_iot

Starting to create an updater bundle with 3 modules ...

Writing file sln_alexa_iot_bank_b_updater.bin ...

Writing bootstrap module ...
Signing sln_alexa_iot_bootstrap.bin ...
SUCCESS: Signature created.
SUCCESS: Signature verified.
Writing signature of sln_alexa_iot_bootstrap.bin ...

Writing bootloader module ...
Signing sln_alexa_iot_bootloader.bin ...
SUCCESS: Signature created.
SUCCESS: Signature verified.
Writing signature of sln_alexa_iot_bootloader.bin ...

Writing App A module ...
Signing sln_alexa_iot_ais_ffs_demo.bin ...
SUCCESS: Signature created.
SUCCESS: Signature verified.
Writing signature of sln_alexa_iot_ais_ffs_demo.bin ...
```

## 5. Bundle with new bootstrap and bootloader images and a new app_a certificate, open boot case

```
user@host:~/ota_signing/sign$ python bundle_generate.py --updater_bank_b sln_alexa_iot_bank_b_updater.bin --bundle_name bootstrap_bootloader_app_a_cert_update --bundle_cert prod.app.a --bootstrap sln_alexa_iot_bootstrap.bin --bootstrap_cert prod.app.b --bootloader sln_alexa_iot_bootloader.bin --bootloader_cert prod.app.a --app_a sln_alexa_iot_ais_ffs_demo.bin --app_a_cert prod.app.b --app_a_cert_only --platform_name sln_alexa_iot

Starting to create an updater bundle with 3 modules ...

Writing file sln_alexa_iot_bank_b_updater.bin ...

Writing bootstrap module ...
Signing sln_alexa_iot_bootstrap.bin ...
SUCCESS: Signature created.
SUCCESS: Signature verified.
Writing signature of sln_alexa_iot_bootstrap.bin ...

Writing bootloader module ...
Signing sln_alexa_iot_bootloader.bin ...
SUCCESS: Signature created.
SUCCESS: Signature verified.
Writing signature of sln_alexa_iot_bootloader.bin ...

Writing App A certificate module ...
Signing sln_alexa_iot_ais_ffs_demo.bin ...
SUCCESS: Signature created.
SUCCESS: Signature verified.
Writing file ../ca/certs/prod.app.b.crt.pem ...
```

## 6. Bundle with a new bootstrap image and new bootloader, app_a and root ca certificates, open boot case

```
user@host:~/ota_signing/sign$ python bundle_generate.py --updater_bank_b sln_alexa_iot_bank_b_updater.bin --bundle_name bootstrap_bootloader_cert_app_a_cert_root_ca_update --bundle_cert prod.app.a --bootstrap sln_alexa_iot_bootstrap.bin --bootstrap_cert prod.app.b --bootloader sln_alexa_iot_bootloader.bin --bootloader_cert prod.app.a --bootloader_cert_only --app_a sln_alexa_iot_ais_ffs_demo.bin --app_a_cert prod.app.b --app_a_cert_only --root_ca_cert prod.root.ca --platform_name sln_alexa_iot

Starting to create an updater bundle with 4 modules ...

Writing file sln_alexa_iot_bank_b_updater.bin ...

Writing bootstrap module ...
Signing sln_alexa_iot_bootstrap.bin ...
SUCCESS: Signature created.
SUCCESS: Signature verified.
Writing signature of sln_alexa_iot_bootstrap.bin ...

Writing bootloader certificate module ...
Signing sln_alexa_iot_bootloader.bin ...
SUCCESS: Signature created.
SUCCESS: Signature verified.
Writing file ../ca/certs/prod.app.a.crt.pem ...

Writing App A certificate module ...
Signing sln_alexa_iot_ais_ffs_demo.bin ...
SUCCESS: Signature created.
SUCCESS: Signature verified.
Writing file ../ca/certs/prod.app.b.crt.pem ...

Writing root ca certificate module ...
Writing file ../ca/certs/prod.root.ca.crt.pem ...
```

## 7. Bundle with a new bootstrap image and new bootloader, app_a and root ca certificates, secure boot case

```
(env) user@host:~/ota_signing/sign$ python bundle_generate.py --updater_bank_b sln_alexa_iot_bank_b_updater.bin --bundle_name bootstrap_bootloader_cert_app_a_cert_root_ca_update --bundle_cert prod.app.a --bootstrap sln_alexa_iot_bootstrap.srec --bootstrap_cert prod.app.b --bootloader sln_alexa_iot_bootloader.bin --bootloader_cert prod.app.a --bootloader_cert_only --app_a sln_alexa_iot_ais_ffs_demo.bin --app_a_cert prod.app.b --app_a_cert_only --root_ca_cert prod.root.ca --secure_boot --platform_name sln_alexa_iot

Starting to create an updater bundle with 4 modules ...

Writing file sln_alexa_iot_bank_b_updater.bin ...

Signing bootstrap module for HAB authentication ...
Cryptographicaly signing ...
SUCCESS: Created signed image.
Writing signed bootstrap module ...
Signing ./sln_alexa_iot_bootstrap_signed_nopadding.bin ...
SUCCESS: Signature created.
SUCCESS: Signature verified.
Writing signature of ./sln_alexa_iot_bootstrap_signed_nopadding.bin ...

Writing bootloader certificate module ...
Signing sln_alexa_iot_bootloader.bin ...
SUCCESS: Signature created.
SUCCESS: Signature verified.
Writing file ../ca/certs/prod.app.a.crt.pem ...

Writing App A certificate module ...
Signing sln_alexa_iot_ais_ffs_demo.bin ...
SUCCESS: Signature created.
SUCCESS: Signature verified.
Writing file ../ca/certs/prod.app.b.crt.pem ...

Writing root ca certificate module ...
Writing file ../ca/certs/prod.root.ca.crt.pem ...
```

**NOTE:** Bootsrap binary needs to be in SREC format. Also, it is needed to build the bootstrap with XIP boot headers set to zero:
- XIP_BOOT_HEADER_ENABLE=0
- XIP_BOOT_HEADER_DCD_ENABLE=0

**Furthermore, one needs to activate de Ivaldi environment before calling bundle_generate.py with --secure_boot.**
**Secure boot argument only makes sense when one wants to update the bootstrap on a secure boot enabled board.**

### Usage restrictions

1. Overwriting the bank in which the updater bundle is written is not allowed (creating a bundle with bank B updater image and bank B ais demo, for ex.).
2. Updating the certificate corresponding to the bank from which the updater runs is not allowed (creating a bundle with bank B updater image and app B certificate module, for ex.).
3. If a root ca certificate update is wanted, the root ca updater certificate module must be sent together with all the other certificates (cred cert, app_a OR app_b cert. Not both of them due to restricition #2). Otherwise, the chain of trust authentication will fail on the embedded side. The other certificates may be sent as updater certificates modules or appended to an updater app image module. Please note that, since bootstrap certificate is not saved in flash, it is not mandatory to add it when updating root ca.
4. The updater image must always be a part of the bundle, so either updater_bank_a or updater_bank_b options should be used. But not both of them.
5. Any image option (updater, bootstrap, bootloader, app A, app B) must be followed by the corresponding certificate option.
6. Bundle cert option is mandatory, it is being used for signing the entire bundle package.

### Output

Script outputs a .sha256 raw signature file and a .sha256.txt base64 encoded signature file, besides the updater bundle, of course:

```
user@host:~/ota_signing/sign$ cd ./output # Navigate into the output directory
user@host:~/ota_signing/sign/output$ ls
bundle.bootstrap_update bundle.bootstrap_update.sha256  bundle.bootstrap_update.sha256.txt
user@host:~/ota_signing/sign/output$
```

**The resulting bundle can be used for any supported updated methods: MSD, AWS OTA, OTW**
**The content of .sha256.txt is the one supposed to be used in the signature field when an AWS OTA job is created.**
