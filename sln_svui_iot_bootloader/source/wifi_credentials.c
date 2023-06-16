/*
 * Copyright 2019-2021 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#if 0 //TODO add wifi support
#include "wifi_credentials.h"

#ifdef FFS_ENABLED

#include "wifi_mgr.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"

/* Set to avoid calling aceWifiMgr_ API. Instead, overwrite directly the WiFi profiles KVS entry */
#define DELETE_DIRECTLY_KVS_ENTRY 1

#if DELETE_DIRECTLY_KVS_ENTRY
#include "hal_kv_storage.h"
#include "hal_wifi.h"

/* WARNING: BE SURE THAT THIS STRUCT IS THE SAME WITH THE ONE FROM hal_wifi.c */
typedef struct aceWifiHal_RTOS_config
{
    aceWifiHal_config_t mgrConfig;
    int priority; /**< Priority of the Network */
} aceWifiHal_RTOS_config_t;

/* WARNING: BE SURE THAT THIS VARIABLE IS THE SAME WITH THE ONE FROM hal_wifi.c */
static char *WIFI_KEY_PROFILE = "WifiCred.profile";
#endif

static status_t wifi_credentials_kvs_get(wifi_cred_t *wifi_cred);
static status_t wifi_credentials_kvs_set(wifi_cred_t *wifi_cred);
static status_t wifi_credentials_kvs_present(void);
static status_t wifi_credentials_kvs_reset(void);

#endif

status_t wifi_credentials_present()
{
    status_t status = kStatus_Success;
    status_t status_flash;
    uint32_t credLen = 0;

#ifdef FFS_ENABLED
    if (status == kStatus_Success)
    {
        status = wifi_credentials_kvs_present();
    }
#endif

    if (status == kStatus_Success)
    {
        status_flash = SLN_FLASH_MGMT_Read(WIFI_CRED_FILE_NAME, NULL, &credLen);
        if ((status_flash != SLN_FLASH_MGMT_OK) || (credLen == 0))
        {
            status = kStatus_Fail;
        }
    }

    return status;
}

status_t wifi_credentials_flash_get(wifi_cred_t *wifi_cred)
{
    status_t status;
    status_t status_flash;
    uint32_t credLen;

    if (wifi_cred != NULL)
    {
        memset(wifi_cred, 0, sizeof(wifi_cred_t));
        status = kStatus_Success;
    }
    else
    {
        status = kStatus_Fail;
    }

#ifdef FFS_ENABLED
    if (status == kStatus_Success)
    {
        status = wifi_credentials_kvs_get(wifi_cred);
    }
#endif

    if (status == kStatus_Success)
    {
        credLen      = sizeof(wifi_cred_t);
        status_flash = SLN_FLASH_MGMT_Read(WIFI_CRED_FILE_NAME, (uint8_t *)wifi_cred, &credLen);
        if ((status_flash != SLN_FLASH_MGMT_OK) && (status_flash != SLN_FLASH_MGMT_ENOENTRY) &&
            (status_flash != SLN_FLASH_MGMT_ENOENTRY2))
        {
            status = kStatus_Fail;
        }
    }

    return status;
}

status_t wifi_credentials_flash_set(wifi_cred_t *wifi_cred)
{
    status_t status;
    status_t status_flash;

    if (wifi_cred != NULL)
    {
        status = kStatus_Success;
    }
    else
    {
        status = kStatus_Fail;
    }

#ifdef FFS_ENABLED
    if (status == kStatus_Success)
    {
        status = wifi_credentials_kvs_set(wifi_cred);
    }
#endif

    if (status == kStatus_Success)
    {
        status_flash = SLN_FLASH_MGMT_Save(WIFI_CRED_FILE_NAME, (uint8_t *)wifi_cred, sizeof(wifi_cred_t));
        if ((status_flash == SLN_FLASH_MGMT_EOVERFLOW) || (status_flash == SLN_FLASH_MGMT_EOVERFLOW2))
        {
            status_flash = SLN_FLASH_MGMT_Erase(WIFI_CRED_FILE_NAME);
            status_flash = SLN_FLASH_MGMT_Save(WIFI_CRED_FILE_NAME, (uint8_t *)wifi_cred, sizeof(wifi_cred_t));
            if (status_flash != SLN_FLASH_MGMT_OK)
            {
                status = kStatus_Fail;
            }
        }
        else if (status_flash != SLN_FLASH_MGMT_OK)
        {
            status = kStatus_Fail;
        }
    }

    return status;
}

