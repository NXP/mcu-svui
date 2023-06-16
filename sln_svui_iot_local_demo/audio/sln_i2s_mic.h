/*
 * Copyright 2022 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef SLN_I2S_MIC_H_
#define SLN_I2S_MIC_H_

#include "sln_mic_config.h"

#if (MICS_TYPE == MICS_I2S)

#include "stdbool.h"
#include "stdint.h"

/**
 * @brief Return the pointer to the buffer containing PCM mic data.
 *
 * @return Pointer to the buffer containing PCM mic data.
 */
int16_t *I2S_MIC_GetPcmBufferPointer(void);

/**
 * @brief Return the pointer to the buffer containing PCM amp data.
 *
 * @return Pointer to the buffer containing PCM amp data.
 */
int16_t *I2S_MIC_GetAmpBufferPointer(void);

/**
 * @brief Configure Microphone layer.
 *
 * @param config Configuration to be used.
 */
void I2S_MIC_SetTaskConfig(mic_task_config_t *config);

/**
 * @brief Turn on the microphones.
 */
void I2S_MIC_MicsOn(void);

/**
 * @brief Turn off the microphones.
 */
void I2S_MIC_MicsOff(void);

/**
 * @brief Return current state of the mics.
 *
 * @return Return current state of the mics (true == mics on, false == mics off).
 */
bool I2S_MIC_GetMicsState(void);

/**
 * @brief I2S microphones processing task function.
 *        Application layer should create a task based on this function.
 *
 * @param pvParameters Pointer to the parameters which should be passed to the Task. May be NULL.
 */
void I2S_MIC_Task(void *pvParameters);

#endif /* (MICS_TYPE == MICS_I2S) */

#endif /* SLN_I2S_MIC_H_ */
