/*
 * Copyright 2022 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef SLN_I2S_MIC_PROCESSING_H_
#define SLN_I2S_MIC_PROCESSING_H_

#include "sln_mic_config.h"

#if (MICS_TYPE == MICS_I2S)

#include "stdint.h"

/**
 * @brief Process microphones samples.
 *        Samples should be arranged in the following format: mic1 mic2 .. micN, mic1 mic2 .. micN, etc.
 *        Re-arange the samples in the following format: mic1 mic1 .. mic1, mic2 mic2 .. mic2, micN micN .. micN.
 *        Apply High Pass Filter to center audio amplitudes to zero.
 *
 * @param in Pointer to the buffer containing the samples.
 * @param out Pointer where to store processed data.
 */
void I2S_MIC_ProcessMicStream(uint8_t *in, int16_t *out);

#endif /* (MICS_TYPE == MICS_I2S) */

#endif /* SLN_I2S_MIC_PROCESSING_H_ */
