/*
 * Copyright 2018-2021 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.d
 */

#ifndef _AUDIO_PROCESSING_TASK_H_
#define _AUDIO_PROCESSING_TASK_H_

#include <stdint.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief
 */
void audio_processing_task(void *pvParameters);

/*!
 * @brief
 */
void audio_processing_set_mic_input_buffer(int16_t *buf);

/*!
 * @brief
 */
void audio_processing_set_amp_input_buffer(int16_t *buf);

#if defined(__cplusplus)
}
#endif

#endif /* _AUDIO_PROCESSING_TASK_H_ */
