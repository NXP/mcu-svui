/*
 * Copyright 2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#if ENABLE_DSMT_ASR

#include "demo_definitions_dsmt.h"
#include "cn_voice_demos_dsmt.h"
#include "de_voice_demos_dsmt.h"
#include "en_voice_demos_dsmt.h"
#include "fr_voice_demos_dsmt.h"
#include "sln_voice_demo.h"

sln_voice_demo_t const * const all_voice_demos_dsmt[] = {
    &demo_change_demo_en,
    &demo_elevator_en,
    &demo_washing_machine_en,
    &demo_smart_home_en,
    &demo_change_demo_fr,
    &demo_elevator_fr,
    &demo_washing_machine_fr,
    &demo_smart_home_fr,
    &demo_change_demo_de,
    &demo_elevator_de,
    &demo_washing_machine_de,
    &demo_smart_home_de,
    &demo_change_demo_cn,
    &demo_elevator_cn,
    &demo_washing_machine_cn,
    &demo_smart_home_cn,
    NULL // end with NULL to signal list ending
};

#endif /* ENABLE_DSMT_ASR */
