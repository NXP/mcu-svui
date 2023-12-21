/*
 * Copyright 2018-2023 NXP.
 * NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly
 * in accordance with the applicable license terms. By expressly accepting such terms or by downloading,
 * installing, activating and/or otherwise using the software, you are agreeing that you have read, and
 * that you agree to comply with and are bound by, such license terms. If you do not agree to be bound by
 * the applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#include <stdbool.h>
#include <stdint.h>

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

/* NXP includes. */
#include "board.h"
#include "sln_mic_config.h"
#include "sln_afe.h"
#include "sln_amplifier.h"
#include "sln_rgb_led_driver.h"
#include "local_sounds_task.h"
#if ENABLE_AUDIO_DUMP
#include "audio_dump.h"
#endif /* ENABLE_AUDIO_DUMP */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define AFE_BLOCKS_TO_ACCUMULATE (3)

#if SLN_MIC_COUNT == 2
#define AFE_MEM_SIZE AFE_MEM_SIZE_2MICS
#elif SLN_MIC_COUNT == 3
#define AFE_MEM_SIZE AFE_MEM_SIZE_3MICS
#else
#error "UNSUPPORTED NUMBER OF MICROPHONES"
#endif /* SLN_MIC_COUNT */

#if VAD_ENABLED
/* After Voice Activity detected, assume Voice Activity for next VAD_FORCED_TRUE_CALLS */
/* The value below is for 10 minutes * 60 seconds * 100 frames of 10 ms, hence VAD will
 * activate after 10 minutes of continuous silence */
#define VAD_FORCED_TRUE_CALLS 10 * 60 * 100

/* Number of consecutive frames with activity detected before getting out of detection 'sleep' */
#define VAD_ACTIVITY_FRAMES    10
#endif /* VAD_ENABLED */

/*******************************************************************************
 * Variables
 ******************************************************************************/

SDK_ALIGN(static uint8_t __attribute__((section(".bss.$SRAM_DTC"))) s_afeExternalMemory[AFE_MEM_SIZE], 8);
SDK_ALIGN(static int16_t __attribute__((section(".bss.$SRAM_ITC")))
          s_outStream[PCM_SINGLE_CH_SMPL_COUNT * AFE_BLOCKS_TO_ACCUMULATE],
          8);

static uint8_t s_outBlocksCnt = 0;
QueueHandle_t g_xSampleQueue  = NULL;

static pcmPingPong_t *s_micInputStream = NULL;
static int16_t *s_ampInputStream       = NULL;

volatile uint32_t g_wakeWordLength  = 0;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static sln_afe_status_t _sln_afe_init(void);
static sln_afe_status_t _sln_afe_process_audio(int16_t *micStream, int16_t *ampStream, void **cleanStream);
static sln_afe_status_t _sln_afe_trigger_found(void);
#if VAD_ENABLED
static sln_afe_status_t _sln_afe_vad(int16_t *micStream, bool *voiceActivity);
#endif /* VAD_ENABLED */

/*******************************************************************************
 * Code
 ******************************************************************************/

void audio_processing_set_mic_input_buffer(int16_t *buf)
{
    s_micInputStream = (pcmPingPong_t *)buf;
}

void audio_processing_set_amp_input_buffer(int16_t *buf)
{
    s_ampInputStream = buf;
}

