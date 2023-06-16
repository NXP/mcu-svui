/*
 * Copyright 2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef SLN_VOICE_DEMO_H_
#define SLN_VOICE_DEMO_H_

#include "stdint.h"

#define NUM_ELEMENTS(x) (sizeof(x) / sizeof(x[0]))

typedef struct _sln_voice_demo
{
    const char * const * ww_strings;
    const char * const *cmd_strings;
    int16_t const * ww_actions;
    int16_t const * cmd_actions;
    const char * const * ww_prompts;
    const char * const * cmd_prompts;
    const char *demo_prompt;
    int16_t num_ww_strings;
    int16_t num_cmd_strings;
    void *model;
    int language_id;
    int demo_id;
    const char *language_str;
    const char *demo_str;
} sln_voice_demo_t;

#endif /* SLN_VOICE_DEMO_H_ */
