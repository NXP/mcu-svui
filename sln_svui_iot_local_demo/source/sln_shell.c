/*
 * Copyright 2018-2023 NXP.
 * NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly
 * in accordance with the applicable license terms. By expressly accepting such terms or by downloading,
 * installing, activating and/or otherwise using the software, you are agreeing that you have read, and
 * that you agree to comply with and are bound by, such license terms. If you do not agree to be bound by
 * the applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "sln_shell.h"
#include "IndexCommands.h"
#include "sln_local_voice_common.h"
#include "sln_app_fwupdate.h"
#include "ctype.h"
#include "sln_flash_fs_ops.h"
#include "flash_ica_driver.h"

#if ENABLE_AUDIO_DUMP
#include "composite.h"
#include "audio_dump.h"
#endif /* ENABLE_AUDIO_DUMP */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#if (defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))
#define SLN_SERIAL_MANAGER_RECEIVE_BUFFER_LEN 2048U
#endif

#define BANK_A_STRING "BankA"
#define BANK_B_STRING "BankB"
#define BANK_U_STRING "BankUnknown"

/* Max number of arguments for a shell command */
#define MAX_ARGC          4
/* Max argv string size, string terminator included */
#define MAX_ARGV_STR_SIZE 16

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
extern void *pvPortCalloc(size_t nmemb, size_t xSize);

#if USE_WIFI_CONNECTION
static shell_status_t sln_print_handler(shell_handle_t shellHandle, int32_t argc, char **argv);
static shell_status_t sln_erase_handler(shell_handle_t shellHandle, int32_t argc, char **argv);
static shell_status_t sln_setup_handler(shell_handle_t shellHandle, int32_t argc, char **argv);
#endif
static shell_status_t sln_reset_handler(shell_handle_t shellHandle, int32_t argc, char **argv);
static shell_status_t sln_factoryreset_handler(shell_handle_t shellHandle, int32_t argc, char **argv);
static shell_status_t sln_changebank_handler(shell_handle_t shellHandle, int32_t argc, char **argv);
static shell_status_t sln_commands_handler(shell_handle_t shellHandle, int32_t argc, char **argv);
static shell_status_t sln_changedemo_handler(shell_handle_t shellHandle, int32_t argc, char **argv);
static shell_status_t sln_volume_handler(shell_handle_t shellHandle, int32_t argc, char **argv);
static shell_status_t sln_mute_handler(shell_handle_t shellHandle, int32_t argc, char **argv);
static shell_status_t sln_timeout_handler(shell_handle_t shellHandle, int32_t argc, char **argv);
static shell_status_t sln_followup_handler(shell_handle_t shellHandle, int32_t argc, char **argv);
static shell_status_t sln_changelang_handler(shell_handle_t shellHandle, int32_t argc, char **argv);
static shell_status_t sln_ptt_handler(shell_handle_t shellHandle, int32_t argc, char **argv);
#if ENABLE_DSMT_ASR
static shell_status_t sln_cmdresults_handler(shell_handle_t shellHandle, int32_t argc, char **argv);
#endif /* ENABLE_DSMT_ASR */
static shell_status_t sln_updateotw_handler(shell_handle_t shellHandle, int32_t argc, char **argv);
#if USE_WIFI_CONNECTION
static shell_status_t sln_updateota_handler(shell_handle_t shellHandle, int32_t argc, char **argv);
#endif
static shell_status_t sln_version_handler(shell_handle_t shellHandle, int32_t argc, char **argv);
#if ENABLE_AUDIO_DUMP && ENABLE_AMPLIFIER
static shell_status_t sln_usb_aec_align_handler(shell_handle_t shellHandle, int32_t argc, char **argv);
#endif /* ENABLE_AUDIO_DUMP && ENABLE_AMPLIFIER */
#if SLN_TRACE_CPU_USAGE
static shell_status_t sln_trace_cpu_usage_handler(shell_handle_t shellHandle, int32_t argc, char **argv);
#endif /* SLN_TRACE_CPU_USAGE */

/*******************************************************************************
 * Variables
 ******************************************************************************/
#if USE_WIFI_CONNECTION
SHELL_COMMAND_DEFINE(print,
                     "\r\n\"print\": Print the WiFi Network Credentials currently stored in flash.\r\n",
                     sln_print_handler,
                     0);

SHELL_COMMAND_DEFINE(erase,
                     "\r\n\"erase\": Erase the current WiFi Network credentials from flash.\r\n",
                     sln_erase_handler,
                     0);

SHELL_COMMAND_DEFINE(setup,
                     "\r\n\"setup\": Setup the WiFi Network Credentials\r\n"
                     "         Usage:\r\n"
                     "             setup SSID [PASSWORD] \r\n"
                     "         Parameters:\r\n"
                     "         SSID:       The wireless network name\r\n"
                     "         PASSWORD:   The password for the wireless network\r\n"
                     "                     For open networks it is not needed\r\n",
                     /* if more than two parameters, it'll take just the first two of them */
                     sln_setup_handler,
                     SHELL_IGNORE_PARAMETER_COUNT);
#endif
SHELL_COMMAND_DEFINE(reset, "\r\n\"reset\": Resets the MCU.\r\n", sln_reset_handler, 0);

SHELL_COMMAND_DEFINE(factoryreset, "\r\n\"factoryreset\": Reset the board to the original settings\r\n", sln_factoryreset_handler, 0);

SHELL_COMMAND_DEFINE(changebank,
                     "\r\n\"changebank\": Change the active bank. If Bank A is active, this command will change the boot settings to Bank B and vice versa.\r\n",
                     sln_changebank_handler,
                     0);

SHELL_COMMAND_DEFINE(commands,
                     "\r\n\"commands\": List available voice commands for selected demo.\r\n",
                     sln_commands_handler,
                     0);

SHELL_COMMAND_DEFINE(changedemo,
                     "\r\n\"changedemo\": Change the command set. Save in flash memory.\r\n"
                     "         Usage:\r\n"
                     "            changedemo <param> \r\n"
                     "         Parameters\r\n"
                     "            one of the following demos: "
                     SHELL_SELECTABLE_DEMOS
                     "\r\n",
                     sln_changedemo_handler,
                     1);

