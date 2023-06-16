/*
 * Copyright 2021 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef _SLN_PRDB_H_
#define _SLN_PRDB_H_

#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief  Validate encryption contexts.
 *         If any context missing, restore them and reboot the device.
 *
 * @return kStatus_Success if all contexts are valid.
 */
status_t SLN_PRDB_EncryptionCheck(void);

#endif /* _SLN_PRDB_H_ */
