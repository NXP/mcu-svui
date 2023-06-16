/*
 * Copyright 2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef VIT_EN_EN_STRINGS_TO_PROMPTS_VIT_H_
#define VIT_EN_EN_STRINGS_TO_PROMPTS_VIT_H_

#if ENABLE_VIT_ASR

#include "sln_flash_files.h"

const char * const prompts_ww_en[] = {
    AUDIO_WW_DETECTED, // "Hey NXP"
};

const char * const prompts_elevator_en[] = {
    AUDIO_FIRST_FLOOR_EN,    // "First floor"
    AUDIO_SECOND_FLOOR_EN,   // "Second floor"
    AUDIO_THIRD_FLOOR_EN,    // "Third floor"
    AUDIO_FOURTH_FLOOR_EN,   // "Fourth floor"
    AUDIO_FIFTH_FLOOR_EN,    // "Fifth floor"
    AUDIO_MAIN_LOBBY_EN,     // "Main lobby"
    AUDIO_GROUND_FLOOR_EN,   // "Ground floor"
    AUDIO_BASEMENT_FLOOR_EN, // "Basement floor"
    AUDIO_OPEN_DOOR_EN,      // "Open door"
    AUDIO_CLOSE_DOOR_EN,     // "Close door"
    AUDIO_DEMO_NAME_TEST_EN, // "Change demo"
    AUDIO_LANGUAGE_TEST_EN,  // "Change language"
    AUDIO_OK_EN,             // "Elevator"
    AUDIO_OK_EN,             // "Washing Machine"
    AUDIO_OK_EN,             // "Smart Home"
    AUDIO_OK_EN,             // "English"
    AUDIO_OK_EN,             // "French"
    AUDIO_OK_EN,             // "German"
    AUDIO_OK_EN,             // "Chinese"
};

const char * const prompts_washing_machine_en[] = {
    AUDIO_DELICATE_EN,       // "Delicate"
    AUDIO_NORMAL_EN,         // "Normal"
    AUDIO_HEAVY_DUTY_EN,     // "Heavy duty"
    AUDIO_WHITES_EN,         // "Whites",
    AUDIO_START_EN,          // "Start"
    AUDIO_CANCEL_EN,         // "Cancel"
    AUDIO_DEMO_NAME_TEST_EN, // "Change demo"
    AUDIO_LANGUAGE_TEST_EN,  // "Change language"
    AUDIO_OK_EN,             // "Elevator"
    AUDIO_OK_EN,             // "Washing Machine"
    AUDIO_OK_EN,             // "Smart Home"
    AUDIO_OK_EN,             // "English"
    AUDIO_OK_EN,             // "French"
    AUDIO_OK_EN,             // "German"
    AUDIO_OK_EN,             // "Chinese"
};

const char * const prompts_smart_home_en[] = {
    AUDIO_TURN_ON_THE_LIGHTS_EN,  // "Turn on the lights"
    AUDIO_TURN_OFF_THE_LIGHTS_EN, // "Turn off the lights"
    AUDIO_TEMPERATURE_HIGHER_EN,  // "Temperature higher"
    AUDIO_TEMPERATURE_LOWER_EN,   // "Temperature lower"
    AUDIO_OPEN_THE_WINDOW_EN,     // "Open the window"
    AUDIO_CLOSE_THE_WINDOW_EN,    // "Close the window"
    AUDIO_MAKE_IT_BRIGHTER_EN,    // "Make it brighter"
    AUDIO_MAKE_IT_DARKER_EN,      // "Make it darker"
    AUDIO_DEMO_NAME_TEST_EN,      // "Change demo"
    AUDIO_LANGUAGE_TEST_EN,       // "Change language"
    AUDIO_OK_EN,                  // "Elevator"
    AUDIO_OK_EN,                  // "Washing Machine"
    AUDIO_OK_EN,                  // "Smart Home"
    AUDIO_OK_EN,                  // "English"
    AUDIO_OK_EN,                  // "French"
    AUDIO_OK_EN,                  // "German"
    AUDIO_OK_EN,                  // "Chinese"
};

#endif /* ENABLE_VIT_ASR */
#endif /* VIT_EN_EN_STRINGS_TO_PROMPTS_VIT_H_ */
