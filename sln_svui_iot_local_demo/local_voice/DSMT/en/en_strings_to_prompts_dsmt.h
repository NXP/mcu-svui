/*
 * Copyright 2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef DSMT_EN_EN_STRINGS_TO_PROMPTS_DSMT_H_
#define DSMT_EN_EN_STRINGS_TO_PROMPTS_DSMT_H_

#if ENABLE_DSMT_ASR

#include "sln_flash_files.h"

const char * const prompts_ww_en[] = {
    AUDIO_WW_DETECTED, // "Hey NXP"
    AUDIO_WW_DETECTED, // "Hey NXP ^1"
    AUDIO_WW_DETECTED, // "Hey NXP ^2"
    AUDIO_WW_DETECTED, // "Hey NXP ^3"
    AUDIO_WW_DETECTED, // "Hey NXP ^4"
};

const char * const prompts_change_demo_en[] = {
    AUDIO_OK_EN, // "Elevator"
    AUDIO_OK_EN, // "Washing Machine"
    AUDIO_OK_EN, // "Smart Home"
};

const char * const prompts_elevator_en[] = {
    AUDIO_FIRST_FLOOR_EN,    // "First floor"
    AUDIO_SECOND_FLOOR_EN,   // "Second floor"
    AUDIO_SECOND_FLOOR_EN,   // "Second floor ^1"
    AUDIO_THIRD_FLOOR_EN,    // "Third floor"
    AUDIO_FOURTH_FLOOR_EN,   // "Fourth floor"
    AUDIO_FIFTH_FLOOR_EN,    // "Fifth floor"
    AUDIO_FIFTH_FLOOR_EN,    // "Fifth floor ^1"
    AUDIO_MAIN_LOBBY_EN,     // "Main lobby"
    AUDIO_GROUND_FLOOR_EN,   // "Ground floor"
    AUDIO_BASEMENT_FLOOR_EN, // "Basement floor"
    AUDIO_OPEN_DOOR_EN,      // "Open door"
    AUDIO_CLOSE_DOOR_EN,     // "Close door"
    AUDIO_CLOSE_DOOR_EN,     // "Close door ^1"
    AUDIO_DEMO_NAME_TEST_EN  // "Change demo"
};

const char * const prompts_washing_machine_en[] = {
    AUDIO_DELICATE_EN,      // "Delicate"
    AUDIO_NORMAL_EN,        // "Normal"
    AUDIO_HEAVY_DUTY_EN,    // "Heavy duty"
    AUDIO_HEAVY_DUTY_EN,    // "Heavy duty ^1"
    AUDIO_WHITES_EN,        // "Whites",
    AUDIO_WHITES_EN,        // "Whites ^1"
    AUDIO_START_EN,         // "Start"
    AUDIO_CANCEL_EN,        // "Cancel"
    AUDIO_DEMO_NAME_TEST_EN // "Change demo"
};

const char * const prompts_smart_home_en[] = {
    AUDIO_TURN_ON_THE_LIGHTS_EN,  // "Turn on the lights"
    AUDIO_TURN_ON_THE_LIGHTS_EN,  // "Turn on the lights ^1"
    AUDIO_TURN_ON_THE_LIGHTS_EN,  // "Turn on the lights ^2"
    AUDIO_TURN_ON_THE_LIGHTS_EN,  // "Turn on the lights ^3"
    AUDIO_TURN_ON_THE_LIGHTS_EN,  // "Turn on the lights ^4"
    AUDIO_TURN_ON_THE_LIGHTS_EN,  // "Turn on the lights ^5"
    AUDIO_TURN_OFF_THE_LIGHTS_EN, // "Turn off the lights"
    AUDIO_TURN_OFF_THE_LIGHTS_EN, // "Turn off the lights ^1"
    AUDIO_TURN_OFF_THE_LIGHTS_EN, // "Turn off the lights ^2"
    AUDIO_TEMPERATURE_HIGHER_EN,  // "Temperature higher"
    AUDIO_TEMPERATURE_HIGHER_EN,  // "Temperature higher ^1"
    AUDIO_TEMPERATURE_LOWER_EN,   // "Temperature lower"
    AUDIO_TEMPERATURE_LOWER_EN,   // "Temperature lower ^1"
    AUDIO_OPEN_THE_WINDOW_EN,     // "Open the window"
    AUDIO_OPEN_THE_WINDOW_EN,     // "Open the window ^1"
    AUDIO_OPEN_THE_WINDOW_EN,     // "Open the window ^2"
    AUDIO_CLOSE_THE_WINDOW_EN,    // "Close the window"
    AUDIO_CLOSE_THE_WINDOW_EN,    // "Close the window ^1"
    AUDIO_CLOSE_THE_WINDOW_EN,    // "Close the window ^2"
    AUDIO_CLOSE_THE_WINDOW_EN,    // "Close the window ^3"
    AUDIO_CLOSE_THE_WINDOW_EN,    // "Close the window ^4"
    AUDIO_CLOSE_THE_WINDOW_EN,    // "Close the window ^5"
    AUDIO_MAKE_IT_BRIGHTER_EN,    // "Make it brighter"
    AUDIO_MAKE_IT_DARKER_EN,      // "Make it darker"
    AUDIO_DEMO_NAME_TEST_EN       // "Change demo"
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_EN_EN_STRINGS_TO_PROMPTS_DSMT_H_ */
