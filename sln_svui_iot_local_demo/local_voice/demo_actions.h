/*
 * Copyright 2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef DEMO_ACTIONS_H_

#define DEMO_ACTIONS_H_

#if ENABLE_DSMT_ASR
#include "demo_actions_dsmt.h"
#elif ENABLE_VIT_ASR
#include "demo_actions_vit.h"
#endif /* ENABLE_DSMT_ASR */

#endif /* DEMO_ACTIONS_H_ */
