/*
 * Copyright 2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef VIT_DEMO_ACTIONS_VIT_H_
#define VIT_DEMO_ACTIONS_VIT_H_

#if ENABLE_VIT_ASR

/* Global actions for all languages */

enum _wake_word_action
{
    kWakeWord_Detected = 0,
    kWakeWord_ActionInvalid
};

enum _elevator_action
{
    kElevator_FirstFloor = 0,
    kElevator_SecondFloor,
    kElevator_ThirdFloor,
    kElevator_FourthFloor,
    kElevator_FifthFloor,
    kElevator_MainLobby,
    kElevator_GroundFloor,
    kElevator_BasementFloor,
    kElevator_OpenDoor,
    kElevator_CloseDoor,
    kElevator_ChangeDemo,
    kElevator_ChangeLanguage,
    kElevator_Elevator,
    kElevator_WashingMachine,
    kElevator_SmartHome,
    kElevator_English,
    kElevator_French,
    kElevator_German,
    kElevator_Chinese,
    kElevator_ActionInvalid
};

enum _washing_machine_action
{
    kWashingMachine_Delicate = 0,
    kWashingMachine_Normal,
    kWashingMachine_HeavyDuty,
    kWashingMachine_Whites,
    kWashingMachine_Start,
    kWashingMachine_Cancel,
    kWashingMachine_ChangeDemo,
    kWashingMachine_ChangeLanguage,
    kWashingMachine_Elevator,
    kWashingMachine_WashingMachine,
    kWashingMachine_SmartHome,
    kWashingMachine_English,
    kWashingMachine_French,
    kWashingMachine_German,
    kWashingMachine_Chinese,
    kWashingMachine_ActionInvalid
};

enum _smart_home_action
{
    kSmartHome_TurnOnTheLights = 0,
    kSmartHome_TurnOffTheLights,
    kSmartHome_TemperatureHigher,
    kSmartHome_TemperatureLower,
    kSmartHome_OpenTheWindow,
    kSmartHome_CloseTheWindow,
    kSmartHome_MakeItBrighter,
    kSmartHome_MakeItDarker,
    kSmartHome_ChangeDemo,
    kSmartHome_ChangeLanguage,
    kSmartHome_Elevator,
    kSmartHome_WashingMachine,
    kSmartHome_SmartHome,
    kSmartHome_English,
    kSmartHome_French,
    kSmartHome_German,
    kSmartHome_Chinese,
    kSmartHome_ActionInvalid
};

#endif /* ENABLE_VIT_ASR */
#endif /* VIT_DEMO_ACTIONS_VIT_H_ */
