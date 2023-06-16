/*
 * Copyright 2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef VIT_DE_DE_STRINGS_TO_ACTIONS_VIT_H_
#define VIT_DE_DE_STRINGS_TO_ACTIONS_VIT_H_

#if ENABLE_VIT_ASR

#include "demo_actions_vit.h"
#include "stdint.h"

const int16_t actions_ww_de[] = {
    kWakeWord_Detected, // "Hallo NXP"
};

const int16_t actions_elevator_de[] = {
    kElevator_FirstFloor,     // "Erste etage"
    kElevator_SecondFloor,    // "Zweite etage"
    kElevator_ThirdFloor,     // "Dritte etage"
    kElevator_FourthFloor,    // "Vierte etage"
    kElevator_FifthFloor,     // "Fünfte etage"
    kElevator_MainLobby,      // "Hauptlobby"
    kElevator_GroundFloor,    // "Erdgeschoss"
    kElevator_BasementFloor,  // "Untergeschoss"
    kElevator_OpenDoor,       // "Öffne die tür"
    kElevator_CloseDoor,      // "Schließe die tür"
    kElevator_ChangeDemo,     // "Anwendung wechseln"
    kElevator_ChangeLanguage, // "Sprache wechseln"
    kElevator_Elevator,       // "Aufzug"
    kElevator_WashingMachine, // "Waschmaschine"
    kElevator_SmartHome,      // "Heimautomatisierung"
    kElevator_English,        // "Englisch"
    kElevator_French,         // "Franzosisch"
    kElevator_German,         // "Deutsch"
    kElevator_Chinese,        // "Chinesisch"
};

const int16_t actions_washing_machine_de[] = {
    kWashingMachine_Delicate,       // "Feinwäsche"
    kWashingMachine_Normal,         // "Normaler"
    kWashingMachine_HeavyDuty,      // "Stark verschmutze"
    kWashingMachine_Whites,         // "Weiß"
    kWashingMachine_Start,          // "Starten"
    kWashingMachine_Cancel,         // "Abbrechen"
    kWashingMachine_ChangeDemo,     // "Anwendung wechseln"
    kWashingMachine_ChangeLanguage, // "Sprache wechseln"
    kWashingMachine_Elevator,       // "Aufzug"
    kWashingMachine_WashingMachine, // "Waschmaschine"
    kWashingMachine_SmartHome,      // "Heimautomatisierung"
    kWashingMachine_English,        // "Englisch"
    kWashingMachine_French,         // "Franzosisch"
    kWashingMachine_German,         // "Deutsch"
    kWashingMachine_Chinese,        // "Chinesisch"
};

const int16_t actions_smart_home_de[] = {
    kSmartHome_TurnOnTheLights,   // "Licht einschalten"
    kSmartHome_TurnOffTheLights,  // "Licht ausschalten"
    kSmartHome_TemperatureHigher, // "Temperatur erhöhen"
    kSmartHome_TemperatureLower,  // "Temperatur verringern"
    kSmartHome_OpenTheWindow,     // "Fenster hoch"
    kSmartHome_CloseTheWindow,    // "Fenster runter"
    kSmartHome_MakeItBrighter,    // "Heller"
    kSmartHome_MakeItDarker,      // "Dunkler"
    kSmartHome_ChangeDemo,        // "Anwendung wechseln"
    kSmartHome_ChangeLanguage,    // "Sprache wechseln"
    kSmartHome_Elevator,          // "Aufzug"
    kSmartHome_WashingMachine,    // "Waschmaschine"
    kSmartHome_SmartHome,         // "Heimautomatisierung"
    kSmartHome_English,           // "Englisch"
    kSmartHome_French,            // "Franzosisch"
    kSmartHome_German,            // "Deutsch"
    kSmartHome_Chinese,           // "Chinesisch"
};

#endif /* ENABLE_VIT_ASR */
#endif /* VIT_DE_DE_STRINGS_TO_ACTIONS_VIT_H_ */
