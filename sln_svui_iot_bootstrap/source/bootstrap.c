/*
 * Copyright 2021 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

/* FreeRTOS kernel includes */
#include "FreeRTOS.h"
#include "task.h"

#include "fica_definition.h"
#include "fsl_debug_console.h"

#if ENABLE_ENCRYPTION
#include "sln_prdb.h"
#endif /* ENABLE_ENCRYPTION */

#if !DISABLE_IMAGE_VERIFICATION
#include "sln_img_check.h"
#endif /* !DISABLE_IMAGE_VERIFICATION */

#include "sln_flash_config.h"
#include "sln_flash.h"
#include "sln_reset.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#ifndef UNSECURE_JUMP_ADDRESS
#define UNSECURE_JUMP_ADDRESS FICA_IMG_BOOTLOADER_ADDR
#endif /* UNSECURE_JUMP_ADDRESS */

#define SET_THUMB_ADDRESS(x) (x | 0x1)

/* Set Temporary Stack Top to end of first block of OC RAM */
extern void __base_SRAM_OC_NON_CACHEABLE(void);
#define TEMP_STACK_TOP (__base_SRAM_OC_NON_CACHEABLE + 0x8000)

typedef void (*app_entry_t)(void);

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void BOOTSTRAP_JumpApp(uint32_t appStartAddr);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Application Entry Function for the jump. NOTE: Should be global! */
static app_entry_t s_AppEntry = 0;

/*******************************************************************************
 * Code
 ******************************************************************************/

/**
 * @brief Jump to the application located at provided address.
 *
 * @param appStartAddr Address if the application to jump to.
 */
static void BOOTSTRAP_JumpApp(uint32_t appStartAddr)
{
    status_t status            = kStatus_Success;
    uint32_t appVectorTable    = 0;
    uint32_t appEntryPointAddr = 0;

    if (appStartAddr < FLEXSPI_AMBA_BASE)
    {
        /* appStartAddr may be just an offset so a base needs to be added. */
        appVectorTable = SLN_Flash_Get_Read_Address(appStartAddr);
    }
    else
    {
        appVectorTable = appStartAddr;
    }

    if ((appVectorTable < FLEXSPI_AMBA_BASE) || (appVectorTable >= (FLEXSPI_AMBA_BASE + FLASH_SIZE)))
    {
        configPRINTF(("ERROR: Address 0x%X is not valid!\r\n", appVectorTable));
        status = kStatus_Fail;
    }

    if (status == kStatus_Success)
    {
        appEntryPointAddr = (*((uint32_t *)(appVectorTable + 4)));
        if ((appEntryPointAddr == 0xFFFFFFFF) || (appEntryPointAddr == 0x00000000))
        {
            configPRINTF(("ERROR: Application has not a valid entry point 0x%X!\r\n", appEntryPointAddr));
            status = kStatus_Fail;
        }
    }

    if (status == kStatus_Success)
    {
        /* Point entry point address to entry point call function */
        s_AppEntry = (app_entry_t)(SET_THUMB_ADDRESS(appEntryPointAddr));

        configPRINTF(("Attempting to jump to 0x%X\r\n\r\n\r\n", (uint32_t)s_AppEntry));

        vTaskDelay(portTICK_PERIOD_MS * 50);

        DbgConsole_Flush();

        DbgConsole_Deinit();

        NVIC_DisableIRQ(LPUART6_IRQn);

        /* Set the VTOR to the application vector table address. */
        SCB->VTOR = (uint32_t)appVectorTable;

        /* Set stack pointers to the application stack pointer. */
        __set_MSP((uint32_t)TEMP_STACK_TOP);
        __set_PSP((uint32_t)TEMP_STACK_TOP);

        /* Jump to main app entry point */
        s_AppEntry();
    }
}

/*******************************************************************************
 * API
 ******************************************************************************/

void BOOTSTRAP_Task(void *arg)
{
    status_t status         = kStatus_Success;
    uint32_t appJumpAddress = UNSECURE_JUMP_ADDRESS;

#if ENABLE_ENCRYPTION
    if (status == kStatus_Success)
    {
        /* If eXIP enabled, perform contexts check. */
        status = SLN_PRDB_EncryptionCheck();
    }
#endif /* ENABLE_ENCRYPTION */

#if !DISABLE_IMAGE_VERIFICATION
    if (status == kStatus_Success)
    {
        /* Get address of a valid BOOTLOADER, APP1 or APP2 image. */
        status = SLN_IMG_CHECK_GetValidImgAddress(&appJumpAddress);
    }
#endif /* !DISABLE_IMAGE_VERIFICATION */

    if (status == kStatus_Success)
    {
        /* Jump to the next application (BOOTLOADER / APP1 / APP2) */
        BOOTSTRAP_JumpApp(appJumpAddress);
    }

    sln_reset("ERROR: Unable to jump to next application!\r\n");
}
