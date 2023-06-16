/*
 * Copyright 2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef DSMT_CN_CN_VOICE_DEMOS_DSMT_H_
#define DSMT_CN_CN_VOICE_DEMOS_DSMT_H_

#if ENABLE_DSMT_ASR

#include "sln_voice_demo.h"
#include "cn_strings_dsmt.h"
#include "cn_strings_to_actions_dsmt.h"
#include "cn_strings_to_prompts_dsmt.h"
#include "stddef.h"

extern unsigned int cn_model_begin;

const sln_voice_demo_t demo_change_demo_cn =
{
		ww_cn,
		cmd_change_demo_cn,
		actions_ww_cn,
		actions_change_demo_cn,
		prompts_ww_cn,
		prompts_change_demo_cn,
		AUDIO_DEMO_NAME_TEST_CN,
		NUM_ELEMENTS(ww_cn),
		NUM_ELEMENTS(cmd_change_demo_cn),
		&cn_model_begin,
		ASR_CHINESE,
		ASR_CMD_CHANGE_DEMO,
		LANG_STR_CN,
		DEMO_STR_CHANGE_DEMO,
};

const sln_voice_demo_t demo_elevator_cn =
{
		ww_cn,
		cmd_elevator_cn,
		actions_ww_cn,
		actions_elevator_cn,
		prompts_ww_cn,
		prompts_elevator_cn,
		AUDIO_ELEVATOR_MULTILINGUAL_CN,
		NUM_ELEMENTS(ww_cn),
		NUM_ELEMENTS(cmd_elevator_cn),
		&cn_model_begin,
		ASR_CHINESE,
		ASR_CMD_ELEVATOR,
		LANG_STR_CN,
		DEMO_STR_ELEVATOR,
};

const sln_voice_demo_t demo_washing_machine_cn =
{
		ww_cn,
		cmd_washing_machine_cn,
		actions_ww_cn,
		actions_washing_machine_cn,
		prompts_ww_cn,
		prompts_washing_machine_cn,
		AUDIO_WASHING_MACHINE_MULTILINGUAL_CN,
		NUM_ELEMENTS(ww_cn),
		NUM_ELEMENTS(cmd_washing_machine_cn),
		&cn_model_begin,
		ASR_CHINESE,
		ASR_CMD_WASHING_MACHINE,
		LANG_STR_CN,
		DEMO_STR_WASHING,
};

const sln_voice_demo_t demo_smart_home_cn =
{
		ww_cn,
		cmd_smart_home_cn,
		actions_ww_cn,
		actions_smart_home_cn,
		prompts_ww_cn,
		prompts_smart_home_cn,
		AUDIO_SMART_HOME_MULTILINGUAL_CN,
		NUM_ELEMENTS(ww_cn),
		NUM_ELEMENTS(cmd_smart_home_cn),
		&cn_model_begin,
		ASR_CHINESE,
		ASR_CMD_SMART_HOME,
		LANG_STR_CN,
		DEMO_STR_SMART_HOME,
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_CN_CN_VOICE_DEMOS_DSMT_H_ */
