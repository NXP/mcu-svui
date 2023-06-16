/*
 * Copyright 2021 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "FreeRTOS.h"
#include "fica_definition.h"
#include "fsl_common.h"
#include "sln_flash.h"
#include "sln_auth.h"
#include "sln_flash_fs_ops.h"

#if DISABLE_IMAGE_VERIFICATION
/* NOTE: This is not recommended for a production application! */
#warning "IMAGE VERIFICATION DISABLED! THIS IS NOT RECOMMENDED."
#endif /* DISABLE_IMAGE_VERIFICATION */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static status_t SLN_IMG_CHECK_Validate(fica_img_type_t imgType, fica_t *ficaTable);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/**
 * @brief  Validate an image using its certificate (validated by ROOT_CA).
 *
 * @param  imgType Type of the image to be validated: Bootloader, App_A or App_B.
 * @param  ficaTable Pointer to FICA table.
 *
 * @return kStatus_Success if image is validated.
 */
static status_t SLN_IMG_CHECK_Validate(fica_img_type_t imgType, fica_t *ficaTable)
{
    status_t status    = kStatus_Success;
    uint8_t *img       = NULL;
    uint8_t *caRootPem = NULL;
    uint8_t *caLeafPem = NULL;
    uint32_t imgLen    = 0;
    uint8_t *imgSig    = NULL;

    if ((imgType == FICA_IMG_TYPE_NONE) || (imgType >= FICA_NUM_IMG_TYPES))
    {
        configPRINTF(("ERROR: Invalid image type %d\r\n", imgType));
        status = kStatus_InvalidArgument;
    }

    if (ficaTable == NULL)
    {
        configPRINTF(("ERROR: FICA table pointer is NULL\r\n"));
        status = kStatus_InvalidArgument;
    }

    if (status == kStatus_Success)
    {
        uint8_t *temp = NULL;

        /* Grab the start of the image address from the FICA */
        img = (uint8_t *)(FLEXSPI_AMBA_BASE + ficaTable->records[imgType].imgAddr);

        /* Calculate the image length without certificate */
        imgLen = ficaTable->records[imgType].imgLen - MAX_CERT_LEN;

        temp = (uint8_t *)img;
        temp += imgLen;

        if (strncmp((const char *)temp, "-----BEGIN CERTIFICATE-----", strlen("-----BEGIN CERTIFICATE-----")))
        {
            /* No certificate at end, we can use entire image length (factory application load) */
            imgLen = ficaTable->records[imgType].imgLen;
        }

        imgSig = (uint8_t *)(ficaTable->records[imgType].imgPkiSig);

        /* Check if img address makes sense */
        if (((uint32_t)img < FLEXSPI_AMBA_BASE) || (((uint32_t)img > FLEXSPI_AMBA_BASE + FLASH_SIZE)))
        {
            configPRINTF(("ERROR: Invalid image address 0x%X\r\n", (uint32_t)img));
            status = kStatus_InvalidArgument;
        }
    }

    if (status == kStatus_Success)
    {
        sln_flash_fs_status_t fileStatus   = SLN_FLASH_FS_OK;
        uint32_t caRootLen                 = 0;
        uint32_t caLeafLen                 = 0;

        fileStatus = sln_flash_fs_ops_read(ROOT_CA_CERT, NULL, 0, &caRootLen);
        if (fileStatus == SLN_FLASH_FS_OK)
        {
            /* Found a file */
            caRootPem = (uint8_t *)pvPortMalloc(caRootLen);
            if (caRootPem != NULL)
            {
                memset(caRootPem, 0, caRootLen);
                fileStatus = sln_flash_fs_ops_read(ROOT_CA_CERT, caRootPem, 0, &caRootLen);
            }
            else
            {
                fileStatus = SLN_FLASH_FS_ENOMEM;
            }
        }

        if (fileStatus == SLN_FLASH_FS_OK)
        {
            /* Allocate MAX cert len just to be safe */
            caLeafPem = (uint8_t *)pvPortMalloc(MAX_CERT_LEN);
            if (caLeafPem != NULL)
            {
                memset(caLeafPem, 0, MAX_CERT_LEN);

                switch (imgType)
                {
                    case FICA_IMG_TYPE_BOOTLOADER:
                        fileStatus = sln_flash_fs_ops_read(CRED_SIGNING_CERT, NULL, 0, &caLeafLen);
                        if (fileStatus == SLN_FLASH_FS_OK)
                        {
                            fileStatus = sln_flash_fs_ops_read(CRED_SIGNING_CERT, caLeafPem, 0, &caLeafLen);
                        }
                        break;
                    case FICA_IMG_TYPE_APP_A:
                        fileStatus = sln_flash_fs_ops_read(APP_A_SIGNING_CERT, NULL, 0, &caLeafLen);
                        if (fileStatus == SLN_FLASH_FS_OK)
                        {
                            fileStatus = sln_flash_fs_ops_read(APP_A_SIGNING_CERT, caLeafPem, 0, &caLeafLen);
                        }
                        break;
                    case FICA_IMG_TYPE_APP_B:
                        fileStatus = sln_flash_fs_ops_read(APP_B_SIGNING_CERT, NULL, 0, &caLeafLen);
                        if (fileStatus == SLN_FLASH_FS_OK)
                        {
                            fileStatus = sln_flash_fs_ops_read(APP_B_SIGNING_CERT, caLeafPem, 0, &caLeafLen);
                        }
                        break;
                    default:
                        fileStatus = SLN_FLASH_FS_EINVAL;
                        break;
                }
            }
            else
            {
                fileStatus = SLN_FLASH_FS_ENOMEM;
            }
        }

        if (fileStatus != SLN_FLASH_FS_OK)
        {
            configPRINTF(("ERROR: Unable to read certificates! File Status: %d\r\n", fileStatus));
            status = kStatus_Fail;
        }
    }

    if (status == kStatus_Success)
    {
        sln_auth_status_t authStatus = SLN_AUTH_OK;

        authStatus = SLN_AUTH_Verify_Cert(caRootPem, caLeafPem);
        if (authStatus == SLN_AUTH_OK)
        {
            authStatus = SLN_AUTH_Verify_Signature(caLeafPem, img, imgLen, imgSig);
        }

        if (authStatus != SLN_AUTH_OK)
        {
            configPRINTF(("ERROR: Unable to verify image authenticity, %d\r\n", authStatus));
            status = kStatus_Fail;
        }
    }

    vPortFree(caRootPem);
    vPortFree(caLeafPem);

    return status;
}

