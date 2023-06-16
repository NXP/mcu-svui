/*
 * Copyright 2021 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "FreeRTOS.h"
#include "fsl_common.h"
#include "fusemap.h"
#include "sln_auth.h"
#include "sln_reset.h"

#if ENABLE_CONTEXT_DUMP
#include "task.h"
#include "fsl_debug_console.h"
#include "sln_flash.h"
#endif /* ENABLE_CONTEXT_DUMP */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define NO_CONTEXT(x)  (1U << x)
#define NO_CONTEXT_0   (1U << 0)
#define NO_CONTEXT_1   (1U << 1)
#define NO_CONTEXT_ALL (NO_CONTEXT_0 | NO_CONTEXT_1)

#if ENABLE_CONTEXT_DUMP
#define CTX_BUFF_SIZE 0x150
#endif /* ENABLE_CONTEXT_DUMP */

typedef enum _ctx_status
{
    kContext_Good,       /* All contexts are valid */
    kContext_Fail,       /* Failed to validate a context */
    kContext_Missing0,   /* SLN_CRYPTO_CTX_0 is missing */
    kContext_Missing1,   /* SLN_CRYPTO_CTX_1 is missing */
    kContext_MissingAll, /* All contexts are missing */
} ctx_status_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static ctx_status_t SLN_PRDB_GetStatus(void);
static status_t SLN_PRDB_Restore(ctx_status_t ctxStatus);
#if ENABLE_CONTEXT_DUMP
static void SLN_PRDB_Display(void);
#endif /* ENABLE_CONTEXT_DUMP */

/*******************************************************************************
 * Variables
 ******************************************************************************/

#if ENABLE_CONTEXT_DUMP
#warning "ARE YOU SURE YOU WANT TO PRINT THIS INFORMATION OUT?"
static uint8_t s_TempCtxBuf[CTX_BUFF_SIZE] = {0};
#endif /* ENABLE_CONTEXT_DUMP */

/*******************************************************************************
 * Code
 ******************************************************************************/

/**
 * @brief  Validate encryption contexts.
 *
 * @return kContext_Good if all contexts are valid, otherwise check ctx_status_t.
 */
static ctx_status_t SLN_PRDB_GetStatus(void)
{
    sln_auth_status_t status = SLN_AUTH_OK;
    ctx_status_t ctxStatus   = kContext_Good;
    uint8_t failMask         = 0;

    for (uint8_t ctx = 0; ctx < SLN_CRYPTO_CTX_COUNT; ctx++)
    {
        status = SLN_AUTH_check_context(ctx);

        if (status == SLN_AUTH_NO_CONTEXT)
        {
            configPRINTF(("WARNING: Context %d is empty.\r\n", ctx));
            failMask |= NO_CONTEXT(ctx);
        }
        else if (status != SLN_AUTH_OK)
        {
            configPRINTF(("ERROR: Check context failed %d.\r\n", status));
            failMask  = 0;
            ctxStatus = kContext_Fail;
            break;
        }
    }

    if (ctxStatus == kContext_Good)
    {
        switch (failMask)
        {
            case NO_CONTEXT_0:
                ctxStatus = kContext_Missing0;
                break;

            case NO_CONTEXT_1:
                ctxStatus = kContext_Missing1;
                break;

            case NO_CONTEXT_ALL:
                ctxStatus = kContext_MissingAll;
                break;

            default:
                break;
        }
    }

    return ctxStatus;
}

/**
 * @brief  Restore missing contexts.
 *         If any context restored successfully, reset device.
 *
 * @param  ctxStatus Missing context id. Accepted values: kContext_Missing0, kContext_Missing1 or kContext_MissingAll.
 *
 * @return kStatus_Success if context restored, kStatus_Fail otherwise.
 */
