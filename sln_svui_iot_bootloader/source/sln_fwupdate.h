/*
 * Copyright 2019-2021 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef SLN_FWUPDATE_H_
#define SLN_FWUPDATE_H_

/* Compare the value read from FICA_COMM_FWUPDATE_METHOD_BIT */
#define METHOD_SLN_OTA 0
#define METHOD_SLN_OTW 1

/*!
 * @brief TBD
 */
#define selfTestCleanup() \
    {                     \
    }
/*!
 * @brief Check for any update flags in the FICA and starts the update mechanism.
 *
 * @return True if there is an update request pending, false othewise.
 */

bool FWUpdate_CheckForUpdate(void);

/*!
 * @brief Starts IoT Solution firmware update over UART
 */

void otwAppStart(void);

/*!
 * @brief Starts IoT Solution firmware update over TCP
 */

void otaAppStart(void);

#endif /* SLN_FWUPDATE_H_ */