status_t wifi_credentials_flash_reset(void)
{
    status_t status = kStatus_Success;
    status_t status_flash;
    uint8_t buf[sizeof(wifi_cred_t)] = {0};

#ifdef FFS_ENABLED
    status = wifi_credentials_kvs_reset();
#endif

    if (status == kStatus_Success)
    {
        SLN_FLASH_MGMT_Erase(WIFI_CRED_FILE_NAME);
        status_flash = SLN_FLASH_MGMT_Save(WIFI_CRED_FILE_NAME, buf, sizeof(wifi_cred_t));
        if (status_flash == SLN_FLASH_MGMT_OK)
        {
            status = kStatus_Success;
        }
        else
        {
            status = kStatus_Fail;
        }
    }

    return status;
}

status_t check_valid_credentials(wifi_cred_t *wifi_cred)
{
    status_t status;
    uint8_t i;

    if (wifi_cred != NULL)
    {
        status = kStatus_Success;
    }
    else
    {
        status = kStatus_Fail;
    }

    if (status == kStatus_Success)
    {
        if (wifi_cred->ssid.length > 0)
        {
            for (i = 0; i < wifi_cred->ssid.length; i++)
            {
                if ((wifi_cred->ssid.value[i] == 0x00) || (wifi_cred->ssid.value[i] == 0xFF))
                {
                    status = kStatus_Fail;
                    break;
                }
            }
        }
        else
        {
            status = kStatus_Fail;
        }
    }

    if (status == kStatus_Success)
    {
        if (strlen((char *)(wifi_cred->ssid.value)) == 0)
        {
            status = kStatus_Fail;
        }
    }

    return status;
}

#ifdef FFS_ENABLED

status_t wifi_credentials_sync(void)
{
    status_t status;
    status_t status_flash;
    wifi_cred_t wifi_cred_kvs   = {0};
    wifi_cred_t wifi_cred_flash = {0};
    uint32_t credLen;
    uint8_t dif_wifi_cred = 0;

    status = wifi_credentials_kvs_get(&wifi_cred_kvs);

    if (status == kStatus_Success)
    {
        credLen      = sizeof(wifi_cred_t);
        status_flash = SLN_FLASH_MGMT_Read(WIFI_CRED_FILE_NAME, (uint8_t *)&wifi_cred_flash, &credLen);
        if (status_flash == SLN_FLASH_MGMT_OK)
        {
            if (memcmp(&wifi_cred_flash, &wifi_cred_kvs, sizeof(wifi_cred_t)) == 0)
            {
                dif_wifi_cred = 0;
            }
            else
            {
                dif_wifi_cred = 1;
            }
        }
        else
        {
            dif_wifi_cred = 1;
        }
    }

    if (status == kStatus_Success)
    {
        if (dif_wifi_cred == 1)
        {
            status_flash = SLN_FLASH_MGMT_Save(WIFI_CRED_FILE_NAME, (uint8_t *)&wifi_cred_kvs, sizeof(wifi_cred_t));
            if ((status_flash == SLN_FLASH_MGMT_EOVERFLOW) || (status_flash == SLN_FLASH_MGMT_EOVERFLOW2))
            {
                status_flash = SLN_FLASH_MGMT_Erase(WIFI_CRED_FILE_NAME);
                status_flash = SLN_FLASH_MGMT_Save(WIFI_CRED_FILE_NAME, (uint8_t *)&wifi_cred_kvs, sizeof(wifi_cred_t));
                if (status_flash != SLN_FLASH_MGMT_OK)
                {
                    dif_wifi_cred = 1;
                    status        = kStatus_Fail;
                }
            }
            else if (status_flash != SLN_FLASH_MGMT_OK)
            {
                status = kStatus_Fail;
            }
        }
    }

    return status;
}

static status_t wifi_credentials_kvs_present()
{
    status_t status = kStatus_Success;
    aceWifiMgr_error_t wifi_mgr_status;
    aceWifiMgr_configList_t networks = {0};

    wifi_mgr_status = aceWifiMgr_getConfiguredNetworks(&networks);
    if (wifi_mgr_status != aceWifiMgr_ERROR_SUCCESS)
    {
        status = kStatus_Fail;
    }
    else if (networks.length == 0)
    {
        status = kStatus_Fail;
    }

    return status;
}

