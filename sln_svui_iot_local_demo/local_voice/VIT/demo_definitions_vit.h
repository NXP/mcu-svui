/*
 * Copyright 2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef DEMO_DEFINITIONS_VIT_H_
#define DEMO_DEFINITIONS_VIT_H_

#if ENABLE_VIT_ASR

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
    ASR_CMD_INVALID_DEMO
} asr_inference_t;

/* listen for multiple wake words in parallel; not supported for VIT */
#define MULTILINGUAL               (0)

/* demo after first boot. can be used for selecting the demo at the very first board boot */
#define BOOT_ASR_CMD_DEMO          ASR_CMD_CHANGE_DEMO

/* default demo. this can have the same value as BOOT_ASR_CMD_DEMO
 * in our POC we use BOOT_ASR_CMD_DEMO to give the possibility to the user to select the demo
 * via voice commands. If a selection is not made until the timeout is hit, then
 * DEFAULT_ASR_CMD_DEMO will be loaded */
#define DEFAULT_ASR_CMD_DEMO       ASR_CMD_SMART_HOME

/* default language */
#define DEFAULT_ASR_LANGUAGE       ASR_ENGLISH

/* max languages active at the same time */
#define MAX_CONCURRENT_LANGUAGES   (1)

/* Strings used for shell printing or shell commands */
#define LANG_STR_CN "cn"
#define LANG_STR_DE "de"
#define LANG_STR_EN "en"
#define LANG_STR_FR "fr"

#define DEMO_STR_ELEVATOR    "elevator"
#define DEMO_STR_WASHING     "wash"
#define DEMO_STR_SMART_HOME  "smart"

#define SHELL_SELECTABLE_DEMOS DEMO_STR_ELEVATOR " " DEMO_STR_WASHING " " DEMO_STR_SMART_HOME
#define SHELL_SELECTABLE_LANGUAGES LANG_STR_CN " " LANG_STR_DE " " LANG_STR_EN " " LANG_STR_FR

#endif /* ENABLE_VIT_ASR */
#endif /* DEMO_DEFINITIONS_VIT_H_ */
