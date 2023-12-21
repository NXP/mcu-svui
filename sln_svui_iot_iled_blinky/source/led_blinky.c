/*
 * Copyright 2019, 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "sln_rgb_led_driver.h"

#include "system_MIMXRT106S.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile uint32_t g_systickCounter;
/* The PIN status */
volatile bool g_pinSet = false;

/*******************************************************************************
 * Code
 ******************************************************************************/
void SysTick_Handler(void)
{
    if (g_systickCounter != 0U)
    {
        g_systickCounter--;
    }
}

void SysTick_DelayTicks(uint32_t n)
{
    g_systickCounter = n;
    while (g_systickCounter != 0U)
    {
    }
}

/*!
 * @brief Main function
 */
int main(void)
{
    /* Board pin init */
    BOARD_InitBootPins();
    BOARD_BootClockRUN();
    RGB_LED_Init();
    /* Update the core clock */
    SystemCoreClockUpdate();

    /* Set systick reload value to generate 1ms interrupt */
    if (SysTick_Config(SystemCoreClock / 1000U))
    {
        while (1)
        {
        }
    }

    while (1)
    {
        /* Delay 1000 ms */
        SysTick_DelayTicks(1000U);
        if (g_pinSet)
        {
            RGB_LED_SetColor(LED_COLOR_RED);
            g_pinSet = false;
        }
        else
        {
            RGB_LED_SetColor(LED_COLOR_OFF);
            g_pinSet = true;
        }
    }
}
