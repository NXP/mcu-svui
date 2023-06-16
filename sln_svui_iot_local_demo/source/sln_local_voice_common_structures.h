/*
 * Copyright 2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef SLN_LOCAL_VOICE_COMMON_STRUCTURES_H_
#define SLN_LOCAL_VOICE_COMMON_STRUCTURES_H_

typedef enum _audio_processing_states
{
    kWakeWordDetected = 0,
    kVoiceCommandDetected,
    kMicUpdate,
    kVolumeUpdate,
    kTimeOut,
    kAsrModelChanged,
    kDefault
} audio_processing_states_t;

typedef struct _oob_demo_control
{
    uint16_t language;
    uint16_t commandSet;
    uint8_t commandId;
} oob_demo_control_t;

typedef enum _app_flash_status
{
    READ_SUCCESS  = (1U << 0U),
    READ_FAIL     = (1U << 1U),
    READ_READY    = (1U << 2U),
    WRITE_SUCCESS = (1U << 3U),
    WRITE_FAIL    = (1U << 4U),
    WRITE_READY   = (1U << 5U),
} app_flash_status_t;

typedef enum _asr_mute
{
    ASR_MUTE_OFF = 0,
    ASR_MUTE_ON,
} asr_mute_t;

typedef enum _asr_followup
{
    ASR_FOLLOWUP_OFF = 0,
    ASR_FOLLOWUP_ON,
} asr_followup_t;

typedef enum _asr_ptt
{
    ASR_PTT_OFF = 0,
    ASR_PTT_ON,
} asr_ptt_t;

typedef enum _asr_cmd_res
{
    ASR_CMD_RES_OFF = 0,
    ASR_CMD_RES_ON,
} asr_cmd_res_t;

typedef enum _asr_cfg_demo
{
    ASR_CFG_DEMO_NO_CHANGE = (1U << 0U),               // OOB demo type or languages unchanged
    ASR_CFG_CMD_INFERENCE_ENGINE_CHANGED = (1U << 1U), // OOB demo type changed
    ASR_CFG_DEMO_LANGUAGE_CHANGED = (1U << 2U),        // OOB language type changed
} asr_cfg_demo_t;

typedef struct _app_asr_shell_commands
{
    app_flash_status_t status;
    asr_cfg_demo_t asrCfg;
    uint32_t volume; // 0 ~ 100
    asr_mute_t mute;
    uint32_t timeout; // in millisecond
    asr_followup_t followup;
    uint16_t demo;        // demo types: elevator, washing machine, smart home
    uint16_t activeLanguage; // runtime language types
    asr_ptt_t ptt;
    asr_cmd_res_t cmdresults;
    uint8_t skipWW; // Set to 1 to skip Wake Word phase and go directly to Voice Command phase. Will self-clear automatically.
    uint8_t changeDemoFlow;
    uint8_t changeLanguageFlow;
} app_asr_shell_commands_t;

#endif /* SLN_LOCAL_VOICE_COMMON_STRUCTURES_H_ */
