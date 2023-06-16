/*
 * Copyright 2022-2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef LOCAL_SOUNDS_TASK_H_
#define LOCAL_SOUNDS_TASK_H_

#include "fsl_common.h"
#include "stdint.h"

#if ENABLE_STREAMER

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Initialize the audio streamer.
 *
 * @return kStatus_Success if success.
 */
status_t LOCAL_SOUNDS_InitStreamer(void);

/**
 * @brief Set and play an offline audio.
 *        If there is another audio already playing, do nothing.
 *
 * @param audioFileName Path to the audio file in the filesystem.
 * @param volume Volume of the audio.
 *
 *  @return kStatus_Success if success.
 */
status_t LOCAL_SOUNDS_PlayAudioFile(char *audioFileName, int32_t volume);
/**
 * @brief Check if the audio streamer is playing a file
 *
 * @return true if it is playing.
 */
bool LOCAL_SOUNDS_isPlaying(void);

#endif /* ENABLE_STREAMER */
#endif /* LOCAL_SOUNDS_TASK_H_ */
