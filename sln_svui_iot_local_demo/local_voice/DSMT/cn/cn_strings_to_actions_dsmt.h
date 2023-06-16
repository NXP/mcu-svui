/*
 * Copyright 2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef DSMT_CN_CN_STRINGS_TO_ACTIONS_DSMT_H_
#define DSMT_CN_CN_STRINGS_TO_ACTIONS_DSMT_H_

#if ENABLE_DSMT_ASR

#include "demo_actions_dsmt.h"
#include "stdint.h"

const int16_t actions_ww_cn[] = {
    kWakeWord_Detected, // "你好， 恩智浦"
    kWakeWord_Detected, // "你好， 恩智浦 ^1"
    kWakeWord_Detected, // "你好， 恩智浦 ^2"
    kWakeWord_Detected, // "你好， 恩智浦 ^3"
};

const int16_t actions_change_demo_cn[] = {
    kChangeDemo_Elevator,       // "电梯"
    kChangeDemo_WashingMachine, // "洗衣机"
    kChangeDemo_SmartHome       // "智能家居"
};

const int16_t actions_elevator_cn[] = {
    kElevator_FirstFloor,    // "到一楼"
    kElevator_SecondFloor,   // "到二楼"
    kElevator_ThirdFloor,    // "到三楼"
    kElevator_FourthFloor,   // "到四楼"
    kElevator_FifthFloor,    // "到五楼"
    kElevator_MainLobby,     // "大堂"
    kElevator_GroundFloor,   // "负一楼"
    kElevator_BasementFloor, // "地下室"
    kElevator_OpenDoor,      // "开门"
    kElevator_CloseDoor,     // "关门"
    kElevator_ChangeDemo,    // "选择应用"
};

const int16_t actions_washing_machine_cn[] = {
    kWashingMachine_Delicate,   // "精致模式"
    kWashingMachine_Normal,     // "正常模式"
    kWashingMachine_HeavyDuty,  // "强力模式"
    kWashingMachine_Whites,     // "洗白模式"
    kWashingMachine_Whites,     // "洗白模式 ^1"
    kWashingMachine_Start,      // "开始"
    kWashingMachine_Cancel,     // "取消"
    kWashingMachine_ChangeDemo, // "选择应用"
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
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_CN_CN_STRINGS_TO_ACTIONS_DSMT_H_ */
