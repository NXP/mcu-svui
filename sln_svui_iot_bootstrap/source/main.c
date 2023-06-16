/*
 * Copyright 2019-2022 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

/* Board includes */
#include "pin_mux.h"
#include "board.h"
#include "clock_config.h"
#include "fsl_common.h"
#include "fsl_debug_console.h"

/* FreeRTOS kernel includes */
#include "FreeRTOS.h"
#include "task.h"

#include "bootstrap.h"
#include "sln_reset.h"

/* Security includes */
#if (!DISABLE_IMAGE_VERIFICATION || ENABLE_ENCRYPTION)
#include "ksdk_mbedtls.h"
#endif /* (!DISABLE_IMAGE_VERIFICATION || ENABLE_ENCRYPTION) */

/* Flash includes */
#include "sln_flash.h"
#include "sln_flash_fs_ops.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define SECURITY_VIOLATION_MASK 0x0000003F
#define JTAG_VIOLATION          0x00000002
#define WDOG_VIOLATION          0x00000004
#define GPIO_VIOLATION          0x00000020
#define UNKNOWN_VIOLATION       0x00000019

typedef enum _security_violation
{
    SEC_NO_VIOLATION = 0,
    SEC_JTAG_VIOLATION,
    SEC_WDOG_VIOLATION,
    SEC_GPIO_VIOLATION,
    SEC_UNKNOWN_VIOLATION,
} security_violation_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void BOOTSTRAP_PrintSecurityViolation(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

static const char *s_ViolationStr[] = {"No security violation.", "ERROR: JTAG active violation!",
                                       "ERROR: WDOG2 reset violation!", "ERROR: GPIO_EMC_19 violation!",
                                       "ERROR: UNKNOWN violation!"};

/*******************************************************************************
 * Code
 ******************************************************************************/

static void BOOTSTRAP_PrintSecurityViolation(void)
{
    uint32_t securityStatus = 0;

    securityStatus = (SECURITY_VIOLATION_MASK & SNVS->HPSVSR);

    if (JTAG_VIOLATION & securityStatus)
    {
        configPRINTF(("%s\r\n", s_ViolationStr[SEC_JTAG_VIOLATION]));
    }

    if (WDOG_VIOLATION & securityStatus)
    {
        configPRINTF(("%s\r\n", s_ViolationStr[SEC_WDOG_VIOLATION]));
    }

    if (GPIO_VIOLATION & securityStatus)
    {
        configPRINTF(("%s\r\n", s_ViolationStr[SEC_GPIO_VIOLATION]));
    }

    if (UNKNOWN_VIOLATION & securityStatus)
    {
        configPRINTF(("%s\r\n", s_ViolationStr[SEC_UNKNOWN_VIOLATION]));
    }
}

__attribute__((naked)) void HardFault_Handler(void)
{
    __asm("BKPT #0");
}

/*******************************************************************************
 * API
 ******************************************************************************/

int main(void)
{
    /* Relocate Vector Table */
#if RELOCATE_VECTOR_TABLE
    BOARD_RelocateVectorTableToRam();
#endif

    BOARD_ConfigMPU();
    BOARD_InitBootPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

#if (!DISABLE_IMAGE_VERIFICATION || ENABLE_ENCRYPTION)
    /* Cryptographic hardware modules initialization */
    CRYPTO_InitHardware();
#endif /* (!DISABLE_IMAGE_VERIFICATION || ENABLE_ENCRYPTION) */

    /* Initialize Flash to allow writing */
    SLN_Flash_Init();

    if (SLN_FLASH_FS_OK != sln_flash_fs_ops_init(SLN_FLASH_PLAIN))
    {
        configPRINTF(("ERROR: littlefs init failed!\r\n"));
    }

    PRINTF("\r\n\r\n*** BOOTSTRAP v%d.%d.%d ***\r\n\r\n", APP_MAJ_VER, APP_MIN_VER, APP_BLD_VER);

    BOOTSTRAP_PrintSecurityViolation();

    /* Setup bootstrap task */
    if (xTaskCreate(BOOTSTRAP_Task, BOOTSTRAP_TASK_NAME, BOOTSTRAP_TASK_STACK_SIZE, NULL, BOOTSTRAP_TASK_PRIORITY,
                    NULL) != pdPASS)
    {
        configPRINTF(("ERROR: Unable to create BOOTSTRAP_Task.\r\n"));
    }

    /* Run RTOS */
    vTaskStartScheduler();

    /* We should never get here */
    sln_reset("ERROR: Unable to boot!\r\n");

    return 0;
}