SHELL_COMMAND_DEFINE(changelang,
                     "\r\n\"changelang\": Change language(s). Save in flash memory.\r\n"
                     "         Usage:\r\n"
#if MULTILINGUAL
                     "            changelang language_code1 up to language_code4\r\n"
#else
                     "            changelang language_code\r\n"
#endif
                     "            when called without parameters, it will display the current active language(s)\r\n"
                     "         Parameters\r\n"
#if MULTILINGUAL
                     "            one or more of the following languages, separated by spaces: "
#else
                     "            only one of the following languages: "
#endif
                     SHELL_SELECTABLE_LANGUAGES
                     "\r\n",
                     sln_changelang_handler,
                     SHELL_IGNORE_PARAMETER_COUNT);

SHELL_COMMAND_DEFINE(volume,
                     "\r\n\"volume\": Set speaker volume (0 - 100). Save in flash memory.\r\n"
                     "         Usage:\r\n"
                     "            volume N \r\n"
                     "         Parameters\r\n"
                     "            N between 0 and 100\r\n",
                     sln_volume_handler,
                     1);

SHELL_COMMAND_DEFINE(mute,
                     "\r\n\"mute\": Set microphones state (on / off). Save in flash memory.\r\n"
                     "         Usage:\r\n"
                     "            mute on (or off)\r\n"
                     "         Parameters\r\n"
                     "            on or off\r\n",
                     sln_mute_handler,
                     1);

SHELL_COMMAND_DEFINE(timeout,
                     "\r\n\"timeout\": Set command waiting time (in ms). Save in flash memory.\r\n"
                     "         Usage:\r\n"
                     "            timeout N \r\n"
                     "         Parameters\r\n"
                     "            N milliseconds\r\n",
                     sln_timeout_handler,
                     1);

SHELL_COMMAND_DEFINE(followup,
                     "\r\n\"followup\": Set follow-up mode (on / off). Save in flash memory.\r\n"
                     "         Usage:\r\n"
                     "            followup on (or off) \r\n"
                     "         Parameters\r\n"
                     "            on or off\r\n",
                     sln_followup_handler,
                     1);

SHELL_COMMAND_DEFINE(ptt,
                     "\r\n\"ptt\": Set push-to-talk mode (on / off). Save in flash memory.\r\n"
                     "         Usage:\r\n"
                     "            ptt on (or off) \r\n"
                     "         Parameters\r\n"
                     "            on or off\r\n",
                     sln_ptt_handler,
                     1);

#if ENABLE_DSMT_ASR
SHELL_COMMAND_DEFINE(cmdresults,
                     "\r\n\"cmdresults\": Print the command detection results in console.\r\n"
                     "         Usage:\r\n"
                     "            cmdresults on (or off) \r\n"
                     "         Parameters\r\n"
                     "            on or off\r\n",
                     sln_cmdresults_handler,
                     1);
#endif

SHELL_COMMAND_DEFINE(updateotw,
                     "\r\n\"updateotw\": Restarts the board in the OTW update mode.\r\n",
                     sln_updateotw_handler,
                     0);

#if USE_WIFI_CONNECTION
SHELL_COMMAND_DEFINE(updateota,
                     "\r\n\"updateota\": Restarts the board in the OTA update mode.\r\n",
                     sln_updateota_handler,
                     0);
#endif

SHELL_COMMAND_DEFINE(version, "\r\n\"version\": Print firmware version\r\n", sln_version_handler, 0);

#if ENABLE_AUDIO_DUMP && ENABLE_AMPLIFIER
SHELL_COMMAND_DEFINE(usb_aec_align,
                     "\r\n\"usb_aec_align\": Starts playing the aec alignment sound.\r\n",
                     sln_usb_aec_align_handler,
                     0);
#endif /* ENABLE_AUDIO_DUMP && ENABLE_AMPLIFIER */

#if SLN_TRACE_CPU_USAGE
SHELL_COMMAND_DEFINE(cpu_view, "\r\n\"cpu_view\": Print the CPU usage info\r\n", sln_trace_cpu_usage_handler, 0);
#endif /* SLN_TRACE_CPU_USAGE */

extern app_asr_shell_commands_t appAsrShellCommands;
extern TaskHandle_t appTaskHandle;

static uint8_t s_shellHandleBuffer[SHELL_HANDLE_SIZE];
static shell_handle_t s_shellHandle;

static uint8_t s_serialHandleBuffer[SERIAL_MANAGER_HANDLE_SIZE];
static serial_handle_t s_serialHandle = &s_serialHandleBuffer[0];

#if (defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))
__attribute__((section(".bss.$SRAM_OC_NON_CACHEABLE"))) __attribute__((aligned(8)))
uint8_t readRingBuffer[SLN_SERIAL_MANAGER_RECEIVE_BUFFER_LEN];
#endif

static EventGroupHandle_t s_ShellEventGroup;
#if USE_WIFI_CONNECTION
static wifi_cred_t s_wifi_cred = {0};
#endif
static TaskHandle_t s_appInitTask      = NULL;
static shell_heap_trace_t s_heap_trace = {0};

static uint8_t s_argc = 0;
static char s_argv[MAX_ARGC + 1][MAX_ARGV_STR_SIZE] = {0};

extern oob_demo_control_t oob_demo_control;

/*******************************************************************************
 * Code
 ******************************************************************************/

/* Get current running bank */
static char *getActiveBank(void)
{
    char *activeBank = BANK_U_STRING;

#if ENABLE_REMAP
    /* If remap is enabled find the current running bank by checking
     * the remap offset register */
    if (REMAP_DISABLED_OFFSET == IOMUXC_GPR->GPR32)
    {
        activeBank = BANK_A_STRING;
    }
    else if (REMAP_ENABLED_OFFSET == IOMUXC_GPR->GPR32)
    {
        activeBank = BANK_B_STRING;
    }
#else
    /* If remap is not enabled, find the current running bank by checking
     * the ResetISR Address in the vector table (which is loaded into DTC). */
    uint32_t runningFromBankA =
        (((*(uint32_t *)(APPLICATION_RESET_ISR_ADDRESS)) & APP_VECTOR_TABLE_APP_A) == APP_VECTOR_TABLE_APP_A);
    uint32_t runningFromBankb =
        (((*(uint32_t *)(APPLICATION_RESET_ISR_ADDRESS)) & APP_VECTOR_TABLE_APP_B) == APP_VECTOR_TABLE_APP_B);

    if (runningFromBankA)
    {
        activeBank = BANK_A_STRING;
    }
    else if (runningFromBankb)
    {
        activeBank = BANK_B_STRING;
    }
#endif /* ENABLE_REMAP */

    return activeBank;
}

