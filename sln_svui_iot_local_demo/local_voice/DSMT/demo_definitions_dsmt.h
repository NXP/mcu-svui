/*
 * Copyright 2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef DEMO_VOICE_MODELS_DSMT_H_
#define DEMO_VOICE_MODELS_DSMT_H_

#if ENABLE_DSMT_ASR

/* Enumeration of the languages supported by the project
 * Add, remove or replace languages here, but keep the one bit
 * per language allocation, as currently implemented.
 * The code assumes one bit allocation per definition */
typedef enum _asr_languages
{
    UNDEFINED_LANGUAGE = 0,
    ASR_FIRST_LANGUAGE = (1U << 0U),
    ASR_ENGLISH        = (1U << 0U),
    ASR_CHINESE        = (1U << 1U),
    ASR_GERMAN         = (1U << 2U),
    ASR_FRENCH         = (1U << 3U),
    ASR_ALL_LANG       = (ASR_ENGLISH | ASR_CHINESE | ASR_GERMAN | ASR_FRENCH)
} asr_language_t;

/* Enumeration of the command sets integrated into the project
 * Add, remove or replace the command sets here.
 * This must match the order from the DSMT project.
 * Keep the one bit per command set allocation, as currently implemented.
 * The code assumes one bit allocation per definition */
typedef enum _asr_inference
{
    UNDEFINED_INFERENCE     = 0,
    ASR_WW                  = (1U << 0U),
    ASR_CMD_CHANGE_DEMO     = (1U << 1U),
    ASR_CMD_ELEVATOR        = (1U << 2U),
    ASR_CMD_WASHING_MACHINE = (1U << 3U),
    ASR_CMD_SMART_HOME      = (1U << 4U),
} asr_inference_t;

/* number of command groups, besides wake word group
 * in our implementation: change_demo, elevator, washing machine, smart home */
#define NUM_CMD_GROUPS          (4)

/* languages supported in the firmware (not necessarily running at the same time)
 * this number can be increased depending on the project needs */
#define NUM_LANGUAGES           (4)

/* listen for multiple wake words in parallel */
#define MULTILINGUAL            (1)

/* maximum buffer length in number of frames for keeping the status of commands
 * a frame has 10ms, so with the default value we support maximum 5 seconds long commands
 * this number can be increased depending on the project needs */
#define MAX_COMMAND_FRAMES      (500)

/* the response waiting time in ASR session */
#define TIMEOUT_TIME_IN_MS      (8000)

/* demo after first boot. can be used for selecting the demo at the very first board boot */
#define BOOT_ASR_CMD_DEMO       ASR_CMD_CHANGE_DEMO

/* default demo. this can have the same value as BOOT_ASR_CMD_DEMO
 * in our POC we use BOOT_ASR_CMD_DEMO to give the possibility to the user to select the demo
 * via voice commands. If a selection is not made until the timeout is hit, then
 * DEFAULT_ASR_CMD_DEMO will be loaded */
#define DEFAULT_ASR_CMD_DEMO    ASR_CMD_SMART_HOME

/* default language */
#define DEFAULT_ASR_LANGUAGE    ASR_ENGLISH

#if MULTILINGUAL
/* max languages active at the same time. for CPU bandwidth reasons
 * we cannot support more than 4 languages with DSMT model level 1
 * and only 1 language with DSMT model level 4 */
#define MAX_CONCURRENT_LANGUAGES (4)
#else
#define MAX_CONCURRENT_LANGUAGES (1)
#endif /* MULTILINGUAL */

/* Strings used for shell printing or shell commands */
#define LANG_STR_CN "cn"
#define LANG_STR_DE "de"
#define LANG_STR_EN "en"
#define LANG_STR_FR "fr"

#define DEMO_STR_CHANGE_DEMO "change_demo"
#define DEMO_STR_ELEVATOR    "elevator"
#define DEMO_STR_WASHING     "wash"
#define DEMO_STR_SMART_HOME  "smart"

#define SHELL_SELECTABLE_DEMOS DEMO_STR_ELEVATOR " " DEMO_STR_WASHING " " DEMO_STR_SMART_HOME
#define SHELL_SELECTABLE_LANGUAGES LANG_STR_CN " " LANG_STR_DE " " LANG_STR_EN " " LANG_STR_FR

#endif /* ENABLE_DSMT_ASR */

#endif /* DEMO_VOICE_MODELS_DSMT_H_ */
