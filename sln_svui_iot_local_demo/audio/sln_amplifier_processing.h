/*
 * Copyright 2022 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef SLN_AMPLIFIER_PROCESSING_H_
#define SLN_AMPLIFIER_PROCESSING_H_

#if !AMP_LOOPBACK_DISABLED

#include "stdint.h"
#include "sln_mic_config.h"

/**
 * @brief Retrieve amplifier's data from its ring buffer, down sample the data and copy it to the output buffer.
 *        In case amplifier's ring buffer is empty, schedule future clears of the output buffer.
 *
 * @param s_taskConfig Pointer to the structure containing information about current audio chain.
 * @param buffOut Pointer to the buffer where to store PCM data.
 * @param s_pingPongTimestamp Pointer to the latest timestamp updated by mic callbacks.
 */
void SLN_AMP_GetAmpStream(mic_task_config_t *s_taskConfig, int16_t *buffOut, volatile uint32_t *s_pingPongTimestamp);

#endif /* !AMP_LOOPBACK_DISABLED */

#endif /* SLN_AMPLIFIER_PROCESSING_H_ */
