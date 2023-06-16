/*
 * Copyright 2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef VIT_DE_DE_STRINGS_TO_PROMPTS_VIT_H_
#define VIT_DE_DE_STRINGS_TO_PROMPTS_VIT_H_

#if ENABLE_VIT_ASR

#include "sln_flash_files.h"

const char * const prompts_ww_de[] = {
    AUDIO_WW_DETECTED, // "Hallo NXP"
};

const char * const prompts_elevator_de[] = {
    AUDIO_FIRST_FLOOR_DE,    // "Erste etage"
    AUDIO_SECOND_FLOOR_DE,   // "Zweite etage"
    AUDIO_THIRD_FLOOR_DE,    // "Dritte etage"
    AUDIO_FOURTH_FLOOR_DE,   // "Vierte etage"
    AUDIO_FIFTH_FLOOR_DE,    // "Fünfte etage"
    AUDIO_MAIN_LOBBY_DE,     // "Hauptlobby"
    AUDIO_GROUND_FLOOR_DE,   // "Erdgeschoss"
    AUDIO_BASEMENT_FLOOR_DE, // "Untergeschoss"
    AUDIO_OPEN_DOOR_DE,      // "Öffne die tür"
    AUDIO_CLOSE_DOOR_DE,     // "Schließe die tür"
    AUDIO_DEMO_NAME_TEST_DE, // "Anwendung wechseln"
    AUDIO_LANGUAGE_TEST_DE,  // "Sprache wechseln"
    AUDIO_OK_DE,             // "Aufzug"
    AUDIO_OK_DE,             // "Waschmaschine"
    AUDIO_OK_DE,             // "Heimautomatisierung"
    AUDIO_OK_DE,             // "Englisch"
    AUDIO_OK_DE,             // "Franzosisch"
    AUDIO_OK_DE,             // "Deutsch"
    AUDIO_OK_DE,             // "Chinesisch"
};

const char * const prompts_washing_machine_de[] = {
    AUDIO_DELICATE_DE,       // "Feinwäsche"
    AUDIO_NORMAL_DE,         // "Normaler"
    AUDIO_HEAVY_DUTY_DE,     // "Stark verschmutze"
    AUDIO_WHITES_DE,         // "Weiß"
    AUDIO_START_DE,          // "Starten"
    AUDIO_CANCEL_DE,         // "Abbrechen"
    AUDIO_DEMO_NAME_TEST_DE, // "Anwendung wechseln"
    AUDIO_LANGUAGE_TEST_DE,  // "Sprache wechseln"
    AUDIO_OK_DE,             // "Aufzug"
    AUDIO_OK_DE,             // "Waschmaschine"
    AUDIO_OK_DE,             // "Heimautomatisierung"
    AUDIO_OK_DE,             // "Englisch"
    AUDIO_OK_DE,             // "Franzosisch"
    AUDIO_OK_DE,             // "Deutsch"
    AUDIO_OK_DE,             // "Chinesisch"
};

const char * const prompts_smart_home_de[] = {
    AUDIO_TURN_ON_THE_LIGHTS_DE,  // "Licht einschalten"
    AUDIO_TURN_OFF_THE_LIGHTS_DE, // "Licht ausschalten"
    AUDIO_TEMPERATURE_HIGHER_DE,  // "Temperatur erhöhen"
    AUDIO_TEMPERATURE_LOWER_DE,   // "Temperatur verringern"
    AUDIO_OPEN_THE_WINDOW_DE,     // "Fenster hoch"
    AUDIO_CLOSE_THE_WINDOW_DE,    // "Fenster runter"
    AUDIO_MAKE_IT_BRIGHTER_DE,    // "Heller"
    AUDIO_MAKE_IT_DARKER_DE,      // "Dunkler"
    AUDIO_DEMO_NAME_TEST_DE,      // "Anwendung wechseln"
    AUDIO_LANGUAGE_TEST_DE,       // "Sprache wechseln"
    AUDIO_OK_DE,                  // "Aufzug"
    AUDIO_OK_DE,                  // "Waschmaschine"
    AUDIO_OK_DE,                  // "Heimautomatisierung"
    AUDIO_OK_DE,                  // "Englisch"
    AUDIO_OK_DE,                  // "Franzosisch"
    AUDIO_OK_DE,                  // "Deutsch"
    AUDIO_OK_DE,                  // "Chinesisch"
};

#endif /* ENABLE_VIT_ASR */
#endif /* VIT_DE_DE_STRINGS_TO_PROMPTS_VIT_H_ */