static status_t wifi_credentials_kvs_get(wifi_cred_t *wifi_cred)
{
    status_t status;
    aceWifiMgr_error_t wifi_mgr_status;
    aceWifiMgr_configList_t *networks = NULL;

    if (wifi_cred != NULL)
    {
        status = kStatus_Success;
    }
    else
    {
        status = kStatus_Fail;
    }

    if (status == kStatus_Success)
    {
        networks = (aceWifiMgr_configList_t *)pvPortMalloc(sizeof(aceWifiMgr_configList_t));
        if (networks != NULL)
        {
            memset(networks, 0, sizeof(aceWifiMgr_configList_t));
        }
        else
        {
            status = kStatus_Fail;
        }
    }

    if (status == kStatus_Success)
    {
        memset(wifi_cred, 0, sizeof(wifi_cred_t));

        wifi_mgr_status = aceWifiMgr_getConfiguredNetworks(networks);
        if (wifi_mgr_status != aceWifiMgr_ERROR_SUCCESS)
        {
            status = kStatus_Fail;
        }
    }

    if (status == kStatus_Success)
    {
        if (networks->length > 0)
        {
            if ((networks->list[0].ssidLength > MAX_SSID_LEN) || (networks->list[0].pskLength >= KEY_MAX_LEN))
            {
                status = kStatus_Fail;
            }
            else
            {
                wifi_cred->ssid.length = networks->list[0].ssidLength;
                memcpy(wifi_cred->ssid.value, networks->list[0].ssid, networks->list[0].ssidLength);

                wifi_cred->password.length = networks->list[0].pskLength;
                memcpy(wifi_cred->password.value, networks->list[0].psk, networks->list[0].pskLength);
                wifi_cred->password.value[wifi_cred->password.length] = '\0';
            }
        }
        else
        {
            wifi_cred->ssid.length       = 0;
            wifi_cred->ssid.value[0]     = '\0';
            wifi_cred->password.length   = 0;
            wifi_cred->password.value[0] = '\0';
        }
    }

    if (networks != NULL)
    {
        vPortFree(networks);
    }

    return status;
}

static status_t wifi_credentials_kvs_set(wifi_cred_t *wifi_cred)
{
    status_t status;
    aceWifiMgr_error_t wifi_mgr_status;
    aceWifiMgr_config_t *config       = NULL;
    aceWifiMgr_configList_t *networks = NULL;

    /* Stores the WiFi SSID as a printable string. Added one extra byte for the NULL terminator. */
    char ssid_str[MAX_SSID_LEN + 1] = {0};

    if (wifi_cred != NULL)
    {
        status = kStatus_Success;
    }
    else
    {
        status = kStatus_Fail;
    }

    if (status == kStatus_Success)
    {
        config = (aceWifiMgr_config_t *)pvPortMalloc(sizeof(aceWifiMgr_config_t));
        if (config != NULL)
        {
            memset(config, 0, sizeof(aceWifiMgr_config_t));
        }
        else
        {
            status = kStatus_Fail;
        }
    }

    if (status == kStatus_Success)
    {
        networks = (aceWifiMgr_configList_t *)pvPortMalloc(sizeof(aceWifiMgr_configList_t));
        if (networks != NULL)
        {
            memset(networks, 0, sizeof(aceWifiMgr_configList_t));
        }
        else
        {
            status = kStatus_Fail;
        }
    }

    if (status == kStatus_Success)
    {
        if ((wifi_cred->ssid.length > 0) && (wifi_cred->ssid.length <= MAX_SSID_LEN))
        {
            if (wifi_cred->password.length == 0)
            {
                config->ssidLength = wifi_cred->ssid.length;
                memcpy(config->ssid, wifi_cred->ssid.value, wifi_cred->ssid.length);

                config->pskLength = wifi_cred->password.length;
                memcpy(config->psk, wifi_cred->password.value, wifi_cred->password.length);
                config->psk[config->pskLength] = '\0';

                config->authMode = aceWifiMgr_AUTH_OPEN;
            }
            else if ((wifi_cred->password.length >= KEY_MIN_LEN) && (wifi_cred->password.length < KEY_MAX_LEN))
            {
                config->ssidLength = wifi_cred->ssid.length;
                memcpy(config->ssid, wifi_cred->ssid.value, wifi_cred->ssid.length);

                config->pskLength = wifi_cred->password.length;
                memcpy(config->psk, wifi_cred->password.value, wifi_cred->password.length);
                config->psk[config->pskLength] = '\0';

                config->authMode = aceWifiMgr_AUTH_WPA2;
            }
            else
            {
                status = kStatus_Fail;
            }
        }
        else
        {
            status = kStatus_Fail;
        }
    }

    /* Make room for a new WiFi profile (in case the profiles array is full) */
    if (status == kStatus_Success)
    {
        wifi_mgr_status = aceWifiMgr_getConfiguredNetworks(networks);
        if (wifi_mgr_status == aceWifiMgr_ERROR_SUCCESS)
        {
            if ((networks->length > 0) && (networks->length >= ACE_WIFI_MAX_CONFIGURED_NETWORKS))
            {
                memset(ssid_str, 0, MAX_SSID_LEN + 1);
                memcpy(ssid_str, networks->list[0].ssid, MAX_SSID_LEN);
                wifi_mgr_status = aceWifiMgr_removeNetwork(ssid_str, networks->list[0].authMode);
                if (wifi_mgr_status != aceWifiMgr_ERROR_SUCCESS)
                {
                    status = kStatus_Fail;
                }
            }
        }
        else
        {
            status = kStatus_Fail;
        }
    }

    if (status == kStatus_Success)
    {
        wifi_mgr_status = aceWifiMgr_addNetwork(config);
        if (wifi_mgr_status != aceWifiMgr_ERROR_SUCCESS)
        {
            status = kStatus_Fail;
        }
    }

    if (status == kStatus_Success)
    {
        wifi_mgr_status = aceWifiMgr_saveConfig();
        if (wifi_mgr_status != aceWifiMgr_ERROR_SUCCESS)
        {
            status = kStatus_Fail;
        }
    }

    if (config != NULL)
    {
        vPortFree(config);
    }

    if (networks != NULL)
    {
        vPortFree(networks);
    }

    return status;
}

