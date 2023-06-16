/*
 * Copyright 2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef VIT_CN_CN_STRINGS_TO_PROMPTS_VIT_H_
#define VIT_CN_CN_STRINGS_TO_PROMPTS_VIT_H_

#if ENABLE_VIT_ASR

#include "sln_flash_files.h"

const char * const prompts_ww_cn[] = {
    AUDIO_WW_DETECTED, // "你好， 恩智浦"
};

const char * const prompts_elevator_cn[] = {
    AUDIO_FIRST_FLOOR_CN,    // "到一楼"
    AUDIO_SECOND_FLOOR_CN,   // "到二楼"
    AUDIO_THIRD_FLOOR_CN,    // "到三楼"
    AUDIO_FOURTH_FLOOR_CN,   // "到四楼"
    AUDIO_FIFTH_FLOOR_CN,    // "到五楼"
    AUDIO_MAIN_LOBBY_CN,     // "大堂"
    AUDIO_GROUND_FLOOR_CN,   // "负一楼"
    AUDIO_BASEMENT_FLOOR_CN, // "地下室"
    AUDIO_OPEN_DOOR_CN,      // "开门"
    AUDIO_CLOSE_DOOR_CN,     // "关门"
    AUDIO_DEMO_NAME_TEST_CN, // "选择应用"
    AUDIO_LANGUAGE_TEST_CN,  // "选拉语舀"
    AUDIO_OK_CN,             // "电梯"
    AUDIO_OK_CN,             // "洗衣机"
    AUDIO_OK_CN,             // "智能家居"
    AUDIO_OK_CN,             // "英语"
    AUDIO_OK_CN,             // "法语"
    AUDIO_OK_CN,             // "德语"
    AUDIO_OK_CN,             // "中文"
};

const char * const prompts_washing_machine_cn[] = {
    AUDIO_DELICATE_CN,       // "精致模式"
    AUDIO_NORMAL_CN,         // "正常模式"
    AUDIO_HEAVY_DUTY_CN,     // "强力模式"
    AUDIO_WHITES_CN,         // "洗白模式"
    AUDIO_START_CN,          // "开始"
    AUDIO_CANCEL_CN,         // "取消"
    AUDIO_DEMO_NAME_TEST_CN, // "选择应用"
    AUDIO_LANGUAGE_TEST_CN,  // "选拉语舀"
    AUDIO_OK_CN,             // "电梯"
    AUDIO_OK_CN,             // "洗衣机"
    AUDIO_OK_CN,             // "智能家居"
    AUDIO_OK_CN,             // "英语"
    AUDIO_OK_CN,             // "法语"
    AUDIO_OK_CN,             // "德语"
    AUDIO_OK_CN,             // "中文"
};

const char * const prompts_smart_home_cn[] = {
    AUDIO_TURN_ON_THE_LIGHTS_CN,  // "打开灯"
    AUDIO_TURN_OFF_THE_LIGHTS_CN, // "关闭灯"
    AUDIO_TEMPERATURE_HIGHER_CN,  // "升高温度"
    AUDIO_TEMPERATURE_LOWER_CN,   // "降低温度"
    AUDIO_OPEN_THE_WINDOW_CN,     // "打开窗帘"
    AUDIO_CLOSE_THE_WINDOW_CN,    // "关闭窗帘"
    AUDIO_MAKE_IT_BRIGHTER_CN,    // "亮一点"
    AUDIO_MAKE_IT_DARKER_CN,      // "暗一点"
    AUDIO_DEMO_NAME_TEST_CN,      // "选择应用"
    AUDIO_LANGUAGE_TEST_CN,       // "选拉语舀"
    AUDIO_OK_CN,                  // "电梯"
    AUDIO_OK_CN,                  // "洗衣机"
    AUDIO_OK_CN,                  // "智能家居"
    AUDIO_OK_CN,                  // "英语"
    AUDIO_OK_CN,                  // "法语"
    AUDIO_OK_CN,                  // "德语"
    AUDIO_OK_CN,                  // "中文"
};

#endif /* ENABLE_VIT_ASR */
#endif /* VIT_CN_CN_STRINGS_TO_PROMPTS_VIT_H_ */
