/*
 * Copyright 2019, 2021-2022 NXP
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef _SLN_MSC_VFS_H_
#define _SLN_MSC_VFS_H_

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "fica_definition.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Special MSD update file names */
#define FILENAME_APP_A "APP_A"
#define FILENAME_APP_B "APP_B"
#define FILENAME_FS    "LFS"

typedef enum __transfer_state
{
    TRANSFER_IDLE,
    TRANSFER_START,
    TRANSFER_ACTIVE,
    TRANSFER_FINAL,
    TRANSFER_ERROR
} msc_vfs_state_t;

typedef struct __attribute__((packed)) __fat_file
{
    uint8_t name[11]; /*!< File name (0-7) and 3-byte extension (8-10) */
    uint8_t attributes;
    uint8_t reserved;
    uint8_t time_created_ms;
    uint16_t time_created;
    uint16_t date_created;
    uint16_t date_accessed;
    uint16_t first_cluser_upper;
    uint16_t time_modified;
    uint16_t date_modified;
    uint16_t first_cluster_lower;
    uint32_t size;
} fat_file_t;

typedef struct __attribute__((packed)) __fat_mbr
{
    uint8_t jump_instr[3];
    uint8_t oem_name[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t num_fats;
    uint16_t root_entries;
    uint16_t num_logical_sectors;
    uint8_t media_type;
    uint16_t logical_sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t num_heads;
    uint32_t hidden_sectors;
    uint32_t large_sectors;
    uint8_t physical_drive_number;
    uint8_t not_used;
    uint8_t boot_record_signature;
    uint32_t volume_sn;
    char volume_label[11];
    char fat_type[8];
    uint8_t bootstrap[448];
    uint16_t signature;
} fat_mbr_t;

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief  Initialize MSD internal variables.
 *
 * @param  storageDisk Pointer where to store MSD properties.
 * @param  usbAppTask Pointer to the MSD main task handle.
 * @param  lbaLength Length of Each Logical Address Block.
 *
 * @return kStatus_Success on success.
 */
status_t MSC_VFS_Init(uint8_t *storageDisk, TaskHandle_t *usbAppTask, uint32_t lbaLength);

/**
 * @brief  Process incoming USB packets.
 *
 * @param  offset Offset of the received packet.
 * @param  size Size of the received packet.
 * @param  buffer Pointer to the received packet data.
 *
 * @return kStatus_Success on success.
 */
status_t MSC_VFS_WriteResponse(uint32_t offset, uint32_t size, uint8_t *buffer);

/**
 * @brief  Process incoming read USB packets.
 *
 * @param  offset Offset from witch packets are requested.
 * @param  size Size that is requested.
 * @param  buffer Pointer to a buffer that is going to be returned.
 *
 * @return kStatus_Success on success.
 */
status_t MSC_VFS_ReadResponse(uint32_t offset, uint32_t size, uint8_t *buffer);

/**
 * @brief  Get current state of the transfer process.
 *
 * @return Return current state of the transfer process.
 */
msc_vfs_state_t MSC_VFS_GetTransferState(void);

/**
 * @brief  Set current state of the transfer process.
 *
 * @param  transferState New state of the transfer process.
 */
void MSC_VFS_SetTransferState(msc_vfs_state_t transferState);

/**
 * @brief  Get current transfer process type.
 *
 * @return Return current transfer process type (application, filesystem etc.).
 */
fica_img_type_t MSC_VFS_GetTransferType(void);

/**
 * @brief  Notify that MSD main task finalized processing incoming update file.
 */
void MSC_VFS_SetFileProcessed(void);

/**
 * @brief  Loops (with timeout) until a new update file is pending.
 *
 * @param  maxTimeoutMs Max time in milliseconds to wait.
 *
 * @return Return true if a new file is pending to be downloaded, false otherwise.
 */
bool MSC_VFS_WaitNewFile(uint32_t maxTimeoutMs);

#endif /* _SLN_MSC_VFS_H_ */
