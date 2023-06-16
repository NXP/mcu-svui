/*
 * Copyright 2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

/* Application version */
#define APP_MAJ_VER                    0x01
#define APP_MIN_VER                    0x00
#define APP_BLD_VER                    0x0003

/* Define this to 1 when building for reference design (16 MB flash)
 * Otherwise the project assumes flash size is 32 MB */
#define SLN_SVUI_RD                    0

/* Choose the ASR engine, DSMT or VIT */
#define ENABLE_DSMT_ASR                0
#define ENABLE_VIT_ASR                 1

/* Set USE_DSMT_EVALUATION_MODE to 1 when using DSMT evaluation library */
#if ENABLE_DSMT_ASR
#define USE_DSMT_EVALUATION_MODE 1
#endif /* ENABLE_DSMT_ASR */

/* Enable Voice Activity Detection */
#define VAD_ENABLED                    1

/* Choose microphones type. If using PDM configuration,
 * switch the definition to MICS_PDM */
#define MICS_TYPE                      MICS_I2S

/* Speaker volume, between 0 and 100 */
#define DEFAULT_SPEAKER_VOLUME         55

/* Enable audio dump by setting this define on 1 */
#define ENABLE_AUDIO_DUMP              0

/* Max wake word length. Might consider switching to 1500
 * for shorter wake words. We are using 3seconds to accommodate
 * larger utterances like "Salut NXP" */
#define WAKE_WORD_MAX_LENGTH_MS        3000

/* Disable reference signal. When set to 1, barge-in will not work.
 * Disabling saves RAM memory. */
#define AMP_LOOPBACK_DISABLED          1

/* Enable NXP out of the box experience. If set to 0,
 * no demo change or language change available through voice commands,
 * but these actions will still be possible through shell commands. */
#define ENABLE_NXP_OOBE                1

/* If set to 1, streamer task will run and it will decode opus encoded
 * audio files, then feed raw PCM audio data to the speaker.
 * If set to 0, streamer task will not feed raw PCM audio data to the speaker.
 * Disabling the streamer save RAM memory. */
#define ENABLE_STREAMER                1

/* Enable CPU usage tracing. When set to 1, a new command will be available in
 * sln_shell: cpu_view. This will print CPU usage per task */
#define SLN_TRACE_CPU_USAGE            0
