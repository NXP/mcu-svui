/*
 * Copyright 2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef DSMT_DE_DE_VOICE_DEMOS_DSMT_H_
#define DSMT_DE_DE_VOICE_DEMOS_DSMT_H_

#if ENABLE_DSMT_ASR

#include "sln_voice_demo.h"
#include "de_strings_dsmt.h"
#include "de_strings_to_actions_dsmt.h"
#include "de_strings_to_prompts_dsmt.h"
#include "stddef.h"

extern unsigned int de_model_begin;

const sln_voice_demo_t demo_change_demo_de =
{
		ww_de,
		cmd_change_demo_de,
		actions_ww_de,
		actions_change_demo_de,
		prompts_ww_de,
		prompts_change_demo_de,
		AUDIO_DEMO_NAME_TEST_DE,
		NUM_ELEMENTS(ww_de),
		NUM_ELEMENTS(cmd_change_demo_de),
		&de_model_begin,
		ASR_GERMAN,
		ASR_CMD_CHANGE_DEMO,
		LANG_STR_DE,
		DEMO_STR_CHANGE_DEMO,
};

const sln_voice_demo_t demo_elevator_de =
{
		ww_de,
		cmd_elevator_de,
		actions_ww_de,
		actions_elevator_de,
		prompts_ww_de,
		prompts_elevator_de,
		AUDIO_ELEVATOR_MULTILINGUAL_DE,
		NUM_ELEMENTS(ww_de),
		NUM_ELEMENTS(cmd_elevator_de),
		&de_model_begin,
		ASR_GERMAN,
		ASR_CMD_ELEVATOR,
		LANG_STR_DE,
		DEMO_STR_ELEVATOR,
};

const sln_voice_demo_t demo_washing_machine_de =
{
		ww_de,
		cmd_washing_machine_de,
		actions_ww_de,
		actions_washing_machine_de,
		prompts_ww_de,
		prompts_washing_machine_de,
		AUDIO_WASHING_MACHINE_MULTILINGUAL_DE,
		NUM_ELEMENTS(ww_de),
		NUM_ELEMENTS(cmd_washing_machine_de),
		&de_model_begin,
		ASR_GERMAN,
		ASR_CMD_WASHING_MACHINE,
		LANG_STR_DE,
		DEMO_STR_WASHING,
};

const sln_voice_demo_t demo_smart_home_de =
{
		ww_de,
		cmd_smart_home_de,
		actions_ww_de,
		actions_smart_home_de,
		prompts_ww_de,
		prompts_smart_home_de,
		AUDIO_SMART_HOME_MULTILINGUAL_DE,
		NUM_ELEMENTS(ww_de),
		NUM_ELEMENTS(cmd_smart_home_de),
		&de_model_begin,
		ASR_GERMAN,
		ASR_CMD_SMART_HOME,
		LANG_STR_DE,
		DEMO_STR_SMART_HOME,
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_DE_DE_VOICE_DEMOS_DSMT_H_ */