void audio_processing_task(void *pvParameters)
{
    uint8_t pingPongIdx       = 0;
    uint8_t pingPongAmpIdx    = 0;
    uint32_t taskNotification = 0U;
    uint32_t currentEvent     = 0U;

    int16_t *micStream = NULL;
    int16_t *ampStream = NULL;
    void *cleanStream  = NULL;

    sln_afe_status_t afeStatus;
    bool voiceActivity = false;
    bool prevVoiceActivity = false;

    uint32_t vadStartTicks       = 0;
    uint32_t vadEndTicks         = 0;
    uint32_t totalVadSessionsSec = 0;
    uint32_t totalBoardRunSec    = 0;
    uint32_t vadSessionSec       = 0;

    /* SLN_AFE Initialization. */
    afeStatus = _sln_afe_init();
    if (afeStatus != kAfeSuccess)
    {
        configPRINTF(("ERROR [%d]: AFE engine initialization has failed!\r\n", afeStatus));
        RGB_LED_SetColor(LED_COLOR_RED);
        vTaskDelete(NULL);
    }

    g_xSampleQueue = xQueueCreate(5, PCM_SINGLE_CH_SMPL_COUNT * AFE_BLOCKS_TO_ACCUMULATE * sizeof(short));
    if (g_xSampleQueue == NULL)
    {
        configPRINTF(("Could not create queue for AFE to ASR communication. Audio processing task failed!\r\n"));
        RGB_LED_SetColor(LED_COLOR_RED);
        vTaskDelete(NULL);
    }

    while (1)
    {
        /* Suspend waiting to be activated when receiving PDM mic data after Decimation. */
        xTaskNotifyWait(0U, 0xffffffffU, &taskNotification, portMAX_DELAY);

        /* Figure out if it's a PING or PONG buffer received */
        if (taskNotification & PCM_PING_EVENT)
        {
            pingPongIdx    = 0U;
            pingPongAmpIdx = 0U;
            currentEvent   = PCM_PING_EVENT;
        }

        if (taskNotification & PCM_PONG_EVENT)
        {
            pingPongIdx    = 1U;
            pingPongAmpIdx = 1U;
            currentEvent   = PCM_PONG_EVENT;
        }

        taskNotification &= ~currentEvent;

#if ENABLE_STREAMER
        /* Temporarily bypass audio processing while streaming audio. */
        if (LOCAL_SOUNDS_isPlaying())
        {
            continue;
        }
#endif /* ENABLE_STREAMER */

        micStream = (*s_micInputStream)[pingPongIdx];
        if (s_ampInputStream != NULL)
        {
            ampStream = &s_ampInputStream[pingPongAmpIdx * PCM_SINGLE_CH_SMPL_COUNT];
        }
        else
        {
            ampStream = NULL;
        }

        /* Use SLN_AFE on microphones and speaker data to obtain a clean stream. */
        afeStatus = _sln_afe_process_audio(micStream, ampStream, &cleanStream);
        if (afeStatus != kAfeSuccess)
        {
            configPRINTF(("ERROR [%d]: AFE audio process failed!\r\n", afeStatus));
            RGB_LED_SetColor(LED_COLOR_RED);
        }

#if ENABLE_AUDIO_DUMP
        AUDIO_DUMP_ForwardDataOverUsb(micStream, ampStream, cleanStream);
#endif /* ENABLE_AUDIO_DUMP */

#if VAD_ENABLED
        /* Use SLN_AFE on mic stream to detect Voice Activity and Gate ASR if needed. */
        afeStatus = _sln_afe_vad(micStream, &voiceActivity);
        if (afeStatus != kAfeSuccess)
        {
            configPRINTF(("ERROR [%d]: AFE audio VAD failed!\r\n", afeStatus));
            RGB_LED_SetColor(LED_COLOR_RED);
            voiceActivity = true;
        }

        if (prevVoiceActivity != voiceActivity)
        {
            if (voiceActivity == true)
            {
                vadEndTicks         = xTaskGetTickCount();
                vadSessionSec       = ((vadEndTicks - vadStartTicks) * (DEFAULT_SYSTEM_CLOCK / BOARD_REDUCEDCLOCK_CORE_CLOCK)) / configTICK_RATE_HZ;
                totalBoardRunSec    += vadSessionSec;
                totalVadSessionsSec += vadSessionSec;
                configPRINTF(("VAD: activity detected, detection enabled after %d sec, total bypassing since power on %d sec, timestamp %d sec\r\n",
                              vadSessionSec, totalVadSessionsSec, totalBoardRunSec));
            }
            else
            {
                vadStartTicks = xTaskGetTickCount();
                totalBoardRunSec += ((vadStartTicks - vadEndTicks) / configTICK_RATE_HZ);
                configPRINTF(("VAD: no activity in last %d sec, detection disabled, timestamp %d sec\r\n",
                              VAD_FORCED_TRUE_CALLS / 100, totalBoardRunSec));
            }

            prevVoiceActivity = voiceActivity;
        }
#else
        /* If VAD is disabled set voiceActivity flag to true */
        voiceActivity = true;
#endif /* VAD_ENABLED */

        if (voiceActivity == true)
        {

#if VAD_ENABLED
        /* Revert MCU frequency back to its default value when voice activity is detected
         * so we can resume ASR processing */
        BOARD_RevertClock();
#endif /* VAD_ENABLED */

            /* Prepare and send clean data to ASR module */
            memcpy(&s_outStream[s_outBlocksCnt * PCM_SINGLE_CH_SMPL_COUNT], cleanStream, PCM_SINGLE_CH_SMPL_COUNT * 2);
            s_outBlocksCnt++;
            if (s_outBlocksCnt == AFE_BLOCKS_TO_ACCUMULATE)
            {
                if (xQueueSendToBack(g_xSampleQueue, s_outStream, 0) == errQUEUE_FULL)
                {
                    RGB_LED_SetColor(LED_COLOR_PURPLE);
                }
                s_outBlocksCnt = 0;
            }
        }

#if VAD_ENABLED
        else
        {
            /* Run a lower MCU frequency when no voice activity is detected, because we can bypass
             * ASR and save power by putting the MCU at lower MHz */
            BOARD_ReduceClock();
        }
#endif /* VAD_ENABLED */

        /* Notify SLN_AFE that a wake word was detected. */
        afeStatus = _sln_afe_trigger_found();
        if (afeStatus != kAfeSuccess)
        {
            configPRINTF(("ERROR [%d]: AFE trigger found failed!\r\n", afeStatus));
            RGB_LED_SetColor(LED_COLOR_RED);
        }
    }
}

