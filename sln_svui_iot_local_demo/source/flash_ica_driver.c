/*
 * Copyright 2019-2023 NXP.
 * NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly
 * in accordance with the applicable license terms. By expressly accepting such terms or by downloading,
 * installing, activating and/or otherwise using the software, you are agreeing that you have read, and
 * that you agree to comply with and are bound by, such license terms. If you do not agree to be bound by
 * the applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "fica_definition.h"
#include "flash_ica_driver.h"

#include "FreeRTOS.h"

// Flash includes
#include "sln_flash.h"
#include "sln_flash_ops.h"
#include "sln_flash_fs_ops.h"
#include "fsl_flexspi.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/

// Uncomment to force the FICA table to initialize
//#define FORCE_FICA_TABLE_INIT   1

static fica_t s_fica = {0};

/*******************************************************************************
 * Code
 ******************************************************************************/

int32_t FICA_write_db(void)
{
    int32_t status  = SLN_FLASH_NO_ERROR;
    uint8_t *bufptr = (uint8_t *)&s_fica;

    // Erase the FICA sector
    status = SLN_Erase_Sector(FICA_START_ADDR);

    if (SLN_FLASH_NO_ERROR == status)
    {
        uint32_t runaddr   = FICA_START_ADDR;
        uint32_t endaddr   = FICA_START_ADDR + sizeof(fica_t);
        uint32_t length    = (endaddr - runaddr);
        uint32_t pageCount = length / FLASH_PAGE_SIZE;
        uint32_t pageMod   = length % FLASH_PAGE_SIZE;
        uint32_t toCopy    = 0;

        if (pageMod)
        {
            pageCount++;
        }

        do
        {
            // How much should we copy? SLN_Write_Flash_Page will fill end of page with ones.
            toCopy = (length > FLASH_PAGE_SIZE) ? FLASH_PAGE_SIZE : length;

            // Write a page worth of s_fica to NVM
            status = SLN_Write_Flash_Page(runaddr, (uint8_t *)bufptr, toCopy);

            if (SLN_FLASH_NO_ERROR != status)
            {
                status = SLN_FLASH_ERROR;
                break;
            }

            runaddr += FLASH_PAGE_SIZE; // Increment the destination NVM address
            bufptr += FLASH_PAGE_SIZE;  // Increment the source RAM address
            length -= FLASH_PAGE_SIZE;  // Decrement the total length
        } while (--pageCount);
    }

    // Read it back into the Info Structure so FICA is using what was written
    if (SLN_FLASH_NO_ERROR == status)
    {
        status = SLN_Read_Flash_At_Address(FICA_START_ADDR, (uint8_t *)&s_fica, sizeof(fica_t));
    }

    return status;
}


int32_t FICA_get_app_img_start_addr(int32_t imgtype, uint32_t *startaddr)
{
    int32_t status = SLN_FLASH_ERROR;

    if (NULL != startaddr)
    {
        switch (imgtype)
        {
            case FICA_IMG_TYPE_BOOTLOADER:
                *startaddr = FICA_IMG_BOOTLOADER_ADDR;
                break;
            case FICA_IMG_TYPE_APP_A:
                *startaddr = FICA_IMG_APP_A_ADDR;
                break;
            case FICA_IMG_TYPE_APP_B:
                *startaddr = FICA_IMG_APP_B_ADDR;
                break;
            case FICA_IMG_TYPE_FS:
                *startaddr = FICA_IMG_FILE_SYS_ADDR;
                break;
            case FICA_IMG_TYPE_NONE:
            default:
                *startaddr = FICA_IMG_INVALID_ADDR;
                break;
        }

        if (*startaddr != FICA_IMG_INVALID_ADDR)
        {
            status = SLN_FLASH_NO_ERROR;
        }
    }

    return status;
}

int32_t FICA_write_image_info(int32_t imgtype, uint32_t imgfmt, uint32_t len)
{
    int32_t status     = SLN_FLASH_NO_ERROR;
    uint32_t startaddr = 0;

    // Check if its a valid image type
    if ((FICA_IMG_TYPE_NONE >= imgtype) || (FICA_NUM_IMG_TYPES <= imgtype))
    {
        status = SLN_FLASH_ERROR;
    }

    if (SLN_FLASH_NO_ERROR == status)
    {
        // Set the start address of the image based on the image type
        status = FICA_get_app_img_start_addr(imgtype, &startaddr);
    }

    if (SLN_FLASH_NO_ERROR == status)
    {
        // Write the rest of the record values for this image types ICA (Image Config Area)
        s_fica.records[imgtype].descriptor = FICA_IMG_DESC_ID;

        // Write Image Type into FICA record for this image
        s_fica.records[imgtype].imgType = imgtype;

        // Write Image Start Address into FICA Record for this image type
        s_fica.records[imgtype].imgAddr = startaddr;

        // Write Image Size into FICA Record for this image type
        s_fica.records[imgtype].imgLen = len;

        // Write Image Type into FICA Record for this image type
        s_fica.records[imgtype].imgFmt = imgfmt;

        // Write the info to the flash
        status = FICA_write_db();

        configPRINTF(("[FICA] Flash ICA record written successfully\r\n"));
    }

    return status;
}

