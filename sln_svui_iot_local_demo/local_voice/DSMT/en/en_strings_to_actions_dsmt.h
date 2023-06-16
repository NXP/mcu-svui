/*
 * Copyright 2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef DSMT_EN_EN_STRINGS_TO_ACTIONS_DSMT_H_
#define DSMT_EN_EN_STRINGS_TO_ACTIONS_DSMT_H_

#if ENABLE_DSMT_ASR

#include "demo_actions_dsmt.h"
#include "stdint.h"

const int16_t actions_ww_en[] = {
    kWakeWord_Detected, // "Hey NXP"
    kWakeWord_Detected, // "Hey NXP ^1"
    kWakeWord_Detected, // "Hey NXP ^2"
    kWakeWord_Detected, // "Hey NXP ^3"
    kWakeWord_Detected, // "Hey NXP ^4"
};

const int16_t actions_change_demo_en[] = {
    kChangeDemo_Elevator,       // "Elevator"
    kChangeDemo_WashingMachine, // "Washing Machine"
    kChangeDemo_SmartHome,      // "Smart Home"
};

const int16_t actions_elevator_en[] = {
    kElevator_FirstFloor,    // "First floor"
    kElevator_SecondFloor,   // "Second floor"
    kElevator_SecondFloor,   // "Second floor ^1"
    kElevator_ThirdFloor,    // "Third floor"
    kElevator_FourthFloor,   // "Fourth floor"
    kElevator_FifthFloor,    // "Fifth floor"
    kElevator_FifthFloor,    // "Fifth floor ^1"
    kElevator_MainLobby,     // "Main lobby"
    kElevator_GroundFloor,   // "Ground floor"
    kElevator_BasementFloor, // "Basement floor"
    kElevator_OpenDoor,      // "Open door"
    kElevator_CloseDoor,     // "Close door"
    kElevator_CloseDoor,     // "Close door ^1"
    kElevator_ChangeDemo     // "Change demo"
};

const int16_t actions_washing_machine_en[] = {
    kWashingMachine_Delicate,  // "Delicate"
    kWashingMachine_Normal,    // "Normal"
    kWashingMachine_HeavyDuty, // "Heavy duty"
    kWashingMachine_HeavyDuty, // "Heavy duty ^1"
    kWashingMachine_Whites,    // "Whites",
    kWashingMachine_Whites,    // "Whites ^1"
    kWashingMachine_Start,     // "Start"
    kWashingMachine_Cancel,    // "Cancel"
    kWashingMachine_ChangeDemo // "Change demo"
};

const int16_t actions_smart_home_en[] = {
    kSmartHome_TurnOnTheLights,   // "Turn on the lights"
    kSmartHome_TurnOnTheLights,   // "Turn on the lights ^1"
    kSmartHome_TurnOnTheLights,   // "Turn on the lights ^2"
    kSmartHome_TurnOnTheLights,   // "Turn on the lights ^3"
    kSmartHome_TurnOnTheLights,   // "Turn on the lights ^4"
    kSmartHome_TurnOnTheLights,   // "Turn on the lights ^5"
    kSmartHome_TurnOffTheLights,  // "Turn off the lights"
    kSmartHome_TurnOffTheLights,  // "Turn off the lights ^1"
    kSmartHome_TurnOffTheLights,  // "Turn off the lights ^2"
    kSmartHome_TemperatureHigher, // "Temperature higher"
    kSmartHome_TemperatureHigher, // "Temperature higher ^1"
    kSmartHome_TemperatureLower,  // "Temperature lower"
    kSmartHome_TemperatureLower,  // "Temperature lower ^1"
    kSmartHome_OpenTheWindow,     // "Open the window"
    kSmartHome_OpenTheWindow,     // "Open the window ^1"
    kSmartHome_OpenTheWindow,     // "Open the window ^2"
    kSmartHome_CloseTheWindow,    // "Close the window"
    kSmartHome_CloseTheWindow,    // "Close the window ^1"
    kSmartHome_CloseTheWindow,    // "Close the window ^2"
    kSmartHome_CloseTheWindow,    // "Close the window ^3"
    kSmartHome_CloseTheWindow,    // "Close the window ^4"
    kSmartHome_CloseTheWindow,    // "Close the window ^5"
    kSmartHome_MakeItBrighter,    // "Make it brighter"
    kSmartHome_MakeItDarker,      // "Make it darker"
    kSmartHome_ChangeDemo         // "Change demo"
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_EN_EN_STRINGS_TO_ACTIONS_DSMT_H_ */
