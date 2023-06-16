/*
 * Copyright 2019-2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#include <time.h>

/* Board includes */
#include "board.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"

/* FreeRTOS kernel includes */
#include "FreeRTOS.h"
#include "task.h"

/* Driver includes */
#include "fsl_dmamux.h"
#include "fsl_edma.h"
#include "fsl_iomuxc.h"

/* RGB LED driver header */
#include "sln_rgb_led_driver.h"

/* Shell includes */
#include "sln_shell.h"

/* Application headers */
#include "sln_local_voice_common.h"
#include "switch.h"

/* Flash includes */
#include "sln_flash.h"
#include "sln_flash_fs.h"
#include "sln_flash_fs_ops.h"
#include "sln_flash_files.h"

/* Audio processing includes */
#include "audio_samples.h"
#include "audio_processing_task.h"
#include "pdm_to_pcm_task.h"
#include "sln_amplifier.h"
#if ENABLE_AUDIO_DUMP
#include "audio_dump.h"
#endif /* ENABLE_AUDIO_DUMP */

#include "sln_mic_config.h"
#if (MICS_TYPE == MICS_PDM)
#elif (MICS_TYPE == MICS_I2S)
#include "sln_i2s_mic.h"
#endif /* MICS_TYPE */

#include "local_sounds_task.h"
#include "IndexCommands.h"
#include "app_layer.h"
#include "clock_config.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define audio_processing_task_PRIORITY (configMAX_PRIORITIES - 1)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
TaskHandle_t appTaskHandle             = NULL;
TaskHandle_t audioProcessingTaskHandle = NULL;
TaskHandle_t micTaskHandle             = NULL;
TaskHandle_t appInitDummyNullHandle    = NULL;
#if ENABLE_AUDIO_DUMP
TaskHandle_t aecAlignSoundTaskHandle   = NULL;
#endif /* ENABLE_AUDIO_DUMP */
bool taskPlaying;
uint8_t isRecording;
bool g_SW1Pressed                   = false;
oob_demo_control_t oob_demo_control = {0};

extern app_asr_shell_commands_t appAsrShellCommands;

/*******************************************************************************
 * Callbacks
 ******************************************************************************/
void switch_callback(int32_t button_nr, int32_t state)
{
    if (button_nr == SWITCH_SW1)
    {
        if (state == SWITCH_PRESSED)
        {
            g_SW1Pressed = true;
        }
        else
        {
            g_SW1Pressed = false;
        }
    }

    if (button_nr == SWITCH_SW2)
    {
        if (state == SWITCH_PRESSED)
        {
            APP_LAYER_SwitchToNextDemo();
        }
    }
}

/*******************************************************************************
 * Code
 ******************************************************************************/

#if ENABLE_STREAMER
static status_t audio_play_clip(char *fileName, uint32_t volume)
{
    status_t status = kStatus_Success;

    /* Make sure that speaker is not currently playing another audio. */
    while (LOCAL_SOUNDS_isPlaying())
    {
        vTaskDelay(10);
    }

    status = LOCAL_SOUNDS_PlayAudioFile(fileName, volume);

    return status;
}

static status_t announce_demo(asr_inference_t demo, asr_language_t lang, uint32_t volume)
{
    status_t ret = kStatus_Success;

    char *prompt = get_demo_prompt(demo, lang);
    ret = audio_play_clip(prompt, volume);

    return ret;
}
#endif /* ENABLE_STREAMER */