bool is_FICA_initialized(void)
{
    bool ret = false;

#ifdef FORCE_FICA_TABLE_INIT
    static bool firsttime = true;
    if (firsttime)
    {
        firsttime = false;
        return (false);
    }
#endif /* FORCE_FICA_TABLE_INIT */

    // Check ICA Start Identifier, value should be 0x5A5A5A5A
    if ((s_fica.header.descriptor == FICA_ICA_DESC) && (s_fica.header.version == FICA_VER))
    {
        configPRINTF(("[FICA] Flash ICA already initialized\r\n"));

        ret = true;
    }
    else
    {
        configPRINTF(("[FICA] Flash ICA needs initialization\r\n"));
    }

    return ret;
}

int32_t FICA_initialize(void)
{
    int32_t status     = SLN_FLASH_NO_ERROR;
    bool isInitialized = false;

    configPRINTF(("[FICA] VERSION %d\r\n", FICA_VER));

    status = FICA_verify_ext_flash();

    if (SLN_FLASH_NO_ERROR != status)
    {
        status = SLN_FLASH_ERROR;
    }

    configPRINTF(("[FICA] Checking Image Config Area (ICA) initialization\r\n"));

    if (SLN_FLASH_NO_ERROR == status)
    {
        // Read out FICA from NVM to RAM... at least read out what is where is supposed to be
        status = SLN_Read_Flash_At_Address(FICA_START_ADDR, (uint8_t *)&s_fica, sizeof(fica_t));
    }

    if (SLN_FLASH_NO_ERROR == status)
    {
        isInitialized = is_FICA_initialized();
    }

    // If its already initialized, return no error
    if (!isInitialized)
    {
        configPRINTF(("[FICA] Flash ICA initialization started\r\n"));

        memset((uint8_t *)&s_fica, 0x00, sizeof(fica_t));

        status = SLN_Erase_Sector(FICA_START_ADDR);

        if (kStatus_Success == status)
        {
            // Write the ICA Start ID 0x5A5A5A5A
            s_fica.header.descriptor = FICA_ICA_DESC;

            // Write the ICA Version Number
            s_fica.header.version = FICA_VER;

            // Write the Factory Application type as the current application type
            s_fica.header.currType = FICA_IMG_TYPE_APP_A;

            // Set OTA and OTW Bit so OTA/OTW update shows not pending
            s_fica.header.communication =
                FICA_COMM_AIS_OTA_BIT | FICA_COMM_FWUPDATE_BIT | FICA_COMM_FWUPDATE_METHOD_BIT;

            // Initialize all the records
            for (int32_t imgtype = FICA_IMG_TYPE_BOOTLOADER; imgtype < FICA_NUM_IMG_TYPES; imgtype++)
            {
                // Write defaults to all Image Records
                status = FICA_write_image_info(imgtype, FICA_IMG_FMT_NONE, 0);

                if (SLN_FLASH_NO_ERROR != status)
                {
                    status = SLN_FLASH_ERROR;
                    break;
                }
            }
        }
    }

    configPRINTF(("[FICA] Flash ICA initialization complete\r\n"));

    return status;
}

int32_t FICA_verify_ext_flash(void)
{
    uint8_t id   = FLASH_VENDOR_ID;
    uint8_t *pid = (uint8_t *)&id;

    configPRINTF(("[FICA] RT Flash Check...\r\n"));

    /* Read flash vendor ID */
    if (FICA_read_flash_id(pid) != SLN_FLASH_NO_ERROR)
    {
        configPRINTF(("[FICA] ERROR: Cannot find Flash device!\r\n"));

        return (SLN_FLASH_ERROR);
    }
    else
    {
        configPRINTF(("[FICA] Found Flash device!\r\n"));
    }

    return (SLN_FLASH_NO_ERROR);
}

int32_t FICA_read_flash_id(uint8_t *pid)
{
    uint8_t id;
    int32_t ret = SLN_FLASH_NO_ERROR;
    uint32_t irqState;

    irqState = SLN_ram_disable_irq();

    SLN_ram_disable_d_cache();

    // Read flash ID
    ret = sln_flash_ops_get_flash_id(BOARD_FLEXSPI, &id);

    if (SLN_FLASH_NO_ERROR != ret)
    {
        ret = SLN_FLASH_ERROR;
    }

    if (SLN_FLASH_NO_ERROR == ret)
    {
        // Check if it matches the intended device
        if (*pid != id)
        {
            ret = SLN_FLASH_ERROR;
        }
    }

    SLN_ram_enable_d_cache();

    SLN_ram_enable_irq(irqState);
    /* Flush pipeline to allow pending interrupts take place
     * before starting next loop */
    __ASM volatile("isb 0xF" ::: "memory");

    return ret;
}

int32_t FICA_GetCurAppStartType(int32_t *pimgtype)
{
    int32_t status = SLN_FLASH_NO_ERROR;

    if (NULL == pimgtype)
    {
        status = SLN_FLASH_ERROR;
    }

    if (SLN_FLASH_NO_ERROR == status)
    {
        *pimgtype = FICA_IMG_TYPE_NONE;

        status = FICA_initialize();
    }

    if (SLN_FLASH_NO_ERROR == status)
    {
        // Read the address of the current application programmed in flash
        *pimgtype = s_fica.header.currType;

        if ((*pimgtype != FICA_IMG_TYPE_APP_A) && (*pimgtype != FICA_IMG_TYPE_APP_B))
        {
            // No app found in flash, write the default address
            *pimgtype = FICA_IMG_TYPE_APP_A;

            s_fica.header.currType = *pimgtype;

            // No Application set, write it to flash
            status = FICA_write_db();
        }
    }

    return status;
}

int32_t FICA_SetCurAppStartType(int32_t imgtype)
{
    int32_t status = SLN_FLASH_NO_ERROR;

    s_fica.header.currType = imgtype;

    status = FICA_write_db();

    return status;
}