static status_t SLN_PRDB_Restore(ctx_status_t ctxStatus)
{
    status_t status              = kStatus_Success;
    sln_auth_status_t authStatus = SLN_AUTH_OK;

    switch (ctxStatus)
    {
        case kContext_Missing0:
            configPRINTF(("Restoring context 0...\r\n"));
            authStatus = SLN_AUTH_swap_context(SLN_CRYPTO_CTX_0);
            break;

        case kContext_Missing1:
            configPRINTF(("Restoring context 1...\r\n"));
            authStatus = SLN_AUTH_swap_context(SLN_CRYPTO_CTX_1);
            break;

        case kContext_MissingAll:
            configPRINTF(("Restoring all contexts...\r\n"));
            for (uint8_t ctx = 0; ctx < SLN_CRYPTO_CTX_COUNT; ctx++)
            {
                configPRINTF(("Restoring context %d...\r\n", ctx));
                authStatus = SLN_AUTH_swap_context(ctx);
                if (authStatus != SLN_AUTH_OK)
                {
                    break;
                }
            }
            break;

        default:
            configPRINTF(("ERROR: Invalid restore context argument provided, %d\r\n", ctxStatus));
            authStatus = SLN_AUTH_INVALID_ARG;
            break;
    }

    switch (authStatus)
    {
        case SLN_AUTH_OK:
            configPRINTF(("Context restored successfully.\r\n"));
            sln_reset("Rebooting to use new restored context\r\n");
            break;

        case SLN_AUTH_NO_CONTEXT:
            /* No context, we can assume first boot and set status OK */
            configPRINTF(("WARNING: Restore context missing. Could be first time...\r\n"));
            break;

        default:
            configPRINTF(("ERROR: Context restore failed with status %d\r\n", authStatus));
            status = kStatus_Fail;
            break;
    }

    return status;
}

#if ENABLE_CONTEXT_DUMP
static void SLN_PRDB_Display(void)
{
    SLN_Read_Flash_At_Address(0x400, s_TempCtxBuf, CTX_BUFF_SIZE);

    for (uint32_t idx = 0; idx < CTX_BUFF_SIZE; idx++)
    {
        if ((idx % 16) == 0)
        {
            PRINTF("\r\n");
        }
        PRINTF("%02X", s_TempCtxBuf[idx]);
        vTaskDelay(10);
    }

    PRINTF("\r\n");

    SLN_Read_Flash_At_Address(0x800, s_TempCtxBuf, CTX_BUFF_SIZE);

    for (uint32_t idx = 0; idx < CTX_BUFF_SIZE; idx++)
    {
        if ((idx % 16) == 0)
        {
            PRINTF("\r\n");
        }
        PRINTF("%02X", s_TempCtxBuf[idx]);
        vTaskDelay(10);
    }

    PRINTF("\r\n");
    vTaskDelay(100);
}
#endif /* ENABLE_CONTEXT_DUMP */

/*******************************************************************************
 * API
 ******************************************************************************/

status_t SLN_PRDB_EncryptionCheck(void)
{
    status_t status        = kStatus_Success;
    ctx_status_t ctxStatus = kContext_Good;
    bool isEncryptXIP      = ROM_OCOTP_ENCRYPT_XIP_VALAUE();
    bool isKey0SelSet      = (ROM_OCOTP_BEE_KEY0_SEL_VALUE() > 0);
    bool isKey1SelSet      = (ROM_OCOTP_BEE_KEY1_SEL_VALUE() > 0);

#if ENABLE_CONTEXT_DUMP
    SLN_PRDB_Display();
#endif /* ENABLE_CONTEXT_DUMP */

    if (isEncryptXIP && (isKey0SelSet || isKey1SelSet))
    {
        configPRINTF(("Running encrypted XIP.\r\n"));

        ctxStatus = SLN_PRDB_GetStatus();
        if (ctxStatus == kContext_Good)
        {
            configPRINTF(("All contexts are good.\r\n"));
        }
        else
        {
            /* Try to repair context. */
            configPRINTF(("WARNING: Bad or missing context, trying to repair.\r\n"));
            status = SLN_PRDB_Restore(ctxStatus);
            if (status != kStatus_Success)
            {
                configPRINTF(("ERROR: Failed to restore context %d.\r\n", status));
            }
        }
    }
    else
    {
        configPRINTF(("Running NON encrypted XIP.\r\n"));
    }

    return status;
}
