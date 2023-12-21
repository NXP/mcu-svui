/*
 * Copyright 2019-2021, 2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "comms_message_handler.h"
#include "flash_ica_driver.h"
#include "fsl_debug_console.h"
#include "sln_fwupdate.h"
#include "sln_push_buttons_driver.h"
#include "sln_update.h"

bool FWUpdate_CheckForUpdate(void)
{
    status_t status   = kStatus_Success;
    bool update       = false;
    bool fwupdateflag = false;

    /* Read the FWUPDATE bit from flash */
    status = FICA_initialize();
    if (SLN_FLASH_NO_ERROR == status)
    {
        status = FICA_get_comm_flag(FICA_COMM_FWUPDATE_BIT, &fwupdateflag);
    }

    /* Check if the flag for update has been set */
    if ((SLN_FLASH_NO_ERROR == status) && (0 == fwupdateflag))
    {
        /* Flag for a fwUpdate is set check the methods */
        bool methodFlag = false;

        /* Read the method flag <1-OTA | 0-OTW> */
        status = FICA_get_comm_flag(FICA_COMM_FWUPDATE_METHOD_BIT, &methodFlag);
        if (SLN_FLASH_NO_ERROR == status)
        {
            /* Set the bit for fwupdate and method at the same time */
            status = FICA_set_comm_flag(FICA_COMM_FWUPDATE_BIT);
            if (SLN_FLASH_NO_ERROR != status)
            {
                /* Skip if we were unable to clear the flag for FW or read the method */
                configPRINTF(("Failed to set the firmware update flag \r\n"));
                update = false;
            }
#if COMMS_MESSAGE_HANDLER_FWUPDATE_METHOD_OTA_TCP
            else if (METHOD_SLN_OTA == methodFlag)
            {
                /* We have an ota triggered */
                otaAppStart();
                update = true;
            }
#endif /* COMMS_MESSAGE_HANDLER_FWUPDATE_METHOD_OTA_TCP */
#if COMMS_MESSAGE_HANDLER_FWUPDATE_METHOD_OTW_UART
            else if (METHOD_SLN_OTW == methodFlag)
            {
                /* We have an otw triggered */
                otwAppStart();
                update = true;
            }
#endif /* COMMS_MESSAGE_HANDLER_FWUPDATE_METHOD_OTW_UART */
        }
    }
    else if (SLN_FLASH_NO_ERROR == status)
    {
        /* Clear the flag for the method used for update as the update has finished */
        status = FICA_set_comm_flag(FICA_COMM_FWUPDATE_METHOD_BIT);
        if (SLN_FLASH_NO_ERROR != status)
        {
            configPRINTF(("Failed to reset the method flag\r\n"));
        }
        update = false;
    }

    return update;
}

#if COMMS_MESSAGE_HANDLER_FWUPDATE_METHOD_OTW_UART
void otwAppStart()
{
    PRINTF("Starting the OTW server\r\n");
    /* Flush the console before we deinitialize it */
    DbgConsole_Flush();
    DbgConsole_Deinit();

    SLN_COMMS_MESSAGE_Init(kCommonInterfaceUart);
}
#endif /* COMMS_MESSAGE_HANDLER_FWUPDATE_METHOD_OTW_UART */

#if COMMS_MESSAGE_HANDLER_FWUPDATE_METHOD_OTA_TCP
void otaAppStart()
{
    PRINTF("Starting the OTA TCP server\r\n");
    SLN_COMMS_MESSAGE_Init(kCommonInterfaceTcp);
}
#endif /* COMMS_MESSAGE_HANDLER_FWUPDATE_METHOD_OTA_TCP */