static shell_status_t isNumber(char *arg)
{
    int32_t status = kStatus_SHELL_Success;
    uint32_t i;

    for (i = 0; arg[i] != '\0'; i++)
    {
        if (!isdigit((unsigned char)arg[i]))
        {
            status = kStatus_SHELL_Error;
        }
    }

    return status;
}

static void USB_DeviceClockInit(void)
{
#if defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 0U)
    usb_phy_config_struct_t phyConfig = {
        BOARD_USB_PHY_D_CAL,
        BOARD_USB_PHY_TXCAL45DP,
        BOARD_USB_PHY_TXCAL45DM,
    };
#endif
#if defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 0U)
    if (CONTROLLER_ID == kSerialManager_UsbControllerEhci0)
    {
        CLOCK_EnableUsbhs0PhyPllClock(kCLOCK_Usbphy480M, 480000000U);
        CLOCK_EnableUsbhs0Clock(kCLOCK_Usb480M, 480000000U);
    }
    else
    {
        CLOCK_EnableUsbhs1PhyPllClock(kCLOCK_Usbphy480M, 480000000U);
        CLOCK_EnableUsbhs1Clock(kCLOCK_Usb480M, 480000000U);
    }
    USB_EhciPhyInit(CONTROLLER_ID, BOARD_XTAL0_CLK_HZ, &phyConfig);
#endif
}

static shell_status_t sln_reset_handler(shell_handle_t shellHandle, int32_t argc, char **argv)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xEventGroupSetBitsFromISR(s_ShellEventGroup, RESET_EVENT, &xHigherPriorityTaskWoken);

    return kStatus_SHELL_Success;
}

static shell_status_t sln_factoryreset_handler(shell_handle_t shellHandle, int32_t argc, char **argv)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xEventGroupSetBitsFromISR(s_ShellEventGroup, FACTORY_RESET_EVT, &xHigherPriorityTaskWoken);

    return kStatus_SHELL_Success;
}

static shell_status_t sln_changebank_handler(shell_handle_t shellHandle, int32_t argc, char **argv)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xEventGroupSetBitsFromISR(s_ShellEventGroup, CHANGE_BANK_EVT, &xHigherPriorityTaskWoken);

    return kStatus_SHELL_Success;
}

static shell_status_t sln_commands_handler(shell_handle_t shellHandle, int32_t argc, char **argv)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xEventGroupSetBitsFromISR(s_ShellEventGroup, PRINT_COMMANDS_EVT, &xHigherPriorityTaskWoken);

    return kStatus_SHELL_Success;
}

static shell_status_t sln_changedemo_handler(shell_handle_t shellHandle, int32_t argc, char **argv)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    s_argc = argc;
    strncpy(s_argv[1], argv[1], MAX_ARGV_STR_SIZE);

    xEventGroupSetBitsFromISR(s_ShellEventGroup, CHANGE_DEMO_EVT, &xHigherPriorityTaskWoken);

    return kStatus_SHELL_Success;
}

static shell_status_t sln_changelang_handler(shell_handle_t shellHandle, int32_t argc, char **argv)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    s_argc = argc;
    if (s_argc <= MAX_ARGC)
    {
        for (uint8_t i = 0; i < argc; i++)
        {
            strncpy(s_argv[i], argv[i], MAX_ARGV_STR_SIZE);
        }
    }

    xEventGroupSetBitsFromISR(s_ShellEventGroup, CHANGE_LANG_EVT, &xHigherPriorityTaskWoken);

    return kStatus_SHELL_Success;
}

static shell_status_t sln_volume_handler(shell_handle_t shellHandle, int32_t argc, char **argv)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    s_argc = argc;
    strncpy(s_argv[1], argv[1], MAX_ARGV_STR_SIZE);

    xEventGroupSetBitsFromISR(s_ShellEventGroup, SET_VOLUME_EVT, &xHigherPriorityTaskWoken);

    return kStatus_SHELL_Success;
}

static shell_status_t sln_followup_handler(shell_handle_t shellHandle, int32_t argc, char **argv)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    s_argc = argc;
    strncpy(s_argv[1], argv[1], MAX_ARGV_STR_SIZE);

    xEventGroupSetBitsFromISR(s_ShellEventGroup, FOLLOWUP_EVT, &xHigherPriorityTaskWoken);

    return kStatus_SHELL_Success;
}

static shell_status_t sln_timeout_handler(shell_handle_t shellHandle, int32_t argc, char **argv)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    s_argc = argc;
    strncpy(s_argv[1], argv[1], MAX_ARGV_STR_SIZE);

    xEventGroupSetBitsFromISR(s_ShellEventGroup, SET_TIMEOUT_EVT, &xHigherPriorityTaskWoken);

    return kStatus_SHELL_Success;
}

static shell_status_t sln_mute_handler(shell_handle_t shellHandle, int32_t argc, char **argv)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    s_argc = argc;
    strncpy(s_argv[1], argv[1], MAX_ARGV_STR_SIZE);

    xEventGroupSetBitsFromISR(s_ShellEventGroup, MUTE_EVT, &xHigherPriorityTaskWoken);

    return kStatus_SHELL_Success;
}

#if SLN_TRACE_CPU_USAGE
static shell_status_t sln_trace_cpu_usage_handler(shell_handle_t shellHandle, int32_t argc, char **argv)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xEventGroupSetBitsFromISR(s_ShellEventGroup, TRACE_CPU_USAGE_EVT, &xHigherPriorityTaskWoken);

    return kStatus_SHELL_Success;
}
#endif /* SLN_TRACE_CPU_USAGE */

static shell_status_t sln_ptt_handler(shell_handle_t shellHandle, int32_t argc, char **argv)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    s_argc = argc;
    strncpy(s_argv[1], argv[1], MAX_ARGV_STR_SIZE);

    xEventGroupSetBitsFromISR(s_ShellEventGroup, PTT_EVT, &xHigherPriorityTaskWoken);

    return kStatus_SHELL_Success;
}

