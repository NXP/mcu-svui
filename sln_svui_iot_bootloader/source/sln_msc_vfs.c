/*
 * Copyright 2019, 2021-2023 NXP
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#include <stdint.h>
#include "disk.h"
#include "fsl_common.h"
#include "sln_msc_vfs.h"
#include "flash_ica_driver.h"
#if ENABLE_ENCRYPTION
#include "nor_encrypt_bee.h"
#endif /* ENABLE_ENCRYPTION */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* The size at which to print a message in the log to avoid flooding */
#define USB_DISPLAY_SIZE 0x10000
/* The offset set by the host when sending file information */
#define USB_FILE_OFFSET 0x18
/* Used to ensure binary is using an address in flash */
#define FLASH_BYTE4_UPPER_NIBBLE 0xF0000000
/* Used to get start addr of a binary */
#define FLASH_BYTE3_UPPER_NIBBLE 0x00F00000
/* Minimal size for MSD update file */
#define MSD_FILE_MINIMUM_SIZE 4096

typedef enum _msd_update_type
{
    kMsdUpdateType_None,
    kMsdUpdateType_App_Bank_Opposite,
    kMsdUpdateType_App_Bank_A,
    kMsdUpdateType_App_Bank_B,
    kMsdUpdateType_Lfs,
} msd_update_type_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

static uint8_t s_filesReceivedCnt  = 0;
static uint8_t s_filesProcessedCnt = 0;
static uint8_t s_respondMsc        = false;
static bool s_curFileMetaFound     = false;
static uint32_t s_curFileLength    = 0;

static uint32_t s_startOffset  = 0;
static uint32_t s_dataWritten  = 0;
static uint32_t s_lbaLength    = 0;

static msd_update_type_t s_msdUpdateType = kMsdUpdateType_None;
static msc_vfs_state_t s_transferState   = TRANSFER_IDLE;
static fica_img_type_t s_destinationBank = FICA_IMG_TYPE_NONE;

static TaskHandle_t *s_usbAppTaskHandle = NULL;

const char *ext = "BIN";

static const fat_mbr_t s_fatMbrInit = {.jump_instr              = {0xEB, 0x3C, 0x90},
                                       .oem_name                = {'M', 'S', 'D', '0', 'S', '5', '.', '0'},
                                       .bytes_per_sector        = 0x0200,
                                       .sectors_per_cluster     = 0x08,
                                       .reserved_sectors        = 0x0008,
                                       .num_fats                = 0x02,
                                       .root_entries            = 0x0200,
                                       .num_logical_sectors     = 0x5000,
                                       .media_type              = 0xf0,
                                       .logical_sectors_per_fat = 0x0008,
                                       .sectors_per_track       = 0x003F,
                                       .num_heads               = 0x00FF,
                                       .hidden_sectors          = 0x00000000,
                                       .large_sectors           = 0x00000000,
                                       .physical_drive_number   = 0x08,
                                       .not_used                = 0x00,
                                       .boot_record_signature   = 0x29,
                                       .volume_sn               = 0x3FF6BD53,
                                       .volume_label = {'S', 'L', 'N', '-', 'B', 'O', 'O', 'T', 0x00, 0x00, 0x00},
                                       .fat_type     = {'F', 'A', 'T', '1', '6', ' ', ' ', ' '},
                                       .bootstrap    = {0x00},
                                       .signature    = 0xAA55};

/*******************************************************************************
 * Code
 ******************************************************************************/

