/*
 * Copyright 2019-2022 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef _SLN_FLASH_OPS_
#define _SLN_FLASH_OPS_

#include "flexspi_nor_flash_ops.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define sln_flash_ops_init(a)               flexspi_nor_flash_init(a)
#define sln_flash_ops_get_flash_id(a, b)    flexspi_nor_get_vendor_id(a, b)
#define sln_flash_ops_page_program(a, b, c) flexspi_nor_flash_page_program(a, b, c)
#define sln_flash_ops_erase_sector(a, b)    flexspi_nor_flash_erase_sector(a, b)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

#endif /* _SLN_FLASH_OPS_ */
