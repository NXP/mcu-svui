/*
 * Copyright 2022-2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#if !AMP_LOOPBACK_DISABLED

#include "stdint.h"

#include "FreeRTOS.h"
#include "ringbuffer.h"
#include "semphr.h"
#include "sln_mic_config.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Filter order */
#define FILTER_ORDER 32
#define F3_NUM_TAPS  (FILTER_ORDER - 1)

/* Coefficients used for amp filtering. */
static const int32_t kDefaultF3Coeffs24KHz[F3_NUM_TAPS] =
{
        -17, -6, 62, -145, 190, -104, -179, 613,
        -991, 982, -262, -1303, 3520, -5871, 7672, 31653,
        7672, -5871, 3520, -1303, -262, 982, -991, 613,
        -179, -104, 190, -145, 62, -6, -17,
};

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Buffer used for amp filtering. */
static int16_t s_dsBuffer[FILTER_ORDER] = {0};

#if USE_MQS
/* Buffer to store 48KHz PCM data from the speaker. */
__attribute__((section(".data.$SRAM_DTC"))) __attribute__((aligned(32))) static int16_t s_amp48KhzData[PCM_AMP_SAMPLE_COUNT] = {0};
#endif /* USE_MQS */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void SLN_AMP_FirLowPassFilter(int16_t *samples, uint32_t samplesCnt);
static void SLN_AMP_DownsampleDiffData(uint8_t *in, int16_t *out);;

/*******************************************************************************
 * Code
 ******************************************************************************/

/**
 * @brief Apply FIR Low Pass Filter on amp differential data.
 *        Differential data means that Filter should skip every second sample.
 *
 * @param samples Pointer to the buffer containing 2B samples of data.
 * @param samplesCnt Number of samples in the buffer.
 */
static void SLN_AMP_FirLowPassFilter(int16_t *samples, uint32_t samplesCnt)
{
    int32_t acc = 0;

    for (uint32_t idx = 0; idx < samplesCnt; idx += 2)
    {
        acc = 0;

        /* Pull new data into buffer */
        s_dsBuffer[0] = samples[idx];

        acc += kDefaultF3Coeffs24KHz[1] * (s_dsBuffer[1] + s_dsBuffer[29]);
        acc += kDefaultF3Coeffs24KHz[2] * (s_dsBuffer[2] + s_dsBuffer[28]);

        acc += kDefaultF3Coeffs24KHz[4] * (s_dsBuffer[4] + s_dsBuffer[26]);
        acc += kDefaultF3Coeffs24KHz[5] * (s_dsBuffer[5] + s_dsBuffer[25]);

        acc += kDefaultF3Coeffs24KHz[7] * (s_dsBuffer[7] + s_dsBuffer[23]);
        acc += kDefaultF3Coeffs24KHz[8] * (s_dsBuffer[8] + s_dsBuffer[22]);

        acc += kDefaultF3Coeffs24KHz[10] * (s_dsBuffer[10] + s_dsBuffer[20]);
        acc += kDefaultF3Coeffs24KHz[11] * (s_dsBuffer[11] + s_dsBuffer[19]);

        acc += kDefaultF3Coeffs24KHz[13] * (s_dsBuffer[13] + s_dsBuffer[17]);
        acc += kDefaultF3Coeffs24KHz[14] * (s_dsBuffer[14] + s_dsBuffer[16]);

        acc += kDefaultF3Coeffs24KHz[15] * (s_dsBuffer[15] + 0);

        /* Shift values up in buffer */
        for (uint32_t idx = F3_NUM_TAPS; idx > 0; idx--)
        {
            s_dsBuffer[idx] = s_dsBuffer[idx - 1];
        }

        samples[idx / 2] = (int16_t)(acc >> 15);
    }
}

