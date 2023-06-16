/*
 * Copyright 2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef VIT_FR_FR_VOICE_DEMOS_VIT_H_
#define VIT_FR_FR_VOICE_DEMOS_VIT_H_

#if ENABLE_VIT_ASR

#include "sln_voice_demo.h"
#include "fr_strings_vit.h"
#include "fr_strings_to_actions_vit.h"
#include "fr_strings_to_prompts_vit.h"
#include "demo_definitions_vit.h"
#include "stddef.h"

#include "PL_platformTypes_CortexM.h"
#include "VIT_Model_fr_Elevator.h"
#include "VIT_Model_fr_SmartHome.h"
#include "VIT_Model_fr_WashingMachine.h"

const sln_voice_demo_t demo_elevator_fr =
{
		ww_fr,
		cmd_elevator_fr,
		actions_ww_fr,
		actions_elevator_fr,
		prompts_ww_fr,
		prompts_elevator_fr,
		AUDIO_ELEVATOR_DEMO_FR,
		NUM_ELEMENTS(ww_fr),
		NUM_ELEMENTS(cmd_elevator_fr),
		(void *)VIT_Model_fr_elevator,
		ASR_FRENCH,
		ASR_CMD_ELEVATOR,
		LANG_STR_FR,
		DEMO_STR_ELEVATOR,
};

const sln_voice_demo_t demo_washing_machine_fr =
{
		ww_fr,
		cmd_washing_machine_fr,
		actions_ww_fr,
		actions_washing_machine_fr,
		prompts_ww_fr,
		prompts_washing_machine_fr,
		AUDIO_WASHING_MACHINE_DEMO_FR,
		NUM_ELEMENTS(ww_fr),
		NUM_ELEMENTS(cmd_washing_machine_fr),
		(void *)VIT_Model_fr_washingmachine,
		ASR_FRENCH,
		ASR_CMD_WASHING_MACHINE,
		LANG_STR_FR,
		DEMO_STR_WASHING,
};

const sln_voice_demo_t demo_smart_home_fr =
{
		ww_fr,
		cmd_smart_home_fr,
		actions_ww_fr,
		actions_smart_home_fr,
		prompts_ww_fr,
		prompts_smart_home_fr,
		AUDIO_SMART_HOME_DEMO_FR,
		NUM_ELEMENTS(ww_fr),
		NUM_ELEMENTS(cmd_smart_home_fr),
		(void *)VIT_Model_fr_smarthome,
		ASR_FRENCH,
		ASR_CMD_SMART_HOME,
		LANG_STR_FR,
		DEMO_STR_SMART_HOME,
};

#endif /* ENABLE_VIT_ASR */
#endif /* VIT_FR_FR_VOICE_DEMOS_VIT_H_ */
