/*
 * Copyright 2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "stdint.h"
#include "audio_samples.h"
#include "fsl_common.h"
#include "FreeRTOSConfig.h"
#include "sln_amplifier.h"
#include "sln_local_voice_common.h"
#include "sln_rgb_led_driver.h"
#include "local_sounds_task.h"
#include "sln_flash_files.h"

#include "IndexCommands.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

extern app_asr_shell_commands_t appAsrShellCommands;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

static void APP_LAYER_LedCommandDetected(void)
{
    RGB_LED_SetColor(LED_COLOR_OFF);
    RGB_LED_SetColor(LED_COLOR_GREEN);
    vTaskDelay(200);
    RGB_LED_SetColor(LED_COLOR_OFF);
}

static void APP_LAYER_LedListening(void)
{
    RGB_LED_SetColor(LED_COLOR_OFF);
    RGB_LED_SetColor(LED_COLOR_BLUE);
}

static void APP_LAYER_LedTimeout(void)
{
    RGB_LED_SetColor(LED_COLOR_OFF);
    RGB_LED_SetColor(LED_COLOR_PURPLE);
    vTaskDelay(200);
    RGB_LED_SetColor(LED_COLOR_OFF);
}

static void APP_LAYER_LedError(void)
{
    RGB_LED_SetColor(LED_COLOR_OFF);
    RGB_LED_SetColor(LED_COLOR_RED);
}

#if ENABLE_STREAMER
static status_t APP_LAYER_PlayAudioFromFileSystem(char *filename)
{
    status_t status = kStatus_Success;

    /* Make sure that speaker is not currently playing another audio. */
    while (LOCAL_SOUNDS_isPlaying())
    {
        vTaskDelay(10);
    }

    LOCAL_SOUNDS_PlayAudioFile(filename, appAsrShellCommands.volume);

    return status;
}
#endif /* ENABLE_STREAMER */

/*******************************************************************************
 * API
 ******************************************************************************/

__attribute__ ((weak)) status_t APP_LAYER_ProcessWakeWord(oob_demo_control_t *commandConfig)
{
#if ENABLE_STREAMER
    APP_LAYER_PlayAudioFromFileSystem(AUDIO_WW_DETECTED);
#endif /* ENABLE_STREAMER */

    APP_LAYER_LedListening();

    return kStatus_Success;
}

__attribute__ ((weak)) status_t APP_LAYER_ProcessVoiceCommand(oob_demo_control_t *commandConfig)
{
    status_t status = kStatus_Success;

#if ENABLE_STREAMER
    char *prompt = NULL;
    if (commandConfig != NULL)
    {
        /* Play prompt for the active language */
        prompt = get_prompt_from_keyword(commandConfig->language, commandConfig->commandSet, commandConfig->commandId);

        if (NULL != prompt)
        {
            APP_LAYER_PlayAudioFromFileSystem(prompt);
        }
    }
#endif /* ENABLE_STREAMER */

    if (status == kStatus_Success)
    {
        APP_LAYER_LedCommandDetected();
    }
    else
    {
        APP_LAYER_LedError();
    }

    return status;
}

__attribute__ ((weak)) status_t APP_LAYER_ProcessTimeout(oob_demo_control_t *commandConfig)
{
#if ENABLE_STREAMER
    APP_LAYER_PlayAudioFromFileSystem(AUDIO_TONE_TIMEOUT);
#endif /* ENABLE_STREAMER */

    APP_LAYER_LedTimeout();

    return kStatus_Success;
}

__attribute__ ((weak)) void APP_LAYER_SwitchToNextDemo(void)
{
    configPRINTF(("Switch to next demo from buttons not implemented\r\n"));
}

#if ENABLE_VIT_ASR
__attribute__ ((weak)) bool APP_LAYER_FilterVitDetection(unsigned short commandId, asr_inference_t activeDemo)
{
    bool filterDetection = false;

    /* Add here any filtering logic */

    return filterDetection;
}

__attribute__ ((weak)) void APP_LAYER_HandleFirstBoardBoot(void)
{
    if ((appAsrShellCommands.demo == BOOT_ASR_CMD_DEMO) && (BOOT_ASR_CMD_DEMO && DEFAULT_ASR_CMD_DEMO))
    {
        configPRINTF(("First board boot detected\r\n"));
        /* Insert any first board boot logic here */
    }
}
#endif /* ENABLE_VIT_ASR */