/**
 * @brief Process amplifier differential samples.
 *        Amplifier's buffer contains 480 sample, but because they are differential samples,
 *        each second sample is its predecessor multiplied with (-1). So, in fact, there are
 *        only 240 actual samples of data. The garbage samples will be ignored.
 *        Apply Low Pass Filter to offer a better quality after down sampling.
 *        Down sample data from PCM_AMP_SAMPLE_RATE_HZ (24KHz) to PCM_SAMPLE_RATE_HZ (16KHz).
 *
 * @param in Pointer to the buffer containing the samples.
 * @param out Pointer where to store processed data.
 */
static void SLN_AMP_DownsampleDiffData(uint8_t *in, int16_t *out)
{
    uint32_t i = 0;
    uint32_t j = 0;;
    int16_t *inBuff = (int16_t *)in;

    /* Apply Low Pass Filter to offer a better quality after the downsampling */
    SLN_AMP_FirLowPassFilter(inBuff, PCM_AMP_SAMPLE_COUNT);

    /* Down sample from 24KHz to 16KHz */
    for (i = 0; i < PCM_SINGLE_CH_SMPL_COUNT; i += 2)
    {
        out[i]     = (inBuff[j] / 2) + (inBuff[j + 1] / 2);
        out[i + 1] = (inBuff[j + 1] / 2) + (inBuff[j + 2] / 2);
        j += 3;
    }
}

/*******************************************************************************
 * API
 ******************************************************************************/

void SLN_AMP_GetAmpStream(mic_task_config_t *s_taskConfig, int16_t *buffOut, volatile uint32_t *s_pingPongTimestamp)
{
    uint32_t ampProcessDataSize   = 0;
    uint32_t ampRingBuffOcc       = 0;
    static uint8_t ampOutputDirty = EDMA_TCD_COUNT;

    if ((s_taskConfig->loopbackMutex == NULL) || (s_taskConfig->loopbackRingBuffer == NULL) ||
        (s_taskConfig->updateTimestamp == NULL) )
    {
        return;
    }

    /* Read the loopback data from the amplifier`s ringbuffer.
     * Do not read more than 10ms of data. */
    xSemaphoreTake(s_taskConfig->loopbackMutex, portMAX_DELAY);

    s_taskConfig->updateTimestamp(*s_pingPongTimestamp);

    ampRingBuffOcc = ringbuf_get_occupancy(s_taskConfig->loopbackRingBuffer);
    if (ampRingBuffOcc > PCM_AMP_DATA_SIZE_10_MS)
    {
        ampProcessDataSize = PCM_AMP_DATA_SIZE_10_MS;
    }
    else
    {
        ampProcessDataSize = ampRingBuffOcc;
    }

    ringbuf_read(s_taskConfig->loopbackRingBuffer, (uint8_t *)s_amp48KhzData, ampProcessDataSize);

    xSemaphoreGive(s_taskConfig->loopbackMutex);

    /* In case of need, add padding zeroes to form a 10ms chunk of data.
     * Downsample by 3 the data and place it in the downsampled buffer.
     * Amplify the data by the AMP_SIGNAL_MULTIPLIER factor.
     * In case there is no available data, clear the downsampled buffer. */
    if (ampProcessDataSize > 0)
    {
        /* avoid out of bounds read by dereferencing pointer when ampProcessDataSize is PCM_AMP_DATA_SIZE_10_MS */
        if (PCM_AMP_DATA_SIZE_10_MS - ampProcessDataSize)
        {
            memset(&((uint8_t *)s_amp48KhzData)[ampProcessDataSize], 0, (PCM_AMP_DATA_SIZE_10_MS - ampProcessDataSize));
        }

        SLN_AMP_DownsampleDiffData((uint8_t *)s_amp48KhzData, buffOut);

        ampOutputDirty = EDMA_TCD_COUNT;
    }
    else if (ampOutputDirty > 0)
    {
        ampOutputDirty--;
        memset(buffOut, 0, PCM_SINGLE_CH_SMPL_COUNT * PCM_SAMPLE_SIZE_BYTES);
    }
}

#endif /* !AMP_LOOPBACK_DISABLED */
