/*
 * Copyright 2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef DSMT_CN_CN_STRINGS_DSMT_H_
#define DSMT_CN_CN_STRINGS_DSMT_H_

#if ENABLE_DSMT_ASR

/* Chinese demos strings below.
 * These must match the DSMTv2 groups */

const char * const ww_cn[] = {
        "你好， 恩智浦",
        "你好， 恩智浦 ^1",
        "你好， 恩智浦 ^2",
        "你好， 恩智浦 ^3",
};

const char * const cmd_change_demo_cn[] = {
        "电梯",
        "洗衣机",
        "智能家居",
};

const char * const cmd_elevator_cn[] = {
        "到一楼",
        "到二楼",
        "到三楼",
        "到四楼",
        "到五楼",
        "大堂",
        "负一楼",
        "地下室",
        "开门",
        "关门",
        "选择应用",
};

const char * const cmd_washing_machine_cn[] = {
        "精致模式",
        "正常模式",
        "强力模式",
        "洗白模式",
        "洗白模式 ^1",
        "开始",
        "取消",
        "选择应用",
};

const char * const cmd_smart_home_cn[] = {
        "打开灯",
        "关闭灯",
        "升高温度",
        "降低温度",
        "打开窗帘",
        "关闭窗帘",
        "亮一点",
        "暗一点",
        "选择应用",
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_CN_CN_STRINGS_DSMT_H_ */
