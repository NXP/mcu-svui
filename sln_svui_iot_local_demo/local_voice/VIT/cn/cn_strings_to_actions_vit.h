/*
 * Copyright 2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef VIT_CN_CN_STRINGS_TO_ACTIONS_VIT_H_
#define VIT_CN_CN_STRINGS_TO_ACTIONS_VIT_H_

#if ENABLE_VIT_ASR

#include "demo_actions_vit.h"
#include "stdint.h"

const int16_t actions_ww_cn[] = {
    kWakeWord_Detected, // "你好， 恩智浦"
};

const int16_t actions_elevator_cn[] = {
    kElevator_FirstFloor,     // "到一楼"
    kElevator_SecondFloor,    // "到二楼"
    kElevator_ThirdFloor,     // "到三楼"
    kElevator_FourthFloor,    // "到四楼"
    kElevator_FifthFloor,     // "到五楼"
    kElevator_MainLobby,      // "大堂"
    kElevator_GroundFloor,    // "负一楼"
    kElevator_BasementFloor,  // "地下室"
    kElevator_OpenDoor,       // "开门"
    kElevator_CloseDoor,      // "关门"
    kElevator_ChangeDemo,     // "选择应用"
    kElevator_ChangeLanguage, // "选拉语舀"
    kElevator_Elevator,       // "电梯"
    kElevator_WashingMachine, // "洗衣机"
    kElevator_SmartHome,      // "智能家居"
    kElevator_English,        // "英语"
    kElevator_French,         // "法语"
    kElevator_German,         // "德语"
    kElevator_Chinese,        // "中文"
};

const int16_t actions_washing_machine_cn[] = {
    kWashingMachine_Delicate,       // "精致模式"
    kWashingMachine_Normal,         // "正常模式"
    kWashingMachine_HeavyDuty,      // "强力模式"
    kWashingMachine_Whites,         // "洗白模式"
    kWashingMachine_Start,          // "开始"
    kWashingMachine_Cancel,         // "取消"
    kWashingMachine_ChangeDemo,     // "选择应用"
    kWashingMachine_ChangeLanguage, // "选拉语舀"
    kWashingMachine_Elevator,       // "电梯"
    kWashingMachine_WashingMachine, // "洗衣机"
    kWashingMachine_SmartHome,      // "智能家居"
    kWashingMachine_English,        // "英语"
    kWashingMachine_French,         // "法语"
    kWashingMachine_German,         // "德语"
    kWashingMachine_Chinese,        // "中文"
};

const int16_t actions_smart_home_cn[] = {
    kSmartHome_TurnOnTheLights,   // "打开灯"
    kSmartHome_TurnOffTheLights,  // "关闭灯"
    kSmartHome_TemperatureHigher, // "升高温度"
    kSmartHome_TemperatureLower,  // "降低温度"
    kSmartHome_OpenTheWindow,     // "打开窗帘"
    kSmartHome_CloseTheWindow,    // "关闭窗帘"
    kSmartHome_MakeItBrighter,    // "亮一点"
    kSmartHome_MakeItDarker,      // "暗一点"
    kSmartHome_ChangeDemo,        // "选择应用"
    kSmartHome_ChangeLanguage,    // "选拉语舀"
    kSmartHome_Elevator,          // "电梯"
    kSmartHome_WashingMachine,    // "洗衣机"
    kSmartHome_SmartHome,         // "智能家居"
    kSmartHome_English,           // "英语"
    kSmartHome_French,            // "法语"
    kSmartHome_German,            // "德语"
    kSmartHome_Chinese,           // "中文"
};

#endif /* ENABLE_VIT_ASR */
#endif /* VIT_CN_CN_STRINGS_TO_ACTIONS_VIT_H_ */