#if ENABLE_DSMT_ASR
static shell_status_t sln_cmdresults_handler(shell_handle_t shellHandle, int32_t argc, char **argv)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    s_argc = argc;
    strncpy(s_argv[1], argv[1], MAX_ARGV_STR_SIZE);

    xEventGroupSetBitsFromISR(s_ShellEventGroup, PRINT_CMD_RES_EVT, &xHigherPriorityTaskWoken);

    return kStatus_SHELL_Success;
}
#endif /* ENABLE_DSMT_ASR */

#if USE_WIFI_CONNECTION
static shell_status_t sln_print_handler(shell_handle_t shellHandle, int32_t argc, char **argv)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xEventGroupSetBitsFromISR(s_ShellEventGroup, PRINT_EVENT, &xHigherPriorityTaskWoken);

    return kStatus_SHELL_Success;
}

static shell_status_t sln_erase_handler(shell_handle_t shellHandle, int32_t argc, char **argv)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xEventGroupSetBitsFromISR(s_ShellEventGroup, ERASE_EVENT, &xHigherPriorityTaskWoken);

    return kStatus_SHELL_Success;
}

static shell_status_t sln_setup_handler(shell_handle_t shellHandle, int32_t argc, char **argv)
{
    int32_t status = kStatus_SHELL_Success;

    if (argc == 1 || argc > 3)
    {
        SHELL_Printf(
            s_shellHandle,
            "\r\nIncorrect command parameter(s). Enter \"help\" to view a list of available commands.\r\n\r\n");
        status = kStatus_SHELL_Error;
    }
    else
    {
        char *kWiFiName     = NULL;
        char *kWiFiPassword = NULL;

        kWiFiName = argv[1];
        if (argc > 2)
        {
            kWiFiPassword = argv[2];
        }

        uint32_t name_len = strlen(kWiFiName);
        uint32_t pass_len = kWiFiPassword ? strlen(kWiFiPassword) : 0;

        if (name_len == 0)
        {
            SHELL_Printf(
                s_shellHandle,
                "\r\nIncorrect command parameter(s). Enter \"help\" to view a list of available commands.\r\n\r\n");
            status = kStatus_SHELL_Error;
        }

        if (name_len <= sizeof(s_wifi_cred.ssid.value))
        {
            memcpy(s_wifi_cred.ssid.value, kWiFiName, name_len);
            s_wifi_cred.ssid.length = name_len;
        }
        else
        {
            status = kStatus_SHELL_Error;
        }

        if (pass_len + 1 <= sizeof(s_wifi_cred.password.value))
        {
            if (pass_len != 0)
            {
                memcpy(s_wifi_cred.password.value, kWiFiPassword, pass_len + 1);
            }
            else
            {
                s_wifi_cred.password.value[0] = '\0';
            }
            s_wifi_cred.password.length = pass_len;
        }
        else
        {
            status = kStatus_SHELL_Error;
        }

        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xEventGroupSetBitsFromISR(s_ShellEventGroup, SETUP_EVENT, &xHigherPriorityTaskWoken);
    }

    return status;
}
#endif

#if USE_WIFI_CONNECTION
static shell_status_t sln_updateota_handler(shell_handle_t shellHandle, int32_t argc, char **argv)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xEventGroupSetBitsFromISR(s_ShellEventGroup, OTA_UPDATE_EVT, &xHigherPriorityTaskWoken);

    return kStatus_SHELL_Success;
}
#endif

static shell_status_t sln_updateotw_handler(shell_handle_t shellHandle, int32_t argc, char **argv)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xEventGroupSetBitsFromISR(s_ShellEventGroup, OTW_UPDATE_EVT, &xHigherPriorityTaskWoken);

    return kStatus_SHELL_Success;
}

static shell_status_t sln_version_handler(shell_handle_t shellHandle, int32_t argc, char **argv)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xEventGroupSetBitsFromISR(s_ShellEventGroup, VERSION_EVT, &xHigherPriorityTaskWoken);

    return kStatus_SHELL_Success;
}

#if ENABLE_AUDIO_DUMP && ENABLE_AMPLIFIER
static shell_status_t sln_usb_aec_align_handler(shell_handle_t shellHandle, int32_t argc, char **argv)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xEventGroupSetBitsFromISR(s_ShellEventGroup, AUDIO_DUMP_TEST_EVT, &xHigherPriorityTaskWoken);

    return kStatus_SHELL_Success;
}
#endif /* ENABLE_AUDIO_DUMP && ENABLE_AMPLIFIER */

int log_shell_printf(const char *formatString, ...)
{
    va_list ap;
    char logbuf[configLOGGING_MAX_MESSAGE_LENGTH] = {0};

    va_start(ap, formatString);
    vsnprintf(logbuf, configLOGGING_MAX_MESSAGE_LENGTH, formatString, ap);

    va_end(ap);

    SHELL_Write(s_shellHandle, logbuf, strlen(logbuf));

    return 0;
}