/*******************************************************************************
 * Static Functions
 ******************************************************************************/

static sln_afe_status_t _sln_afe_init(void)
{
    sln_afe_status_t afeStatus = kAfeSuccess;
    sln_afe_config_t afeConfig = {0};

    afeConfig.numberOfMics       = SLN_MIC_COUNT;
    afeConfig.afeMemBlock        = s_afeExternalMemory;
    afeConfig.mallocFunc         = pvPortMalloc;
    afeConfig.freeFunc           = vPortFree;
    afeConfig.afeMemBlockSize    = sizeof(s_afeExternalMemory);
    afeConfig.postProcessedGain  = 3;
    afeConfig.wakeWordMaxLength  = WAKE_WORD_MAX_LENGTH_MS;
    afeConfig.micsPosition[0][0] = SLN_MIC1_POS_X;
    afeConfig.micsPosition[0][1] = SLN_MIC1_POS_Y;
    afeConfig.micsPosition[0][2] = 0;
    afeConfig.micsPosition[1][0] = SLN_MIC2_POS_X;
    afeConfig.micsPosition[1][1] = SLN_MIC2_POS_Y;
    afeConfig.micsPosition[1][2] = 0;
#if SLN_MIC_COUNT == 3
    afeConfig.micsPosition[2][0] = SLN_MIC3_POS_X;
    afeConfig.micsPosition[2][1] = SLN_MIC3_POS_Y;
    afeConfig.micsPosition[2][2] = 0;
#endif /* SLN_MIC_COUNT == 3 */

    afeConfig.dataInType  = kAfeTypeInt16;
    afeConfig.dataOutType = kAfeTypeInt16;

#if !AMP_LOOPBACK_DISABLED
    afeConfig.aecEnabled      = 1;
    afeConfig.aecFilterLength = AEC_FILTER_LENGTH;
#else
    afeConfig.aecEnabled      = 0;
    afeConfig.aecFilterLength = 0;
#endif /* !AMP_LOOPBACK_DISABLED */

    afeConfig.mcuType = kAfeMcuMIMXRT1060;

    afeStatus = SLN_AFE_Init(&afeConfig);

    return afeStatus;
}