void appTask(void *arg)
{
    status_t ret             = kStatus_Success;
    uint32_t statusFlash     = 0;
    mic_task_config_t config = {0};

    (void)ret;

    sln_shell_set_app_init_task_handle(&appInitDummyNullHandle);

#if ENABLE_STREAMER
    ret = LOCAL_SOUNDS_InitStreamer();
    if (ret != kStatus_Success)
    {
        PRINTF("LOCAL_SOUNDS_InitStreamer failed!\r\n");
    }
#endif /* ENABLE_STREAMER */

    int16_t *micBuf = SLN_MIC_GET_PCM_BUFFER_POINTER();
    audio_processing_set_mic_input_buffer(micBuf);

    int16_t *ampBuf = SLN_MIC_GET_AMP_BUFFER_POINTER();
    audio_processing_set_amp_input_buffer(ampBuf);


    /* Create audio processing task */
    if (xTaskCreate(audio_processing_task, "Audio_processing_task", 1536U, NULL, audio_processing_task_PRIORITY,
                    &audioProcessingTaskHandle) != pdPASS)
    {
        PRINTF("Audio processing task creation failed!\r\n");
        RGB_LED_SetColor(LED_COLOR_RED);
        vTaskDelete(NULL);
    }

    /* Configure Microphones */
    config.thisTask       = &micTaskHandle;
    config.processingTask = &audioProcessingTaskHandle;
#if !AMP_LOOPBACK_DISABLED
    config.feedbackEnable  = SLN_AMP_LoopbackEnable;
    config.feedbackDisable = SLN_AMP_LoopbackDisable;
#if USE_MQS
    config.loopbackRingBuffer = SLN_AMP_GetRingBuffer();
    config.loopbackMutex      = SLN_AMP_GetLoopBackMutex();
    config.updateTimestamp    = SLN_AMP_UpdateTimestamp;
    config.getTimestamp       = SLN_AMP_GetTimestamp;
#endif /* USE_MQS */
#endif /* !AMP_LOOPBACK_DISABLED */

    SLN_MIC_SET_TASK_CONFIG(&config);

    /* Create microphones data acquisition task */
    if (xTaskCreate(SLN_MIC_TASK_FUNCTION, SLN_MIC_TASK_NAME, SLN_MIC_TASK_STACK_SIZE, NULL, SLN_MIC_TASK_PRIORITY, &micTaskHandle) !=
        pdPASS)
    {
        PRINTF("PDM to PCM processing task creation failed!\r\n");
        RGB_LED_SetColor(LED_COLOR_RED);
        vTaskDelete(NULL);
    }

#if ENABLE_AUDIO_DUMP
    if (xTaskCreate(AUDIO_DUMP_AecAlignSoundTask, aec_align_sound_task_NAME, aec_align_sound_task_STACK, NULL, aec_align_sound_task_PRIORITY,
                        &aecAlignSoundTaskHandle) != pdPASS)
    {
        PRINTF("xTaskCreate AUDIO_DUMP_AecAlignSoundTask failed!\r\n");
    }
#endif /* ENABLE_AUDIO_DUMP */

    while (!SLN_MIC_GET_STATE())
    {
        vTaskDelay(1);
    }

    RGB_LED_SetColor(LED_COLOR_OFF);

    SLN_AMP_SetVolume(appAsrShellCommands.volume);

#if ENABLE_STREAMER
    announce_demo(appAsrShellCommands.demo, appAsrShellCommands.activeLanguage, appAsrShellCommands.volume);
#endif /* ENABLE_STREAMER */

    APP_LAYER_HandleFirstBoardBoot();

    if (appAsrShellCommands.mute == ASR_MUTE_ON)
    {
        /* close mics and amp feedback loop */
        SLN_MIC_OFF();
        /* show the user that device does not respond */
        RGB_LED_SetColor(LED_COLOR_ORANGE);
    }
    else if (appAsrShellCommands.ptt == ASR_PTT_ON)
    {
        configPRINTF(
            ("ASR Push-To-Talk mode is enabled.\r\n"
             "Press SW3 to input a command.\r\n"));
        /* show the user that device will wake up only on SW3 press */
        RGB_LED_SetColor(LED_COLOR_CYAN);
    }

    /* Initialize the buttons */
    SWITCH_Init();
    SWITCH_RegisterCallback(switch_callback);

    uint32_t taskNotification = 0;
    while (1)
    {
        xTaskNotifyWait(0xffffffffU, 0xffffffffU, &taskNotification, portMAX_DELAY);

        switch (taskNotification)
        {
            case kMicUpdate:
            {
                if (appAsrShellCommands.mute == ASR_MUTE_ON)
                {
                    /* close mics and amp feedback loop */
                    SLN_MIC_OFF();
                    /* show the user that device does not respond */
                    RGB_LED_SetColor(LED_COLOR_ORANGE);
                }
                else
                {
                    /* reopen mics and amp feedback loop */
                    SLN_MIC_ON();
                    /* turn off the orange led */
                    RGB_LED_SetColor(LED_COLOR_OFF);
                }
                break;
            }
            case kVolumeUpdate:
            {
                SLN_AMP_SetVolume(appAsrShellCommands.volume);
                break;
            }
            case kWakeWordDetected:
            {
                APP_LAYER_ProcessWakeWord(&oob_demo_control);
                break;
            }

            case kVoiceCommandDetected:
            {
                APP_LAYER_ProcessVoiceCommand(&oob_demo_control);
                break;
            }

            case kTimeOut:
                APP_LAYER_ProcessTimeout(&oob_demo_control);
                break;

            case kAsrModelChanged:
#if ENABLE_STREAMER
                announce_demo(appAsrShellCommands.demo, oob_demo_control.language, appAsrShellCommands.volume);
#endif /* ENABLE_STREAMER */

                if((appAsrShellCommands.ptt != ASR_PTT_ON) &&
                   (appAsrShellCommands.mute != ASR_MUTE_ON))
                {
                    RGB_LED_SetColor(LED_COLOR_OFF);
                }
                break;

            default:
                break;
        }

        if (appAsrShellCommands.ptt == ASR_PTT_ON)
        {
            /* show the user that device will wake up only on SW3 press */
            RGB_LED_SetColor(LED_COLOR_CYAN);
        }

        /* check the status of shell commands and flash file system */
        if (appAsrShellCommands.status == WRITE_READY)
        {
            appAsrShellCommands.status = WRITE_SUCCESS;
            statusFlash = sln_flash_fs_ops_save(ASR_SHELL_COMMANDS_FILE_NAME, (uint8_t *)&appAsrShellCommands,
                                              sizeof(app_asr_shell_commands_t));
            if (statusFlash != SLN_FLASH_FS_OK)
            {
                configPRINTF(("Failed to write in flash memory.\r\n"));
            }
            else
            {
                configPRINTF(("Updated Shell command parameter in flash memory.\r\n"));
            }
        }

        taskNotification = 0;
    }
}