static bool MSC_isApplicationValid(uint32_t size, uint8_t *buffer, int32_t *currImgType)
{
    bool isFlash       = false;
    bool isValid       = false;
    uint8_t allZero    = true;
    int32_t flashError = SLN_FLASH_NO_ERROR;

    for (int i = 0; i < size; i++)
    {
        if (buffer[i] != 0)
        {
            allZero = false;
            break;
        }
    }

    if (allZero)
    {
        /* We received only 0. We can't say if it is valid or not */
        configPRINTF(("[AppValidation] Received empty first packet. Wait..\r\n"));
    }
    else
    {
        configPRINTF(("[AppValidation] Non 0 element found. Packet is not zeroed \r\n"));

        /* Determine the base programming address from the passed image reset vector */
        uint32_t *resetHandler = (uint32_t *)(&buffer[4]);
        *currImgType           = FICA_IMG_TYPE_NONE;
        uint32_t imgBaseAddr   = (uint32_t)(*resetHandler & FLASH_BYTE3_UPPER_NIBBLE);

        isFlash = ((FLASH_BYTE4_UPPER_NIBBLE & *resetHandler) == FLEXSPI_AMBA_BASE);
        if (isFlash)
        {
            flashError = FICA_GetImgTypeFromAddr(imgBaseAddr, currImgType);
            if (SLN_FLASH_NO_ERROR == flashError)
            {
#if ENABLE_REMAP
                if (*currImgType == FICA_IMG_TYPE_APP_A)
                {
                    isValid = true;
                }
#else
                if ((currImgType == FICA_IMG_TYPE_APP_A) || (currImgType == FICA_IMG_TYPE_APP_B))
                {
                    if ((SLN_MSD_FallbackGet() == true) && ((s_msdUpdateType == kMsdUpdateType_App_Bank_Opposite)))
                    {
                        /* In fallback mode, any of BankA or BankB updates are accepted.
                         * Set destination bank to be equal to the received bank update. */
                        s_destinationBank = currImgType;
                    }

                    if (s_destinationBank == currImgType)
                    {
                        isValid = true;
                    }
                }
#endif /* ENABLE_REMAP */
            }
        }
    }

    return isValid;
}

status_t MSC_VFS_Init(uint8_t *storageDisk, TaskHandle_t *usbAppTask, uint32_t lbaLength)
{
    status_t status = kStatus_Fail;

    if ((NULL != storageDisk) && (NULL != usbAppTask) && (0 != lbaLength))
    {
        status = kStatus_Success;

        memcpy(&storageDisk[0], &s_fatMbrInit, sizeof(fat_mbr_t));

        s_usbAppTaskHandle = usbAppTask;

        s_lbaLength = lbaLength;

        s_curFileLength    = 0;
        s_dataWritten   = 0;
        s_transferState = TRANSFER_IDLE;
    }

    return status;
}

msc_vfs_state_t MSC_VFS_GetTransferState(void)
{
    return s_transferState;
}

void MSC_VFS_SetTransferState(msc_vfs_state_t transferState)
{
    s_transferState = transferState;
}

fica_img_type_t MSC_VFS_GetTransferType(void)
{
    return s_destinationBank;
}

