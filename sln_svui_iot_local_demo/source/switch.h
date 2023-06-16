/*
 * Copyright 2019-2021 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef SWITCH_H_
#define SWITCH_H_

#include "FreeRTOS.h"
#include "board.h"
#include "event_groups.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

typedef enum _switch_events
{
    SWITCH_PRESS_SW1   = (1 << 0),
    SWITCH_RELEASE_SW1 = (1 << 1),
    SWITCH_PRESS_SW2   = (1 << 2),
    SWITCH_RELEASE_SW2 = (1 << 3),
} switch_events_t;

#define SWITCH_COUNT            (2)
#define SWITCH_GPIO_IRQHandler  (BOARD_USER_BUTTON_IRQ_HANDLER)
#define SWITCH_PRESSED          (0)
#define SWITCH_RELEASED         (1)
#define SWITCH_DEBOUNCE_TIME_US (50000)
#define SWITCH_SW1              (1)
#define SWITCH_SW2              (2)

typedef void (*switch_callback_t)(int32_t button_nr, int32_t state);

/*******************************************************************************
 * Prototype
 ******************************************************************************/
/*!
 * @brief Initializes the GPIO and interrupts for the buttons
 */
void SWITCH_Init(void);
void SWITCH_RegisterCallback(switch_callback_t cb);

#endif /* SWITCH_H_ */