int sln_shell_init(void)
{
    status_t status = 0;
    serial_manager_config_t serialConfig;

    serial_port_usb_cdc_config_t usbCdcConfig = {
        .controllerIndex = (serial_port_usb_cdc_controller_index_t)CONTROLLER_ID,
    };

    s_ShellEventGroup = xEventGroupCreate();

    static volatile uint8_t usb_clock_initialized = 0;
    if (!usb_clock_initialized)
    {
        usb_clock_initialized = 1;
        USB_DeviceClockInit();
    }

#if ENABLE_AUDIO_DUMP
    USB_DeviceApplicationInit();
#endif /* ENABLE_USB_AUDIO_DUMP */

    /* Init Serial Manager for USB CDC */
    serialConfig.type = kSerialPort_UsbCdc;
#if (defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))
    serialConfig.ringBuffer     = &readRingBuffer[0];
    serialConfig.ringBufferSize = SLN_SERIAL_MANAGER_RECEIVE_BUFFER_LEN;
#endif
    serialConfig.portConfig = &usbCdcConfig;

    status = SerialManager_Init(s_serialHandle, &serialConfig);
    if (status != kStatus_SerialManager_Success)
    {
        return (int32_t)status;
    }

    /* Init SHELL */
    s_shellHandle = &s_shellHandleBuffer[0];
    SHELL_Init(s_shellHandle, s_serialHandle, "SHELL>> ");

    /* Add the SLN commands to the commands list */
#if USE_WIFI_CONNECTION
    SHELL_RegisterCommand(s_shellHandle, SHELL_COMMAND(print));
    SHELL_RegisterCommand(s_shellHandle, SHELL_COMMAND(setup));
    SHELL_RegisterCommand(s_shellHandle, SHELL_COMMAND(erase));
#endif
    SHELL_RegisterCommand(s_shellHandle, SHELL_COMMAND(reset));
    SHELL_RegisterCommand(s_shellHandle, SHELL_COMMAND(factoryreset));
    SHELL_RegisterCommand(s_shellHandle, SHELL_COMMAND(changebank));
    SHELL_RegisterCommand(s_shellHandle, SHELL_COMMAND(commands));
    SHELL_RegisterCommand(s_shellHandle, SHELL_COMMAND(changedemo));
    SHELL_RegisterCommand(s_shellHandle, SHELL_COMMAND(changelang));
    SHELL_RegisterCommand(s_shellHandle, SHELL_COMMAND(volume));
    SHELL_RegisterCommand(s_shellHandle, SHELL_COMMAND(mute));
    SHELL_RegisterCommand(s_shellHandle, SHELL_COMMAND(timeout));
    SHELL_RegisterCommand(s_shellHandle, SHELL_COMMAND(followup));
    SHELL_RegisterCommand(s_shellHandle, SHELL_COMMAND(ptt));
#if ENABLE_DSMT_ASR
    SHELL_RegisterCommand(s_shellHandle, SHELL_COMMAND(cmdresults));
#endif
    SHELL_RegisterCommand(s_shellHandle, SHELL_COMMAND(updateotw));
#if USE_WIFI_CONNECTION
    SHELL_RegisterCommand(s_shellHandle, SHELL_COMMAND(updateota));
#endif
    SHELL_RegisterCommand(s_shellHandle, SHELL_COMMAND(version));
#if ENABLE_AUDIO_DUMP && ENABLE_AMPLIFIER
    SHELL_RegisterCommand(s_shellHandle, SHELL_COMMAND(usb_aec_align));
#endif /* ENABLE_AUDIO_DUMP && ENABLE_AMPLIFIER */
#if SLN_TRACE_CPU_USAGE
    SHELL_RegisterCommand(s_shellHandle, SHELL_COMMAND(cpu_view));
#endif /* SLN_TRACE_CPU_USAGE */

    return status;
}

