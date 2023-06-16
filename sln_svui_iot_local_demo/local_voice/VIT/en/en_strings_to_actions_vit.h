/*
 * Copyright 2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef VIT_EN_EN_STRINGS_TO_ACTIONS_VIT_H_
#define VIT_EN_EN_STRINGS_TO_ACTIONS_VIT_H_

#if ENABLE_VIT_ASR

#include "demo_actions_vit.h"
#include "stdint.h"

const int16_t actions_ww_en[] = {
    kWakeWord_Detected, // "Hey NXP"
};

const int16_t actions_elevator_en[] = {
    kElevator_FirstFloor,     // "First floor"
    kElevator_SecondFloor,    // "Second floor"
    kElevator_ThirdFloor,     // "Third floor"
    kElevator_FourthFloor,    // "Fourth floor"
    kElevator_FifthFloor,     // "Fifth floor"
    kElevator_MainLobby,      // "Main lobby"
    kElevator_GroundFloor,    // "Ground floor"
    kElevator_BasementFloor,  // "Basement floor"
    kElevator_OpenDoor,       // "Open door"
    kElevator_CloseDoor,      // "Close door"
    kElevator_ChangeDemo,     // "Change demo"
    kElevator_ChangeLanguage, // "Change language"
    kElevator_Elevator,       // "Elevator"
    kElevator_WashingMachine, // "Washing Machine"
    kElevator_SmartHome,      // "Smart Home"
    kElevator_English,        // "English"
    kElevator_French,         // "French"
    kElevator_German,         // "German"
    kElevator_Chinese,        // "Chinese"
};

const int16_t actions_washing_machine_en[] = {
    kWashingMachine_Delicate,       // "Delicate"
    kWashingMachine_Normal,         // "Normal"
    kWashingMachine_HeavyDuty,      // "Heavy duty"
    kWashingMachine_Whites,         // "Whites"
    kWashingMachine_Start,          // "Start"
    kWashingMachine_Cancel,         // "Cancel"
    kWashingMachine_ChangeDemo,     // "Change demo"
    kWashingMachine_ChangeLanguage, // "Change language"
    kWashingMachine_Elevator,       // "Elevator"
    kWashingMachine_WashingMachine, // "Washing Machine"
    kWashingMachine_SmartHome,      // "Smart Home"
    kWashingMachine_English,        // "English"
    kWashingMachine_French,         // "French"
    kWashingMachine_German,         // "German"
    kWashingMachine_Chinese,        // "Chinese"
};

const int16_t actions_smart_home_en[] = {
    kSmartHome_TurnOnTheLights,   // "Turn on the lights"
    kSmartHome_TurnOffTheLights,  // "Turn off the lights"
    kSmartHome_TemperatureHigher, // "Temperature higher"
    kSmartHome_TemperatureLower,  // "Temperature lower"
    kSmartHome_OpenTheWindow,     // "Open the window"
    kSmartHome_CloseTheWindow,    // "Close the window"
    kSmartHome_MakeItBrighter,    // "Make it brighter"
    kSmartHome_MakeItDarker,      // "Make it darker"
    kSmartHome_ChangeDemo,        // "Change demo"
    kSmartHome_ChangeLanguage,    // "Change language"
    kSmartHome_Elevator,          // "Elevator"
    kSmartHome_WashingMachine,    // "Washing Machine"
    kSmartHome_SmartHome,         // "Smart Home"
    kSmartHome_English,           // "English"
    kSmartHome_French,            // "French"
    kSmartHome_German,            // "German"
    kSmartHome_Chinese,           // "Chinese"
};

#endif /* ENABLE_VIT_ASR */
#endif /* VIT_EN_EN_STRINGS_TO_ACTIONS_VIT_H_ */
