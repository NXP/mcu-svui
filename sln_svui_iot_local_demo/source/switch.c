/*
 * Copyright 2019-2021 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "switch.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
static uint32_t current_state[SWITCH_COUNT];
static uint32_t last_state[SWITCH_COUNT] = {15, 15};
static uint32_t last_debounce[SWITCH_COUNT];

static switch_callback_t s_switch_callback = NULL;

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Interrupt service function of switch.
 */
void SWITCH_GPIO_IRQHandler(void)
{
    TickType_t current_time;
    uint32_t int_pin = 0x00;

    /* Get interrupt flag for the GPIO */
    int_pin = GPIO_PortGetInterruptFlags(SW1_GPIO);

    /* Check for the interrupt pin on the GPIO */
    if ((int_pin >> SW1_GPIO_PIN) & 0x01)
    {
        current_state[0] = GPIO_PinRead(SW1_GPIO, SW1_GPIO_PIN);

        if (current_state[0] != last_state[0])
        {
            current_time = xTaskGetTickCountFromISR() * 1000;

            if (current_time - last_debounce[0] > SWITCH_DEBOUNCE_TIME_US)
            {
                if (s_switch_callback != NULL)
                {
                    s_switch_callback(SWITCH_SW1, current_state[0]);
                }
            }

            last_debounce[0] = current_time;
        }

        /* clear the interrupt status */
        GPIO_PortClearInterruptFlags(SW1_GPIO, 1U << SW1_GPIO_PIN);

        last_state[0] = current_state[0];
    }

    if ((int_pin >> SW2_GPIO_PIN) & 0x01)
    {
        current_state[1] = GPIO_PinRead(SW2_GPIO, SW2_GPIO_PIN);

        if (current_state[1] != last_state[1])
        {
            current_time = xTaskGetTickCountFromISR() * 1000; // in us

            if (current_time - last_debounce[1] > SWITCH_DEBOUNCE_TIME_US)
            {
                if (s_switch_callback != NULL)
                {
                    s_switch_callback(SWITCH_SW2, current_state[1]);
                }
            }

            last_debounce[1] = current_time;
        }

        /* clear the interrupt status */
        GPIO_PortClearInterruptFlags(SW2_GPIO, 1U << SW2_GPIO_PIN);

        last_state[1] = current_state[1];
    }

    SDK_ISR_EXIT_BARRIER;
}

void SWITCH_Init(void)
{
    /* Define the init structure for the input switch pin */
    gpio_pin_config_t sw_config = {
        kGPIO_DigitalInput,
        0,
        kGPIO_IntRisingOrFallingEdge,
    };

    last_state[0] = SWITCH_RELEASED;
    last_state[1] = SWITCH_RELEASED;

    /* Init input switch GPIO. */
    NVIC_SetPriority(BOARD_USER_BUTTON_IRQ, configMAX_SYSCALL_INTERRUPT_PRIORITY - 1);
    EnableIRQ(BOARD_USER_BUTTON_IRQ);
    GPIO_PinInit(SW1_GPIO, SW1_GPIO_PIN, &sw_config);
    GPIO_PortClearInterruptFlags(SW1_GPIO, 1U << SW1_GPIO_PIN);

    GPIO_PinInit(SW2_GPIO, SW2_GPIO_PIN, &sw_config);
    GPIO_PortClearInterruptFlags(SW2_GPIO, 1U << SW2_GPIO_PIN);

    /* Enable GPIO pin interrupt */
    GPIO_PortEnableInterrupts(SW1_GPIO, 1U << SW1_GPIO_PIN);
    GPIO_PortEnableInterrupts(SW2_GPIO, 1U << SW2_GPIO_PIN);
}

void SWITCH_RegisterCallback(switch_callback_t cb)
{
    s_switch_callback = cb;
}
