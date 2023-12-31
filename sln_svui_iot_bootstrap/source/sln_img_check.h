/*
 * Copyright 2021 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef _SLN_IMG_CHECK_H_
#define _SLN_IMG_CHECK_H_

#include <stdint.h>
#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief  Search through (Bootloader, Active Application, Backup Application) images list and detects
 *         first valid image address.
 *
 * @param imageAddr Pointer where to store first valid image address.
 *
 * @return kStatus_Success if a valid image was found.
 */
status_t SLN_IMG_CHECK_GetValidImgAddress(uint32_t *imageAddr);

#endif /* _SLN_IMG_CHECK_H_ */
