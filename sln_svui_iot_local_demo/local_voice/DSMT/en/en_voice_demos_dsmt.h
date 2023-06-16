/*
 * Copyright 2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef DSMT_EN_EN_VOICE_DEMOS_DSMT_H_
#define DSMT_EN_EN_VOICE_DEMOS_DSMT_H_

#if ENABLE_DSMT_ASR

#include "sln_voice_demo.h"
#include "en_strings_dsmt.h"
#include "en_strings_to_actions_dsmt.h"
#include "en_strings_to_prompts_dsmt.h"
#include "stddef.h"

extern unsigned int en_model_begin;

const sln_voice_demo_t demo_change_demo_en =
{
		ww_en,
		cmd_change_demo_en,
		actions_ww_en,
		actions_change_demo_en,
		prompts_ww_en,
		prompts_change_demo_en,
		AUDIO_DEMO_NAME_TEST_EN,
		NUM_ELEMENTS(ww_en),
		NUM_ELEMENTS(cmd_change_demo_en),
		&en_model_begin,
		ASR_ENGLISH,
		ASR_CMD_CHANGE_DEMO,
		LANG_STR_EN,
		DEMO_STR_CHANGE_DEMO,
};

const sln_voice_demo_t demo_elevator_en =
{
		ww_en,
		cmd_elevator_en,
		actions_ww_en,
		actions_elevator_en,
		prompts_ww_en,
		prompts_elevator_en,
		AUDIO_ELEVATOR_MULTILINGUAL_EN,
		NUM_ELEMENTS(ww_en),
		NUM_ELEMENTS(cmd_elevator_en),
		&en_model_begin,
		ASR_ENGLISH,
		ASR_CMD_ELEVATOR,
		LANG_STR_EN,
		DEMO_STR_ELEVATOR,
};

const sln_voice_demo_t demo_washing_machine_en =
{
		ww_en,
		cmd_washing_machine_en,
		actions_ww_en,
		actions_washing_machine_en,
		prompts_ww_en,
		prompts_washing_machine_en,
		AUDIO_WASHING_MACHINE_MULTILINGUAL_EN,
		NUM_ELEMENTS(ww_en),
		NUM_ELEMENTS(cmd_washing_machine_en),
		&en_model_begin,
		ASR_ENGLISH,
		ASR_CMD_WASHING_MACHINE,
		LANG_STR_EN,
		DEMO_STR_WASHING,
};

const sln_voice_demo_t demo_smart_home_en =
{
		ww_en,
		cmd_smart_home_en,
		actions_ww_en,
		actions_smart_home_en,
		prompts_ww_en,
		prompts_smart_home_en,
		AUDIO_SMART_HOME_MULTILINGUAL_EN,
		NUM_ELEMENTS(ww_en),
		NUM_ELEMENTS(cmd_smart_home_en),
		&en_model_begin,
		ASR_ENGLISH,
		ASR_CMD_SMART_HOME,
		LANG_STR_EN,
		DEMO_STR_SMART_HOME,
};

#endif /* ENABLE_DSMT_ASR */

#endif /* DSMT_EN_EN_VOICE_DEMOS_DSMT_H_ */
