/*
 * Copyright 2019-2022 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "network_connection.h"

#include "lwip/dhcp.h"
#include "lwip/dns.h"
#include "lwip/prot/dhcp.h"
#include "lwip/tcpip.h"
#include "netif/ethernet.h"

#if USE_ETHERNET_CONNECTION
#include "fsl_phydp83848.h"
#include "fsl_enet_mdio.h"

#include "enet_ethernetif.h"
#elif USE_WIFI_CONNECTION
#include "lwip/opt.h"
#include "lwip/tcpip.h"
#include "wifi_credentials.h"

#define AP_SECURITY WICED_SECURITY_WPA2_AES_PSK
#define AP_CHANNEL  (1)

__attribute__((weak)) void APP_Wifi_UX_Callback(network_wifi_conn_state_t state)
{
    configPRINTF(("Network connection state, %d\r\n", state));
}

#else
/* should never happen */
#endif

#if USE_ETHERNET_CONNECTION
/* MAC address configuration. */
#define configMAC_ADDR                     \
    {                                      \
        0x00, 0x04, 0x9F, 0x05, 0x1C, 0x81 \
    }

/* MDIO operations. */
#define EXAMPLE_MDIO_OPS enet_ops

/* Address of PHY interface. */
#define EXAMPLE_PHY_ADDRESS BOARD_ENET0_PHY_ADDRESS

/* PHY operations. */
#define EXAMPLE_PHY_OPS phydp83848_ops

static mdio_handle_t mdioHandle = {.ops = &EXAMPLE_MDIO_OPS};
static phy_handle_t phyHandle   = {.phyAddr = EXAMPLE_PHY_ADDRESS, .mdioHandle = &mdioHandle, .ops = &EXAMPLE_PHY_OPS};
#endif /* USE_ETHERNET_CONNECTION */

static struct netif ip_netif;
static bool s_connectState                  = false;
static connection_update_fn *update_link_cb = NULL;

TickType_t get_mqtt_connection_backoff(uint32_t retry_attempts, uint32_t base_delay)
{
    /** Example
     *  base_delay = 500ms
     *  retry_attempts = 3
     *  exponential = 150ms = ((base_delay / 10) * retry attempts)
     *  delay_adder_from_base = 950ms = (base_delay + (exponential * retry_attempts)
     *  full_delay = 1450ms = base_delay + delay_adder_from_base
     */
    return (pdMS_TO_TICKS((base_delay) + ((base_delay + ((base_delay / 10) * retry_attempts) * retry_attempts))));
}

void APP_Connect_Update_Handler_Set(connection_update_fn *func)
{
    update_link_cb = func;
}

ip_addr_t get_ipaddress(void)
{
    return ip_netif.ip_addr;
}

bool get_connect_state(void)
{
    return s_connectState;
}

status_t APP_NETWORK_Re_Link(void)
{
    /* Wait indefinitely for the link to re-establish. */
    do
    {
        vTaskDelay(portTICK_PERIOD_MS * 1000);
    } while (!get_connect_state());

#if USE_WIFI_CONNECTION

    /**
     * For WiFi connection, link loss is associated with loss of RF signal.
     *
     * Best practice here is to re-establish RF connection once the WiFi
     * Access Point's RF signal is present.
     */

    vTaskDelay(portTICK_PERIOD_MS * 1000);

    configPRINTF(("Resetting WiFi connection...\r\n"));
    APP_NETWORK_Uninit();

    configPRINTF(("...connecting to Access Point...\r\n"));
    return APP_NETWORK_Wifi_Connect(true, false);
#else
    return kStatus_Success;
#endif
}

#if USE_ETHERNET_CONNECTION

void enet_status_callback(struct netif *netif)
{
    bool linkUp = false;

    if (netif_is_link_up(netif))
    {
        linkUp = true;
    }
    else
    {
        linkUp = false;
    }

    /* Only call if it's a valid function pointer */
    if (update_link_cb)
    {
        (update_link_cb)(linkUp);
    }

    s_connectState = linkUp;
}