static status_t wifi_credentials_kvs_reset(void)
{
    status_t status;

#if DELETE_DIRECTLY_KVS_ENTRY
    ace_status_t kvs_status;
    aceWifiHal_RTOS_config_t *profile_array = NULL;
    uint32_t profile_array_size             = ACE_WIFI_MAX_CONFIGURED_NETWORKS * sizeof(aceWifiHal_RTOS_config_t);

    if (profile_array_size > 0)
    {
        status = kStatus_Success;
    }
    else
    {
        status = kStatus_Fail;
    }

    if (status == kStatus_Success)
    {
        profile_array = (aceWifiHal_RTOS_config_t *)pvPortMalloc(profile_array_size);
        if (profile_array != NULL)
        {
            memset(profile_array, 0, profile_array_size);
        }
        else
        {
            status = kStatus_Fail;
        }
    }

    if (status == kStatus_Success)
    {
        kvs_status = aceKeyValueDsHal_set(WIFI_KEY_PROFILE, profile_array, profile_array_size);
        if (kvs_status != ACE_STATUS_OK)
        {
            status = kStatus_Fail;
        }
    }

    if (profile_array != NULL)
    {
        vPortFree(profile_array);
    }
#else
    aceWifiMgr_error_t wifi_mgr_status;
    uint16_t network_id;
    aceWifiMgr_configList_t *networks = NULL;

    /* Stores the WiFi SSID as a printable string. Added one extra byte for the NULL terminator. */
    char ssid_str[MAX_SSID_LEN + 1] = {0};

    networks = (aceWifiMgr_configList_t *)pvPortMalloc(sizeof(aceWifiMgr_configList_t));
    if (networks != NULL)
    {
        memset(networks, 0, sizeof(aceWifiMgr_configList_t));
        status = kStatus_Success;
    }
    else
    {
        status = kStatus_Fail;
    }

    if (status == kStatus_Success)
    {
        wifi_mgr_status = aceWifiMgr_getConfiguredNetworks(networks);
        if (wifi_mgr_status == aceWifiMgr_ERROR_SUCCESS)
        {
            status = kStatus_Success;
        }
        else
        {
            status = kStatus_Fail;
        }
    }

    if (status == kStatus_Success)
    {
        for (network_id = 0; network_id < networks->length; network_id++)
        {
            memset(ssid_str, 0, MAX_SSID_LEN + 1);
            memcpy(ssid_str, networks->list[network_id].ssid, MAX_SSID_LEN);
            wifi_mgr_status = aceWifiMgr_removeNetwork(ssid_str, networks->list[network_id].authMode);
            if (wifi_mgr_status != aceWifiMgr_ERROR_SUCCESS)
            {
                status = kStatus_Fail;
                break;
            }
        }
    }

    if (status == kStatus_Success)
    {
        wifi_mgr_status = aceWifiMgr_saveConfig();
        if (wifi_mgr_status != aceWifiMgr_ERROR_SUCCESS)
        {
            status = kStatus_Fail;
        }
    }

    if (networks != NULL)
    {
        vPortFree(networks);
    }

#endif

    return status;
}

#endif /* FFS_ENABLED */
#endif
