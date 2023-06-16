/*
 * Copyright 2022-2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef INDEXCOMMANDS_H_
#define INDEXCOMMANDS_H_

#include "stdbool.h"

#if ENABLE_DSMT_ASR
#include "sln_local_voice_dsmt.h"
#elif ENABLE_VIT_ASR
#include "sln_local_voice_vit.h"
#endif /* ENABLE_DSMT_ASR */

uint16_t get_cmd_number(asr_language_t asrLang, asr_inference_t infCMDType);
char **get_cmd_strings(asr_language_t asrLang, asr_inference_t infCMDType);
char **get_ww_strings(asr_language_t asrLang);
uint16_t get_action_from_keyword(asr_language_t asrLang, asr_inference_t infCMDType, uint8_t keywordId);
char *get_prompt_from_keyword(asr_language_t asrLang, asr_inference_t infCMDType, uint8_t keywordId);
char *get_demo_prompt(asr_inference_t demoType, asr_language_t asrLang);
void *get_demo_model(asr_language_t asrLang, asr_inference_t infCMDType);
const char *get_demo_string(asr_language_t asrLang, asr_inference_t infCMDType);
char *get_language_str_from_id(uint16_t language_id);
uint16_t get_language_id_from_str(char *language_str);
uint16_t get_demo_id_from_str(char *demo_str);
void print_active_languages_str(uint16_t active_languages, uint16_t demo_id);
bool validate_all_active_languages(asr_language_t asrLang, asr_inference_t infCMDType);
uint16_t active_languages_get_first(asr_language_t asrLang);

#endif /* INDEXCOMMANDS_H_ */