static sln_afe_status_t _sln_afe_process_audio(int16_t *micStream, int16_t *ampStream, void **cleanStream)
{
    sln_afe_status_t afeStatus = kAfeSuccess;
    int16_t *refSignal         = NULL;

#if ENABLE_AMPLIFIER
    if (SLN_AMP_GetState() == kSlnAmpIdle)
    {
        /* Bypass AEC if there is no streaming (by sending NULL as refSignal).
         * Bypassing AEC greatly reduces CPU usage of SLN_AFE_Process_Audio function. */
        refSignal = NULL;
    }
    else
    {
        refSignal = ampStream;
    }
#endif /* ENABLE_AMPLIFIER */

    afeStatus = SLN_AFE_Process_Audio(micStream, refSignal, cleanStream);

    return afeStatus;
}

static sln_afe_status_t _sln_afe_trigger_found()
{
    sln_afe_status_t afeStatus          = kAfeSuccess;
    uint32_t wakeWordStartOffsetSamples = 0;
    uint32_t wakeWordStartOffsetMs      = 0;

    if (g_wakeWordLength != 0)
    {
        wakeWordStartOffsetSamples = g_wakeWordLength + (s_outBlocksCnt * PCM_SINGLE_CH_SMPL_COUNT);
        wakeWordStartOffsetMs      = wakeWordStartOffsetSamples / (PCM_SAMPLE_RATE_HZ / 1000);

        if (wakeWordStartOffsetMs <= WAKE_WORD_MAX_LENGTH_MS)
        {
            afeStatus        = SLN_AFE_Trigger_Found(wakeWordStartOffsetSamples);
        }
        else
        {
            configPRINTF(("Warning: Reported wake word length (%d ms) bigger than supported max wake word length (%d ms)!\r\n",
                          wakeWordStartOffsetMs, WAKE_WORD_MAX_LENGTH_MS));

            afeStatus        = SLN_AFE_Trigger_Found(WAKE_WORD_MAX_LENGTH_MS * (PCM_SAMPLE_RATE_HZ / 1000));
        }

        g_wakeWordLength = 0;
    }

    return afeStatus;
}

#if VAD_ENABLED
static sln_afe_status_t _sln_afe_vad(int16_t *micStream, bool *voiceActivity)
{
    sln_afe_status_t afeStatus = kAfeSuccess;

    static uint32_t timeout = VAD_FORCED_TRUE_CALLS;
    bool vadResult          = false;
    static bool prevVadResult      = false;
    static int  prevVadCount       = 0;

    afeStatus = SLN_AFE_Voice_Detected(micStream, &vadResult);

    if (vadResult == prevVadResult)
    {
        prevVadCount++;
    }
    else
    {
        prevVadCount = 0;
    }

    prevVadResult = vadResult;

    /* After a Voice Activity was detected, enter in a "ForcedTrue" period for VAD_FORCED_TRUE_CALLS
     * and return True directly. We assume that during this period user will speak so there is no need to
     * actually check for Voice Activity. */
    if (timeout == 0)
    {
        *voiceActivity = false;
    }
    else
    {
        *voiceActivity = true;
    }

    /* Reset ForcedTrue period if needed */
    if ((vadResult == true) && (prevVadCount >= VAD_ACTIVITY_FRAMES))
    {
        timeout        = VAD_FORCED_TRUE_CALLS;
    }

    if (timeout > 0)
    {
        timeout--;
    }


    return afeStatus;
}
#endif /* VAD_ENABLED */