void sln_shell_task(void *arg)
{
    volatile EventBits_t shellEvents = 0U;

    SHELL_Printf(s_shellHandle, "Howdy! Type \"help\" to see what this shell can do!\r\n");
    SHELL_Printf(s_shellHandle, "SHELL>> ");

    while (1)
    {
        shellEvents = xEventGroupWaitBits(s_ShellEventGroup, 0x00FFFFFF, pdTRUE, pdFALSE, portMAX_DELAY);

#if USE_WIFI_CONNECTION
        status_t status = kStatus_Success;

        if (shellEvents & PRINT_EVENT)
        {
            wifi_cred_t wifi_cred = {0};

            /* Stores the WiFi SSID as a printable string. Added one extra byte for the NULL terminator. */
            char ssid_str[SSID_NAME_SIZE + 1] = {0};

            status = wifi_credentials_flash_get(&wifi_cred);
            if (!status)
            {
                if (check_valid_credentials(&wifi_cred))
                {
                    SHELL_Printf(s_shellHandle, "Found no credentials in flash\r\n");
                    SHELL_Printf(s_shellHandle, "SHELL>> ");
                }
                else
                {
                    SHELL_Printf(s_shellHandle, "These are the credentials:\r\n");

                    strncpy(ssid_str, (char *)wifi_cred.ssid.value, SSID_NAME_SIZE);
                    SHELL_Printf(s_shellHandle, "Wifi name: %s\r\n", ssid_str);

                    SHELL_Printf(s_shellHandle, "Wifi password: %s\r\n", wifi_cred.password.value);
                    SHELL_Printf(s_shellHandle, "SHELL>> ");
                }
            }
            else
            {
                SHELL_Printf(s_shellHandle, "Failed to read wifi credentials from flash, error code %d\r\n", status);
                SHELL_Printf(s_shellHandle, "SHELL>> ");
            }
        }

        if (shellEvents & SETUP_EVENT)
        {
            status = wifi_credentials_flash_set(&s_wifi_cred);
            if (!status)
            {
                SHELL_Printf(s_shellHandle, "Credentials saved\r\n");
                NVIC_SystemReset();
            }
            else
            {
                SHELL_Printf(s_shellHandle, "Failed to write wifi credentials in flash, error code %d\r\n", status);
                SHELL_Printf(s_shellHandle, "SHELL>> ");
            }
        }

        if (shellEvents & ERASE_EVENT)
        {
            status = wifi_credentials_flash_reset();
            if (!status)
            {
                SHELL_Printf(s_shellHandle, "Credentials erased\r\n");
                NVIC_SystemReset();
            }
            else
            {
                SHELL_Printf(s_shellHandle, "Failed to wipe wifi credentials from flash, error code %d\r\n", status);
                SHELL_Printf(s_shellHandle, "SHELL>> ");
            }
        }
#endif
        if (shellEvents & VERSION_EVT)
        {
            SHELL_Printf(s_shellHandle, "Firmware version: %d.%d.%d, Current Bank: %s, ", APP_MAJ_VER, APP_MIN_VER,
                         APP_BLD_VER, getActiveBank());
            print_asr_version();
            SHELL_Printf(s_shellHandle, "SHELL>> ");
        }

#if ENABLE_AUDIO_DUMP && ENABLE_AMPLIFIER
        if (shellEvents & AUDIO_DUMP_TEST_EVT)
        {
            static bool test_sound_on = false;
            if (test_sound_on)
            {
                SHELL_Printf(s_shellHandle, "Board stopped playing the test sound!\r\n");
                AUDIO_DUMP_AecAlignSoundStop();
                test_sound_on = false;
            }
            else
            {
                SHELL_Printf(s_shellHandle, "Board will start playing a test sound!\r\n");
                AUDIO_DUMP_AecAlignSoundStart();
                test_sound_on = true;
            }
            SHELL_Printf(s_shellHandle, "SHELL>> ");
        }
#endif /* ENABLE_AUDIO_DUMP && ENABLE_AMPLIFIER */

#if SLN_TRACE_CPU_USAGE
        if (shellEvents & TRACE_CPU_USAGE_EVT)
        {
            char *cpuLoad = NULL;

            cpuLoad = PERF_GetCPULoad();
            if (cpuLoad != NULL)
            {
                SHELL_Printf(s_shellHandle, "\r\n\r\nCPU usage:\r\n%s\r\n", cpuLoad);
            }
            else
            {
                SHELL_Printf(s_shellHandle, "Could NOT get the CPU usage\r\n");
            }
        }
#endif /* SLN_TRACE_CPU_USAGE */

        if (shellEvents & RESET_EVENT)
        {
            NVIC_SystemReset();
        }

        if (shellEvents & FACTORY_RESET_EVT)
        {
            uint32_t statusFlash  = SLN_FLASH_FS_OK;

            statusFlash = sln_flash_fs_ops_erase(ASR_SHELL_COMMANDS_FILE_NAME);

            if (statusFlash != SLN_FLASH_FS_OK)
            {
                configPRINTF(("Failed deleting local demo configuration from flash memory.\r\n\r\n"));
            }
            else
            {
                configPRINTF(("Resetting the board to original settings.\r\n\r\n"));
                NVIC_SystemReset();
            }
        }

        if (shellEvents & CHANGE_BANK_EVT)
        {
            int32_t oldImgType;
            uint32_t status = SLN_FLASH_NO_ERROR;

            status = FICA_GetCurAppStartType(&oldImgType);
            if (status == SLN_FLASH_NO_ERROR)
            {
                if (FICA_IMG_TYPE_APP_A == oldImgType)
                {
                    FICA_SetCurAppStartType(FICA_IMG_TYPE_APP_B);
                    SHELL_Printf(s_shellHandle, "Application bank switched from A to B\r\n");
                }
                else
                {
                    FICA_SetCurAppStartType(FICA_IMG_TYPE_APP_A);
                    SHELL_Printf(s_shellHandle, "Application bank switched from B to A\r\n");
                }
            }
            else
            {
                SHELL_Printf(s_shellHandle, "[Error] Switching application bank failed %d\r\n", status);
            }

            SHELL_Printf(s_shellHandle, "\r\nRebooting...\r\n");
            vTaskDelay(200);
            NVIC_SystemReset();
        }

        if (shellEvents & PRINT_COMMANDS_EVT)
        {
            unsigned int cmd_number     = 0;
            asr_language_t lang         = ASR_FIRST_LANGUAGE;
            asr_language_t activeLanguage = appAsrShellCommands.activeLanguage;

            configPRINTF(("Available commands in selected language(s):\r\n\r\n"));

            while (activeLanguage != UNDEFINED_LANGUAGE)
            {
                if ((activeLanguage & lang) != UNDEFINED_LANGUAGE)
                {
                    configPRINTF(("Demo:      %s\r\n", get_demo_string(lang, appAsrShellCommands.demo)));
                    configPRINTF(("Language:  %s\r\n", get_language_str_from_id(lang)));
                    configPRINTF(("Wake Word: %s\r\n", get_ww_strings(lang)[0]));
                    configPRINTF(("Commands:\r\n"));

                    cmd_number = get_cmd_number((asr_language_t)lang, appAsrShellCommands.demo);
                    char **cmdString = get_cmd_strings(lang, appAsrShellCommands.demo);

                    for (unsigned int i = 0; i < cmd_number; i++)
                    {
                        if (strlen(cmdString[i]) > 1)
                        {
                            if (cmdString[i][strlen(cmdString[i]) - 2] == '^')
                            {
                                /* Skip duplicated commands. Duplicated commands have '^' at the end. */
                                continue;
                            }
                        }

                        configPRINTF(("  %s\r\n", cmdString[i]));
                    }
                    activeLanguage &= ~lang;

                    configPRINTF(("\r\n"));
                }
                lang <<= 1U;
            }
        }

        if (shellEvents & CHANGE_DEMO_EVT)
        {
            char *str;
            uint16_t demo_id = UNDEFINED_INFERENCE;

            if (s_argc != 2)
            {
                SHELL_Printf(
                    s_shellHandle,
                    "\r\nIncorrect command parameter(s). Enter \"help\" to view a list of available commands.\r\n\r\n");
            }
            else
            {
                str = s_argv[1];

                demo_id   = get_demo_id_from_str(str);

                if (demo_id != UNDEFINED_INFERENCE)
                {
                    appAsrShellCommands.demo   = demo_id;
                    appAsrShellCommands.status = WRITE_READY;
                    appAsrShellCommands.asrCfg = ASR_CFG_CMD_INFERENCE_ENGINE_CHANGED;
                    configPRINTF(("Changing to %s commands demo.\r\n", str));
                }
                else
                {
                    configPRINTF(("Invalid input.\r\n"));
                }

                // notify main task
                xTaskNotifyFromISR(appTaskHandle, kDefault, eSetBits, NULL);
            }
        }

        if (shellEvents & CHANGE_LANG_EVT)
        {
            int32_t status                  = kStatus_SHELL_Success;
            int32_t language_limit          = 0;
            asr_language_t activeLanguage   = UNDEFINED_LANGUAGE;

            int idx;

            if ((s_argc - 1) > MAX_CONCURRENT_LANGUAGES)
            {
                SHELL_Printf(s_shellHandle,
                             "\r\nIncorrect command parameter(s). Please enter up to %d languages at a time.\r\n\r\n",
                             MAX_CONCURRENT_LANGUAGES);
            }
            else
            {
                /* when called without arguments, we will print the active languages */
                if (s_argc == 1)
                {
                    activeLanguage = appAsrShellCommands.activeLanguage;

                    configPRINTF(("Enabled language(s):"));
                    print_active_languages_str(appAsrShellCommands.activeLanguage, appAsrShellCommands.demo);
                    configPRINTF((".\r\n"));
                }
                else
                {
                    activeLanguage = UNDEFINED_LANGUAGE;

#if MULTILINGUAL
                    if ((appAsrShellCommands.ptt == ASR_PTT_ON) &&
                        (s_argc > 2))
                    {
                        /* during PTT only one language will work */
                        configPRINTF(("Only one language supported during Push-to-Talk mode\r\n"));
                    }
                    else
#endif /* MULTILINGUAL */
                    {
                        configPRINTF(("Enabling "));

                        language_limit = s_argc;

                        for (idx = 1; idx < language_limit; idx++)
                        {
                            uint16_t current_lang_id = get_language_id_from_str(s_argv[idx]);

                            if ((current_lang_id == UNDEFINED_LANGUAGE) ||
                                (activeLanguage & current_lang_id))
                            {
                                configPRINTF(("\r\nERROR(arg: %s) ", s_argv[idx]));
                                status = kStatus_SHELL_Error;
                                break;
                            }
                            else
                            {
                                activeLanguage |= current_lang_id;
                                configPRINTF(("%s ", s_argv[idx]));
                            }
                        }

                        if (status == kStatus_SHELL_Success)
                        {
                            configPRINTF(("language(s).\r\n"));
                        }

                        if (status == kStatus_SHELL_Success)
                        {
                            appAsrShellCommands.activeLanguage = activeLanguage;
                            appAsrShellCommands.status         = WRITE_READY;
                            appAsrShellCommands.asrCfg         = ASR_CFG_DEMO_LANGUAGE_CHANGED;

                            if (s_argc == 2)
                            {
                                /* if only one language sent, let's change the oob demo as well
                                 * to have the prompts in this language */
                                oob_demo_control.language = activeLanguage;
                            }
                            else
                            {
                                /* if multiple languages sent, let's use the first one for prompts initially */
                                oob_demo_control.language = get_language_id_from_str(s_argv[1]);
                            }

                            /* notify main task */
                            xTaskNotifyFromISR(appTaskHandle, kDefault, eSetBits, NULL);
                        }
                        else
                        {
                            SHELL_Printf(s_shellHandle,
                                         "\r\nIncorrect/duplicated command parameter(s). Enter \"help\" to view a list of "
                                         "available commands.\r\n\r\n");
                        }
                    }
                }
            }
        }

        if (shellEvents & SET_VOLUME_EVT)
        {
            if (s_argc > 2)
            {
                SHELL_Printf(
                    s_shellHandle,
                    "\r\nIncorrect command parameter(s). Enter \"help\" to view a list of available commands.\r\n\r\n");
            }
            else
            {
                if (s_argc == 1)
                {
                    configPRINTF(("Speaker volume set to %d.\r\n", appAsrShellCommands.volume));
                }
                else if (s_argc == 2 && (isNumber(s_argv[1]) == kStatus_SHELL_Success) && atoi(s_argv[1]) >= 0 &&
                         atoi(s_argv[1]) <= 100)
                {
                    appAsrShellCommands.volume = (uint32_t)atoi(s_argv[1]);
                    appAsrShellCommands.status = WRITE_READY;
                    configPRINTF(("Setting speaker volume to %d.\r\n", appAsrShellCommands.volume));

                    // notify main task
                    xTaskNotifyFromISR(appTaskHandle, kVolumeUpdate, eSetBits, NULL);
                }
                else
                {
                    configPRINTF(("Invalid volume value. Set between 0 to 100.\r\n", appAsrShellCommands.volume));
                }
            }
        }

        if (shellEvents & FOLLOWUP_EVT)
        {
            char *str;

            if (s_argc != 2)
            {
                SHELL_Printf(
                    s_shellHandle,
                    "\r\nIncorrect command parameter(s). Enter \"help\" to view a list of available commands.\r\n\r\n");
            }
            else
            {
                str = s_argv[1];

                if (strcmp(str, "on") == 0)
                {
                    appAsrShellCommands.followup = ASR_FOLLOWUP_ON;
                    appAsrShellCommands.status   = WRITE_READY;
                    configPRINTF(("Setting ASR Follow-Up mode on.\r\n"));
                }
                else if (strcmp(str, "off") == 0)
                {
                    appAsrShellCommands.followup = ASR_FOLLOWUP_OFF;
                    appAsrShellCommands.status   = WRITE_READY;
                    configPRINTF(("Setting ASR Follow-Up mode off.\r\n"));
                }
                else
                {
                    configPRINTF(("Invalid input.\r\n"));
                }

                // notify main task
                xTaskNotifyFromISR(appTaskHandle, kDefault, eSetBits, NULL);
            }
        }

        if (shellEvents & SET_TIMEOUT_EVT)
        {
            if (s_argc > 2)
            {
                SHELL_Printf(
                    s_shellHandle,
                    "\r\nIncorrect command parameter(s). Enter \"help\" to view a list of available commands.\r\n\r\n");
            }
            else
            {
                if (s_argc == 1)
                {
                    configPRINTF(("Timeout value set to %d.\r\n", appAsrShellCommands.timeout));
                }
                else if (s_argc == 2 && (isNumber(s_argv[1]) == kStatus_SHELL_Success) && atoi(s_argv[1]) >= 0)
                {
                    appAsrShellCommands.timeout = (uint32_t)atoi(s_argv[1]);
                    appAsrShellCommands.status  = WRITE_READY;
                    configPRINTF(("Setting command waiting time to %d ms.\r\n", appAsrShellCommands.timeout));

                    // notify main task
                    xTaskNotifyFromISR(appTaskHandle, kDefault, eSetBits, NULL);
                }
                else
                {
                    configPRINTF(("Invalid waiting time %d ms.\r\n", appAsrShellCommands.timeout));
                }
            }
        }

        if (shellEvents & MUTE_EVT)
        {
            char *str;

            if (s_argc != 2)
            {
                SHELL_Printf(
                    s_shellHandle,
                    "\r\nIncorrect command parameter(s). Enter \"help\" to view a list of available commands.\r\n\r\n");
            }
            else
            {
                str = s_argv[1];

                if (strcmp(str, "on") == 0)
                {
                    appAsrShellCommands.mute   = ASR_MUTE_ON;
                    appAsrShellCommands.status = WRITE_READY;
                    configPRINTF(("Setting mute on.\r\n"));

                    if (appAsrShellCommands.ptt == ASR_PTT_ON)
                    {
                        // deactivate PTT mode when muted
                        appAsrShellCommands.ptt = ASR_PTT_OFF;
                        configPRINTF(("Disabling ASR Push-To-Talk mode.\r\n"));
                    }

                    // notify main task
                    xTaskNotifyFromISR(appTaskHandle, kMicUpdate, eSetBits, NULL);
                }
                else if (strcmp(str, "off") == 0)
                {
                    appAsrShellCommands.mute   = ASR_MUTE_OFF;
                    appAsrShellCommands.status = WRITE_READY;
                    configPRINTF(("Setting mute off.\r\n"));

                    // notify main task
                    xTaskNotifyFromISR(appTaskHandle, kMicUpdate, eSetBits, NULL);
                }
                else
                {
                    configPRINTF(("Invalid input.\r\n"));
                }
            }
        }

        if (shellEvents & PTT_EVT)
        {
            char *str;

            if (s_argc != 2)
            {
                SHELL_Printf(
                    s_shellHandle,
                    "\r\nIncorrect command parameter(s). Enter \"help\" to view a list of available commands.\r\n\r\n");
            }
            else
            {
                str = s_argv[1];

                if (strcmp(str, "on") == 0)
                {
                    if (appAsrShellCommands.mute == ASR_MUTE_OFF)
                    {
                        appAsrShellCommands.ptt            = ASR_PTT_ON;
                        appAsrShellCommands.status         = WRITE_READY;
#if MULTILINGUAL
                        uint16_t prevLanguage              = appAsrShellCommands.activeLanguage;
                        appAsrShellCommands.activeLanguage = active_languages_get_first(appAsrShellCommands.activeLanguage);
                        oob_demo_control.language          = appAsrShellCommands.activeLanguage;

                        if (appAsrShellCommands.activeLanguage != prevLanguage)
                        {
                            appAsrShellCommands.asrCfg         = ASR_CFG_DEMO_LANGUAGE_CHANGED;
                        }

                        configPRINTF(("Setting ASR Push-To-Talk mode on. %s language active.\r\n",
                                      get_language_str_from_id(active_languages_get_first(appAsrShellCommands.activeLanguage))));
#else
                        configPRINTF(("Setting ASR Push-To-Talk mode on. %s language active.\r\n",
                                      get_language_str_from_id(appAsrShellCommands.activeLanguage)));
#endif /* MULTILINGUAL */

                        // notify main task
                        xTaskNotifyFromISR(appTaskHandle, kMicUpdate, eSetBits, NULL);
                    }
                    else
                    {
                        // cannot activate PTT when muted
                        configPRINTF(("Mics are muted! Turn mute off before activating ASR Push-To-Talk mode.\r\n"));
                    }
                }
                else if (strcmp(str, "off") == 0)
                {
                    appAsrShellCommands.ptt    = ASR_PTT_OFF;
                    appAsrShellCommands.status = WRITE_READY;
                    configPRINTF(("Setting ASR Push-To-Talk mode off.\r\n"));

                    // notify main task
                    xTaskNotifyFromISR(appTaskHandle, kMicUpdate, eSetBits, NULL);
                }
                else
                {
                    configPRINTF(("Invalid input.\r\n"));
                }
            }
        }

#if ENABLE_DSMT_ASR
        if (shellEvents & PRINT_CMD_RES_EVT)
        {
            char *str;

            if (s_argc != 2)
            {
                SHELL_Printf(
                    s_shellHandle,
                    "\r\nIncorrect command parameter(s). Enter \"help\" to view a list of available commands.\r\n\r\n");
            }
            else
            {
                str = s_argv[1];

                if (strcmp(str, "on") == 0)
                {
                    appAsrShellCommands.cmdresults = ASR_CMD_RES_ON;
                    configPRINTF(("Setting command results printing to on.\r\n"));
                }
                else if (strcmp(str, "off") == 0)
                {
                    appAsrShellCommands.cmdresults = ASR_CMD_RES_OFF;
                    configPRINTF(("Setting command results printing to off.\r\n"));
                }
                else
                {
                    configPRINTF(("Invalid input.\r\n"));
                }
            }
        }
#endif /* ENABLE_DSMT_ASR */

        if (shellEvents & OTW_UPDATE_EVT)
        {
            FWUpdate_set_SLN_OTW();

            SHELL_Printf(s_shellHandle, "\r\nReseting the board in OTW update mode\r\n");
            NVIC_SystemReset();
        }

#if USE_WIFI_CONNECTION
        if (shellEvents & OTA_UPDATE_EVT)
        {
            FWUpdate_set_SLN_OTA();

            SHELL_Printf(s_shellHandle, "\r\nReseting the board in OTA update mode\r\n");
            NVIC_SystemReset();
        }
#endif /* USE_WIFI_CONNECTION */
    }

    while (SerialManager_Deinit(s_serialHandle) == kStatus_SerialManager_Busy)
    {
        /* should happen pretty quickly after the call of g_shellCommandexit.pFuncCallBack,
         * just need to wait till the read and write handles are closed */
    }

    /* clean event group */
    vEventGroupDelete(s_ShellEventGroup);

    vTaskDelete(NULL);
}

