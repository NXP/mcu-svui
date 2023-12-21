/*
 * Copyright 2019, 2021-2023 NXP.
 * NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly
 * in accordance with the applicable license terms. By expressly accepting such terms or by downloading,
 * installing, activating and/or otherwise using the software, you are agreeing that you have read, and
 * that you agree to comply with and are bound by, such license terms. If you do not agree to be bound by
 * the applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef _FICA_DRIVER_H_
#define _FICA_DRIVER_H_

/*!
 * @addtogroup flash_ica
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#include <stdbool.h>

#define SLN_FLASH_NO_ERROR 0
#define SLN_FLASH_ERROR    -1

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Write the FICA database from the buffer
 *
 */
int32_t FICA_write_db(void);

/*!
 * @brief Get the application external flash start address, given the passed image type
 *
 */
int32_t FICA_get_app_img_start_addr(int32_t imgtype, uint32_t *startaddr);

/*!
 * @brief Write the image info to external memory
 *
 */
int32_t FICA_write_image_info(int32_t imgtype, uint32_t imgfmt, uint32_t len);

/*!
 * @brief Is Flash Image Config Area Initialized ?
 *
 */
bool is_FICA_initialized(void);

/*!
 * @brief Check if FICA is initialized, initialize it if needed
 *
 */
int32_t FICA_initialize(void);

/*!
 * @brief Initialize External Flash driver
 *
 */
int32_t FICA_verify_ext_flash(void);

/*!
 * @brief Reads the flash ID
 *
 */
int32_t FICA_read_flash_id(uint8_t *pid);

/*!
 * @brief Reads the address of the current application programmed in flash
 *
 */
int32_t FICA_GetCurAppStartType(int32_t *pimgtype);

/*!
 * @brief Writes the current start image type
 *
 */
int32_t FICA_SetCurAppStartType(int32_t imgtype);

#endif /* _FICA_DRIVER_H_ */