/*******************************************************************************
 * API
 ******************************************************************************/

status_t SLN_IMG_CHECK_GetValidImgAddress(uint32_t *imageAddr)
{
    status_t status         = kStatus_Success;
    fica_img_type_t imgType = FICA_IMG_TYPE_NONE;
    fica_t *ficaTable       = NULL;

    if (imageAddr == NULL)
    {
        configPRINTF(("ERROR: image address pointer is NULL\r\n"));
        status = kStatus_Fail;
    }

    if (status == kStatus_Success)
    {
        ficaTable = pvPortMalloc(sizeof(fica_t));
        if (ficaTable != NULL)
        {
            memset(ficaTable, 0, sizeof(fica_t));
        }
        else
        {
            configPRINTF(("ERROR: Failed to allocate memory for FICA table\r\n"));
            status = kStatus_Fail;
        }
    }

    if (status == kStatus_Success)
    {
        configPRINTF(("Reading FICA table\r\n"));

        status = SLN_Read_Flash_At_Address(FICA_START_ADDR, (uint8_t *)ficaTable, sizeof(fica_t));
        if (status != kStatus_Success)
        {
            configPRINTF(("ERROR: Failed to read FICA table %d\r\n", status));
        }
    }

    /* Find first valid image from: Bootloader, Active Bank App, Backup Bank App */
    if (status == kStatus_Success)
    {
        /* !!! Validate BOOTLOADER !!! */
        imgType = ficaTable->header.currBootType;
        configPRINTF(("Validating BOOTLOADER image\r\n"));
        status = SLN_IMG_CHECK_Validate(imgType, ficaTable);
        if (status == kStatus_Success)
        {
            configPRINTF(("BOOTLOADER image OK\r\n"));
        }
        else
        {
            configPRINTF(("ERROR: BOOTLOADER image NOT valid\r\n"));
        }

        /* !!! Validate Active Bank APPLICATION (if BOOTLOADER was not valid) !!! */
        if (status != kStatus_Success)
        {
            imgType = ficaTable->header.currType;
            configPRINTF(("Validating Active Bank %d APPLICATION image\r\n", imgType));
            status = SLN_IMG_CHECK_Validate(imgType, ficaTable);
            if (status == kStatus_Success)
            {
                configPRINTF(("Active Bank %d APPLICATION image OK\r\n", imgType));
            }
            else
            {
                configPRINTF(("ERROR: Active Bank %d APPLICATION image NOT valid\r\n", imgType));
            }
        }

        /* !!! Validate Backup Bank APPLICATION (if BOOTLOADER and Active Bank APPLICATION were not valid) !!! */
        if (status != kStatus_Success)
        {
            /* If the current type is APP A, try and boot APP B. Don't save it as it shouldn't persist in a fallback
             * image. This could be an issue with OTA in the future but the fact that we are here means OTA won't work
             * as we don't have a valid Bootloader to do it for us! */
            imgType = (ficaTable->header.currType == FICA_IMG_TYPE_APP_A ? FICA_IMG_TYPE_APP_B : FICA_IMG_TYPE_APP_A);
            configPRINTF(("Validating Backup Bank %d APPLICATION image\r\n", imgType));
            status = SLN_IMG_CHECK_Validate(imgType, ficaTable);
            if (status == kStatus_Success)
            {
                configPRINTF(("Backup Bank %d APPLICATION image OK\r\n", imgType));
            }
            else
            {
                configPRINTF(("ERROR: Backup Bank %d APPLICATION image NOT valid\r\n", imgType));
            }
        }
    }
    else
    {
        status = kStatus_Fail;
    }

    if (status == kStatus_Success)
    {
        *imageAddr = ficaTable->records[imgType].imgAddr;
    }
    else
    {
        configPRINTF(("ERROR: NO VALID IMAGE FOUND\r\n"));
    }

    vPortFree(ficaTable);

    return status;
}