bool MSC_VFS_WaitNewFile(uint32_t maxTimeoutMs)
{
    bool newFile       = false;
    uint32_t timeoutMs = 0;

    while (timeoutMs < maxTimeoutMs)
    {
        if (s_filesReceivedCnt > s_filesProcessedCnt)
        {
            newFile = true;
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
        timeoutMs += 10;
    }

    return newFile;
}

void MSC_VFS_SetFileProcessed(void)
{
    s_filesProcessedCnt++;
}

status_t MSC_VFS_ReadResponse(uint32_t offset, uint32_t size, uint8_t *buffer)
{
    if ((TRANSFER_ACTIVE == s_transferState) && (offset >= s_startOffset))
    {
        configPRINTF(("[ReadRequest] Read request with offset 0x%X, size %d\r\n", offset, size));

        /* Calculate the image address for where to store this data */
        uint32_t imgOffset = ((offset - s_startOffset) * s_lbaLength);

        FICA_app_program_read(buffer, imgOffset, size);
        /* More retransmission might occur. wait */
        s_respondMsc = true;
    }

    return kStatus_Success;
}

status_t MSC_VFS_WriteResponse(uint32_t offset, uint32_t size, uint8_t *buffer)
{
    static bool isRetransmission      = false;
    static bool isValid               = false;
    static int32_t currImgType        = FICA_IMG_TYPE_NONE;
    static uint32_t expectedNewOffset = 0;
    status_t status    = kStatus_Success;
    int32_t flashError = SLN_FLASH_NO_ERROR;
    int32_t oldImgType = FICA_IMG_TYPE_NONE;

    if ((buffer == NULL) || (size == 0))
    {
        configPRINTF(("[Error] MSC_VFS_WriteResponse bad parameters %p %d\r\n", buffer, size));
        status = kStatus_InvalidArgument;
    }

    if (offset == USB_FILE_OFFSET)
    {
        if ((s_curFileLength > 0) && (s_dataWritten >= s_curFileLength))
        {
            if (s_respondMsc == true)
            {
                /* Host will resent the file information if the read request was bad */
                configPRINTF(("Don't finish. We might have retransmission \r\n"));
            }
            else
            {
                if (isRetransmission == true)
                {
                    configPRINTF(("[Write Response] Retransmission ended \r\n"));
                    isRetransmission = false;
                }

                configPRINTF(("[Download] (100%s) DONE \r\n", "%%"));

                if (isValid)
                {
                    s_curFileMetaFound = 0;
                    s_dataWritten      = 0;
                    s_curFileLength    = 0;
                    s_transferState    = TRANSFER_FINAL;
                    expectedNewOffset  = 0;
                    isValid            = false;

                    /* Wake up the application task to finalize transfer */
                    vTaskResume(*s_usbAppTaskHandle);

                    /* Loop until MSD task finishes to process previous update file. */
                    while (s_filesProcessedCnt != s_filesReceivedCnt)
                    {
                        vTaskDelay(pdMS_TO_TICKS(10));
                    }
                }
                else
                {
                    /* We rcv the whole image, but it is not valid */
                    configPRINTF(("[Error] New image has bad type %d\r\n", currImgType));
                    s_transferState = TRANSFER_ERROR;
                    status          = kStatus_Fail;
                }
            }
        }
    }

    /* Clear the respond flag as we received either file information, retransmission or new data */
    if (s_respondMsc)
    {
        s_respondMsc = false;
    }

    if (status == kStatus_Success)
    {
        /* If not found yet, search for the metadata of the receiving file.
         * Do this once per MSD run. */
        if (s_curFileMetaFound == false)
        {
            char *ret          = NULL;
            uint8_t filesFound = 0;

            /* Search for the file extension.
             * In case of multiple files, their metadatas are enumerated consecutively. */
            for (uint32_t idx = 0; idx < size; idx++)
            {
                if (0 == memcmp(&buffer[idx], ext, 3))
                {
                    if (s_filesReceivedCnt == filesFound)
                    {
                        ret = (char *)(&buffer[idx]);
                        break;
                    }
                    filesFound++;
                }
            }

            /* Check file's metadata. */
            if (NULL != ret)
            {
                char *temp        = ret - 8;
                fat_file_t *file  = (fat_file_t *)temp;
                char fileName[12] = {0};

                if (file->size > 0)
                {
                    if (file->size < MSD_FILE_MINIMUM_SIZE)
                    {
                        configPRINTF(("[Error] File size %d B is too small for minimal admitted %d B\r\n",
                                file->size,
                                MSD_FILE_MINIMUM_SIZE));

                        s_transferState = TRANSFER_ERROR;
                        status           = kStatus_Fail;
                    }
                    else
                    {
                        memcpy(fileName, file->name, sizeof(file->name));
                        configPRINTF(("[Download] File Attributes: Name - %s, Size - %d B\r\n",
                                      (const char *)fileName, file->size));

                        s_curFileMetaFound = true;
                        s_curFileLength    = file->size;
                        s_startOffset      = 0;
                        s_transferState    = TRANSFER_IDLE;
                        s_filesReceivedCnt++;

                        if (strncmp(fileName, FILENAME_APP_A, strlen(FILENAME_APP_A)) == 0)
                        {
                            s_msdUpdateType   = kMsdUpdateType_App_Bank_A;
                            s_destinationBank = FICA_IMG_TYPE_APP_A;

                            configPRINTF(("[Download] Application update for Bank A. Destination Bank %d\r\n", s_destinationBank));
                        }
                        else if (strncmp(fileName, FILENAME_APP_B, strlen(FILENAME_APP_B)) == 0)
                        {
                            s_msdUpdateType   = kMsdUpdateType_App_Bank_B;
                            s_destinationBank = FICA_IMG_TYPE_APP_B;

                            configPRINTF(("[Download] Application update for Bank B. Destination Bank %d\r\n", s_destinationBank));
                        }
                        else if (strncmp(fileName, FILENAME_FS, strlen(FILENAME_FS)) == 0)
                        {
                            s_msdUpdateType   = kMsdUpdateType_Lfs;
                            s_destinationBank = FICA_IMG_TYPE_FS;

                            configPRINTF(("[Download] File System update\r\n"));
                        }
                        else
                        {
                            s_msdUpdateType = kMsdUpdateType_App_Bank_Opposite;

                            FICA_GetCurAppStartType(&oldImgType);
                            if (oldImgType == FICA_IMG_TYPE_APP_A)
                            {
                                s_destinationBank = FICA_IMG_TYPE_APP_B;
                            }
                            else
                            {
                                s_destinationBank = FICA_IMG_TYPE_APP_A;
                            }

                            configPRINTF(("[Download] Application update for opposite Bank. Destination Bank %d\r\n", s_destinationBank));
                        }
                    }
                }
                else
                {
                    /* Invalid metadata. Skip */
                    configPRINTF(("Not a metadata size = 0, Skip \r\n"));
                }
            }
            else
            {
                /* Missing metadata, skip packet */
            }
        }
        else
        {
            /* Metadata already received, process incoming data. */
            if (TRANSFER_IDLE == s_transferState)
            {
                /* Rest of offsets coming in will be relative to the start offset */
                s_startOffset     = offset;
                expectedNewOffset = offset;

                if (s_msdUpdateType == kMsdUpdateType_Lfs)
                {
                    if (s_curFileLength <= FICA_FILE_SYS_SIZE)
                    {
                        /* Init FICA to be ready for the new File System */
                        flashError = FICA_app_program_ext_init(s_destinationBank);
                        if (SLN_FLASH_NO_ERROR == flashError)
                        {
                            configPRINTF(("[Download] ---------------------------\r\n"));
                            configPRINTF(("[Download] ----- Updating File System\r\n"));
                            configPRINTF(("[Download] ---------------------------\r\n"));

                            s_transferState = TRANSFER_ACTIVE;
                            isValid         = true;
                        }
                        else
                        {
                            configPRINTF(("[Error] FICA_app_program_ext_init failed %d\r\n", flashError));
                            status           = kStatus_Fail;
                            s_transferState = TRANSFER_ERROR;
                        }
                    }
                    else
                    {
                        {
                            configPRINTF(("[Error] File size %d B is too big for maximum admitted %d B (for File System)\r\n",
                                    s_curFileLength,
                                    FICA_FILE_SYS_SIZE));
                            s_transferState = TRANSFER_ERROR;
                            status           = kStatus_Fail;
                        }
                    }
                }
                else if ((s_msdUpdateType == kMsdUpdateType_App_Bank_Opposite) ||
                         (s_msdUpdateType == kMsdUpdateType_App_Bank_A) ||
                         (s_msdUpdateType == kMsdUpdateType_App_Bank_B))
                {
                    if (s_curFileLength <= FICA_IMG_APP_A_SIZE)
                    {
                        /* Initialize FICA, verify img address is valid */
                        flashError = FICA_initialize();
                        if (SLN_FLASH_NO_ERROR == flashError)
                        {
                            /* Init FICA to be ready for the new application */
                            flashError = FICA_app_program_ext_init(s_destinationBank);
                        }

                        if (SLN_FLASH_NO_ERROR == flashError)
                        {
                            FICA_GetCurAppStartType(&oldImgType);
                            configPRINTF(("[Download] ----------------------------\r\n"));
                            configPRINTF(("[Download] ----- Old image type %d\r\n", oldImgType));
                            configPRINTF(("[Download] ----- New image type %d\r\n", s_destinationBank));
                            configPRINTF(("[Download] ----------------------------\r\n"));

                            s_transferState = TRANSFER_ACTIVE;
                        }
                        else
                        {
                            configPRINTF(("[Error] Unable to begin transfer of file %d\r\n", flashError));
                            status          = kStatus_Fail;
                            s_transferState = TRANSFER_ERROR;
                        }
                    }
                    else
                    {
                        configPRINTF(("[Error] File size %d B is too big for maximum admitted %d B (for Application)\r\n",
                                s_curFileLength,
                                FICA_IMG_APP_A_SIZE));
                        s_transferState = TRANSFER_ERROR;
                        status           = kStatus_Fail;
                    }
                }
                else
                {
                    configPRINTF(("[Error] Unsupported update type %d\r\n", s_msdUpdateType));
                    s_transferState = TRANSFER_ERROR;
                    status           = kStatus_Fail;
                }
            }

            if (TRANSFER_ACTIVE == s_transferState)
            {
                if ((isValid == false) && (offset == s_startOffset))
                {
                    /* LFS type img is always validate in last stage. Only validate the app img*/
                    isValid = MSC_isApplicationValid(size, buffer, &currImgType);

                    if (isValid == true)
                    {
                        configPRINTF(("[Download] Packet has been validated \r\n"));
                    }
                }

                if (offset >= s_startOffset)
                {
                    uint32_t imgOffset      = 0;
                    uint32_t imgBaseAddress = 0;

                    /* Calculate the image address for where to store this data */
                    imgOffset = ((offset - s_startOffset) * s_lbaLength);

                    FICA_GetNewAppStartAddr(&imgBaseAddress);

                    if ((offset < expectedNewOffset) && (isRetransmission == false))
                    {
                        configPRINTF(("[Write Response] Retransmission occurred offset 0x%X, addr 0x%X, \r\n", offset,
                                      (FLEXSPI_AMBA_BASE + imgBaseAddress + imgOffset)));

                        isRetransmission = true;
                    }

                    configPRINTF(
                        ("[Download](%d%s) Saving %d B to 0x%X. Expected offset 0x%X received offset 0x%X \r\n",
                         ((s_dataWritten * 100) / s_curFileLength), "%%", size,
                         (FLEXSPI_AMBA_BASE + imgBaseAddress + imgOffset), expectedNewOffset, offset));

                    /* Write data to flash */
                    flashError = FICA_app_program_ext_abs(imgOffset, buffer, size);
                    if (SLN_FLASH_NO_ERROR == flashError)
                    {
                        static uint32_t cumulatedSize = 0;

                        cumulatedSize += size;

                        if (cumulatedSize >= USB_DISPLAY_SIZE)
                        {
                            /* Add this delay to be able to see the logs */
                            cumulatedSize = 0;
                            vTaskDelay(50);
                        }

                        if (expectedNewOffset == offset)
                        {
                            s_dataWritten += size;
                            if (isRetransmission == true)
                            {
                                configPRINTF(("[Write Response] Retransmission ended \r\n"));
                                isRetransmission = false;
                            }

                            expectedNewOffset = offset + size / s_lbaLength;
                        }
                    }
                    else
                    {
                        configPRINTF(("[Error] Save failed %d\r\n", flashError));
                        status           = kStatus_Fail;
                        s_transferState = TRANSFER_ERROR;
                    }
                }
            }
        }
    }
    else
    {
        configPRINTF(("[Error] Empty write response!\r\n"));
    }

    /* Wake up USB_MSC task to handle the error (if any) */
    if (status != kStatus_Success)
    {
        /* Give MSD task enough time to process previous notification. */
        vTaskDelay(pdMS_TO_TICKS(100));
        vTaskResume(*s_usbAppTaskHandle);
        vTaskSuspend(NULL);
    }

    return status;
}