/*!
 * @brief Main function
 */
void main(void)
{
    /* Enable additional fault handlers */
    SCB->SHCSR |= (SCB_SHCSR_BUSFAULTENA_Msk | /*SCB_SHCSR_USGFAULTENA_Msk |*/ SCB_SHCSR_MEMFAULTENA_Msk);

    /* Init board hardware */
    /* Relocate Vector Table */
#if RELOCATE_VECTOR_TABLE
    BOARD_RelocateVectorTableToRam();
#endif

    BOARD_ConfigMPU();
    BOARD_InitBootPins();
    BOARD_BootClockRUN();

    SLN_Flash_Init();

    /*
     * AUDIO PLL setting: Frequency = Fref * (DIV_SELECT + NUM / DENOM)
     *                              = 24 * (32 + 77/100)
     *                              = 786.48 MHz
     */
    const clock_audio_pll_config_t audioPllConfig = {
        .loopDivider = 32,  /* PLL loop divider. Valid range for DIV_SELECT divider value: 27~54. */
        .postDivider = 1,   /* Divider after the PLL, should only be 1, 2, 4, 8, 16. */
        .numerator   = 77,  /* 30 bit numerator of fractional loop divider. */
        .denominator = 100, /* 30 bit denominator of fractional loop divider */
    };

    CLOCK_InitAudioPll(&audioPllConfig);

    CLOCK_SetMux(kCLOCK_Sai1Mux, BOARD_PDM_SAI_CLOCK_SOURCE_SELECT);
    CLOCK_SetDiv(kCLOCK_Sai1PreDiv, BOARD_PDM_SAI_CLOCK_SOURCE_PRE_DIVIDER);
    CLOCK_SetDiv(kCLOCK_Sai1Div, BOARD_PDM_SAI_CLOCK_SOURCE_DIVIDER);
    CLOCK_EnableClock(kCLOCK_Sai1);

    CLOCK_SetMux(kCLOCK_Sai2Mux, BOARD_PDM_SAI_CLOCK_SOURCE_SELECT);
    CLOCK_SetDiv(kCLOCK_Sai2PreDiv, BOARD_PDM_SAI_CLOCK_SOURCE_PRE_DIVIDER);
    CLOCK_SetDiv(kCLOCK_Sai2Div, BOARD_PDM_SAI_CLOCK_SOURCE_DIVIDER);
    CLOCK_EnableClock(kCLOCK_Sai2);

    edma_config_t dmaConfig = {0};

    EDMA_GetDefaultConfig(&dmaConfig);
    EDMA_Init(DMA0, &dmaConfig);

    DMAMUX_Init(DMAMUX);

    RGB_LED_Init();
    RGB_LED_SetColor(LED_COLOR_GREEN);

    sln_shell_init();

    /* Init littlefs and set pre and post sector erase callbacks */
    sln_flash_fs_status_t statusFlash = sln_flash_fs_ops_init(false);
    if (SLN_FLASH_FS_OK != statusFlash)
    {
        configPRINTF(("littlefs init failed!\r\n"));
    }

    sln_flash_fs_cbs_t flash_mgmt_cbs = {NULL, NULL, SLN_MIC_OFF, SLN_MIC_ON};

    statusFlash = sln_flash_fs_ops_setcbs(&flash_mgmt_cbs);
    if (SLN_FLASH_FS_OK != statusFlash)
    {
        configPRINTF(("littlefs callbacks setting failed!\r\n"));
    }

    xTaskCreate(appTask, "APP_Task", 512, NULL, configMAX_PRIORITIES - 4, &appTaskHandle);
    xTaskCreate(sln_shell_task, "Shell_Task", 1024, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(local_voice_task, "Local_Voice_Task", 4096, NULL, configMAX_PRIORITIES - 4, NULL);

    /* Run RTOS */
    vTaskStartScheduler();

    /* Should not reach this statement */
    while (1)
        ;
}

