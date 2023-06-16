/*
 * Copyright 2021 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef _BOOTSTRAP_MAIN_H_
#define _BOOTSTRAP_MAIN_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief BOOTSTRAP Task settings */
#define BOOTSTRAP_TASK_NAME       "Bootstrap_Task"
#define BOOTSTRAP_TASK_STACK_SIZE 1024
#define BOOTSTRAP_TASK_PRIORITY   (configMAX_PRIORITIES - 1)

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief If ENABLE_ENCRYPTION is 1 and device has encrypted XIP enabled, check PRDB contexts.
 *        If something wrong, reset.
 *        If DISABLE_IMAGE_VERIFICATION is 0, validate available images (Bootloader, Current Application, Backup
 *        Application) using theirs certificates. Select as "next application" first valid image. If none validated,
 *        reset. Jump to "next application".
 *
 * @param arg Not used
 */
void BOOTSTRAP_Task(void *arg);

#endif /* _BOOTSTRAP_MAIN_H_ */
