/*
 * Copyright 2019-2021, 2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef __PERF_H__
#define __PERF_H__

#include "FreeRTOS.h"
#include "task.h"

#if defined(__cplusplus)
extern "C" {
#endif /*_cplusplus*/

#if SLN_TRACE_CPU_USAGE
/**
 * @brief Returns a pointer to the string containing the CPU load info
 */
char *PERF_GetCPULoad(void);
#endif /* SLN_TRACE_CPU_USAGE */

/**
 * @brief print minimum heap remaining
 */
void PERF_PrintHeap(void);

/**
 * @brief print the statistics of memory tasks stack consuming
 */
void PERF_PrintStacks(void);

#if defined(__cplusplus)
}
#endif /*_cplusplus*/

#endif