void sln_shell_set_app_init_task_handle(TaskHandle_t *handle)
{
    if (NULL != handle)
    {
        s_appInitTask = *handle;
    }
}

void sln_shell_trace_malloc(void *ptr, size_t size)
{
    if (s_shellHandle)
    {
        if (s_heap_trace.enable)
        {
            if (size >= s_heap_trace.threshold)
            {
                SHELL_Printf(s_shellHandle, "[TRACE] Allocated %d bytes to 0x%X\r\n", size, (int)ptr);
                SHELL_Printf(s_shellHandle, "SHELL>> ");
            }
        }
    }
}

void sln_shell_trace_free(void *ptr, size_t size)
{
    if (s_shellHandle)
    {
        if (s_heap_trace.enable)
        {
            if (size >= s_heap_trace.threshold)
            {
                SHELL_Printf(s_shellHandle, "[TRACE] De-allocated %d bytes from 0x%X\r\n", size, (int)ptr);
                SHELL_Printf(s_shellHandle, "SHELL>> ");
            }
        }
    }
}
#if !(SDK_DEBUGCONSOLE)
int DbgConsole_Printf(const char *formatString, ...)
{
    va_list ap;
    char logbuf[configLOGGING_MAX_MESSAGE_LENGTH] = {0};

    va_start(ap, formatString);
    vsnprintf(logbuf, configLOGGING_MAX_MESSAGE_LENGTH, formatString, ap);

    va_end(ap);

    return 0;
}
#endif
