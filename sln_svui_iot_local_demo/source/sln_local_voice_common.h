/*
 * Copyright 2022-2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef SLN_LOCAL_VOICE_COMMON_H_
#define SLN_LOCAL_VOICE_COMMON_H_

#include "stdint.h"
#include "string.h"

#if ENABLE_DSMT_ASR
#include "sln_local_voice_dsmt.h"
#elif ENABLE_VIT_ASR
#include "sln_local_voice_vit.h"
#else
#error "Must use either ENABLE_DSMT_ASR or ENABLE_VIT_ASR"
#endif

// Shell Commands Related
#define ASR_SHELL_COMMANDS_FILE_NAME "asr_shell_commands.dat"

/*!
 * @brief Print current ASR engine and its version
 */
void print_asr_version(void);

/*!
 * @brief Task responsible for ASR initialization and ASR processing.
 *
 * The ASR engine is specified in app.h
 */
void local_voice_task(void *arg);

#endif /* SLN_LOCAL_VOICE_COMMON_H_ */