void APP_NETWORK_Init(bool use_dhcp)
{
    ip4_addr_t ipaddr, netmask, gw;
    struct dhcp *dhcp;
    ethernetif_config_t enet_config = {
        .phyHandle  = &phyHandle,
        .macAddress = configMAC_ADDR,
    };

    if (use_dhcp)
    {
        IP4_ADDR(&ipaddr, 0, 0, 0, 0);
        IP4_ADDR(&netmask, 0, 0, 0, 0);
        IP4_ADDR(&gw, 0, 0, 0, 0);
    }
    else
    {
        IP4_ADDR(&ipaddr, configIP_ADDR0, configIP_ADDR1, configIP_ADDR2, configIP_ADDR3);
        IP4_ADDR(&netmask, configNET_MASK0, configNET_MASK1, configNET_MASK2, configNET_MASK3);
        IP4_ADDR(&gw, configGW_ADDR0, configGW_ADDR1, configGW_ADDR2, configGW_ADDR3);

#if LWIP_DNS
        ip4_addr_t dnsserver;
        IP4_ADDR(&dnsserver, configDNS1_ADDR0, configDNS1_ADDR1, configDNS1_ADDR2, configDNS1_ADDR3);
        dns_setserver(0, &dnsserver);
        IP4_ADDR(&dnsserver, configDNS2_ADDR0, configDNS2_ADDR1, configDNS2_ADDR2, configDNS2_ADDR3);
        dns_setserver(1, &dnsserver);
#endif
    }

    configPRINTF(("APP_NETWORK_Init: start\r\n"));
    vTaskDelay(100);

    tcpip_init(NULL, NULL);

    if (enet_config.phyHandle->mdioHandle->resource.csrClock_Hz == 0U)
    {
        enet_config.phyHandle->mdioHandle->resource.csrClock_Hz = CLOCK_GetPllFreq(kCLOCK_PllEnet);
    }

    netif_add(&ip_netif, &ipaddr, &netmask, &gw, &enet_config, ethernetif0_init, tcpip_input);
    netif_set_default(&ip_netif);
    netif_set_up(&ip_netif);
    netif_set_link_callback(&ip_netif, &enet_status_callback);

    if (use_dhcp)
    {
        configPRINTF(("Getting IP address from DHCP ...\r\n"));
        dhcp_start(&ip_netif);

        dhcp = (struct dhcp *)netif_get_client_data(&ip_netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);
        while (dhcp->state != DHCP_STATE_BOUND)
            vTaskDelay(1000);

        if (dhcp->state == DHCP_STATE_BOUND)
        {
            configPRINTF(("IPv4 Address: %u.%u.%u.%u\r\n", ((u8_t *)&ip_netif.ip_addr.addr)[0],
                          ((u8_t *)&ip_netif.ip_addr.addr)[1], ((u8_t *)&ip_netif.ip_addr.addr)[2],
                          ((u8_t *)&ip_netif.ip_addr.addr)[3]));
        }

        configPRINTF(("DHCP OK\r\n"));
    }

    if (netif_is_link_up(&ip_netif))
    {
        s_connectState = true;
    }
    else
    {
        s_connectState = false;
    }
}

void APP_NETWORK_Uninit()
{
    /* do nothing */
}
#elif USE_WIFI_CONNECTION

void APP_NETWORK_Init(void)
{
}

status_t APP_NETWORK_Wifi_Connect(bool use_dhcp, bool provideUxLed)
{
    return kStatus_Success;
}

void APP_NETWORK_Uninit()
{
}

status_t APP_NETWORK_Wifi_StartAP(char *ap_ssid, char *ap_passwd)
{
    return kStatus_Success;
}

status_t APP_NETWORK_Wifi_StopAP(void)
{
    return kStatus_Success;
}

status_t APP_NETWORK_Wifi_CheckCred(void)
{
    wifi_cred_t wifi_cred = {0};

    /* try to get credentials from flash */
    if (wifi_credentials_flash_get(&wifi_cred))
    {
        return kStatus_Fail;
    }

    if (check_valid_credentials(&wifi_cred))
    {
        return kStatus_Fail;
    }

    return kStatus_Success;
}

#endif /* USE_WIFI_CONNECTION */
