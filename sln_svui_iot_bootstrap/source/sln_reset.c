/*
 * Copyright 2020 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"

#define RESET_PREFIX_MSG "[System reset]"

#define RESET_DELAY_MS    500
#define RESET_DELAY_TICKS (RESET_DELAY_MS * portTICK_PERIOD_MS)

void sln_reset(const char *msg)
{
    char prefix_msg[] = RESET_PREFIX_MSG;

    if (msg == NULL)
    {
        configPRINTF(("%s - no reason specified\r\n", prefix_msg));
    }
    else
    {
        configPRINTF(("%s - %s\r\n", prefix_msg, msg));
    }

    vTaskDelay(RESET_DELAY_TICKS);

    NVIC_SystemReset();
}
