#include "ohos_init.h"
#include "ohos_types.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "cmsis_os2.h"

#include "wifi_device.h"
#include "wifi_event.h"
#include "wifi_error_code.h"

#include "lwip/netif.h"
#include "lwip/netifapi.h"
#include "lwip/ip4_addr.h"
#include "lwip/api_shell.h"

#include "genki_wifi_sta.h"
#include "hi_nv.h"

#define DEF_TIMEOUT 15
#define ONE_SECOND 1

#define SELECT_WLAN_PORT "wlan0"

static WifiEvent wifi_event_handler = {0};
static int g_staScanSuccess = 0;
static int g_ConnectSuccess = 0;
static int ssid_count = 0;

static struct netif *g_lwip_netif = NULL;
static char *wifi_ssid = NULL;

typedef struct {
    hi_u8 enable;
    hi_u8 ssid[33];
    hi_u8 password[65];
} genki_nv_sta_cache;

#define NV_ID 0x83

static unsigned char is_connected = 0;


static void OnWifiConnectionChanged(int state, WifiLinkedInfo *info) {
    (void) info;

    if (state > 0) {
        g_ConnectSuccess = 1;
        printf("[wifi_sta]cb for wifi connect, success\r\n");
    } else {
        printf("[wifi_sta]cb for wifi connect, failed, please check password\r\n");
        wifi_sta_clear_cache();
    }
    return;
}

static void OnWifiScanStateChanged(int state, int size) {
    (void) state;
    if (size > 0) {
        ssid_count = size;
        g_staScanSuccess = 1;
    }
    return;
}

static void OnHotspotStateChanged(int state) {
    printf("[wifi_sta]cb for HotspotStateChanged:state is %d.\n", state);
    return;
}

static void OnHotspotStaJoin(StationInfo *info) {
    (void) info;
    printf("[wifi_sta]STA join AP\r\n");
    return;
}

static void OnHotspotStaLeave(StationInfo *info) {
    (void) info;
    printf("[wifi_sta]cb for HotspotStaLeave:mac is %s.\r\n", info->macAddress);
    return;
}

static WifiErrorCode wifi_init(void) {
    WifiErrorCode error;
    wifi_event_handler.OnWifiConnectionChanged = OnWifiConnectionChanged;
    wifi_event_handler.OnWifiScanStateChanged = OnWifiScanStateChanged;
    wifi_event_handler.OnHotspotStateChanged = OnHotspotStateChanged;
    wifi_event_handler.OnHotspotStaJoin = OnHotspotStaJoin;
    wifi_event_handler.OnHotspotStaLeave = OnHotspotStaLeave;
    error = RegisterWifiEvent(&wifi_event_handler);
    return error;
}

static void wait_scan_result(void) {
    int scanTimeout = DEF_TIMEOUT;
    while (scanTimeout > 0) {
        sleep(ONE_SECOND);
        scanTimeout--;
        if (g_staScanSuccess == 1) {
            printf("[wifi_sta]wait scan result: wait success[%d]s\n", (DEF_TIMEOUT - scanTimeout));
            break;
        }
    }
    if (scanTimeout <= 0) {
        printf("[wifi_sta]wait scan result:timeout!\n");
    }
}

static int wait_connect_result(void) {
    int ConnectTimeout = DEF_TIMEOUT;
    while (ConnectTimeout > 0) {
        sleep(1);
        ConnectTimeout--;
        if (g_ConnectSuccess == 1) {
            printf("[wifi_sta]wait connect result:wait success[%d]s\n", (DEF_TIMEOUT - ConnectTimeout));
            break;
        }
    }
    if (ConnectTimeout <= 0) {
        printf("[wifi_sta]wait connect result:timeout!\n");
        return 0;
    }
    return 1;
}

int wifi_sta_connect(const char *ssid, const char *password, const char *hostname) {

    // wifi init
    WifiErrorCode error = wifi_init();
    if (WIFI_SUCCESS != error) {
        printf("[wifi_sta]register wifi event failed, error=%d\r\n", error);
        return error;
    }
    printf("[wifi_sta]register wifi event success\r\n");

    // wifi enable
    error = EnableWifi();
    if (WIFI_SUCCESS != error) {
        printf("[wifi_sta]enable wifi  failed, error=%d\r\n", error);
        return error;
    }
    printf("[wifi_sta]enable wifi success\r\n");

    // check wifi active
    if (IsWifiActive() == 0) {
        printf("[wifi_sta] wifi station is not active\r\n");
        return error;
    }

//    unsigned int size = WIFI_SCAN_HOTSPOT_LIMIT;
//    WifiScanInfo *info = NULL;
//    info = malloc(sizeof(WifiScanInfo) * size);
//    if (NULL == info) {
//        printf("[wifi_sta] wifi scan info init failed\r\n");
//        return error;
//    }

//    // scan wifi list
//    do {
//        // reset
//        ssid_count = 0;
//        g_staScanSuccess = 0;
//
//        // start scan
//        Scan();
//
//        // wait scan result
//        wait_scan_result();
//
//        // get scan result
//        error = GetScanInfoList(info, &size);
//
//    } while (g_staScanSuccess != 1);
//
//    // print wifi list
//    for (int i = 0; i < ssid_count; ++i) {
//        printf("[wifi_sta]scan wifi: %s\r\n", info[i].ssid);
//
//        if (strcmp(ssid, info[i].ssid) == 0) {
//            WifiDeviceConfig ap_config = {0};
//            strcpy(ap_config.ssid, info[i].ssid);
//            strcpy(ap_config.preSharedKey, password);
//            ap_config.securityType = WIFI_SEC_TYPE_PSK;
//
//            int result;
//            if (AddDeviceConfig(&ap_config, &result) == WIFI_SUCCESS) {
//
//                // connect to wifi
//                if (ConnectTo(result) == WIFI_SUCCESS && wait_connect_result() == 1) {
//                    printf("[wifi_sta]wifi connect success!\r\n");
//                    g_lwip_netif = netifapi_netif_find(SELECT_WLAN_PORT);
//                    break;
//                }
//            }
//        }
//
//        if (i == ssid_count - 1) {
//            printf("[wifi_sta]ERROR No wifi expected \r\n");
//        }
//    }

    int len = strlen(ssid);
    wifi_ssid = (char *) calloc(len + 1, sizeof(char));
    memcpy(wifi_ssid, ssid, len);
    wifi_ssid[len] = '\0';

    WifiDeviceConfig ap_config = {0};
    strcpy(ap_config.ssid, ssid);
    strcpy(ap_config.preSharedKey, password);
    ap_config.securityType = WIFI_SEC_TYPE_PSK;

    int result;
    if (AddDeviceConfig(&ap_config, &result) == WIFI_SUCCESS) {

        // connect to wifi
        if (ConnectTo(result) == WIFI_SUCCESS && wait_connect_result() == 1) {
            printf("[wifi_sta]wifi connect success!\r\n");
            g_lwip_netif = netifapi_netif_find(SELECT_WLAN_PORT);
        }
    }

    if (!g_lwip_netif) {
        printf("[wifi_sta]netif error \r\n");
        return -1;
    }

    // start dhcp
    netifapi_set_hostname(g_lwip_netif, hostname, strlen(hostname));
    netifapi_dhcp_start(g_lwip_netif);

    // wait dhcp
    while (1) {
        if (dhcp_is_bound(g_lwip_netif) == ERR_OK) {
            printf("[wifi_sta]dhcp bound success\r\n");

            netifapi_netif_common(g_lwip_netif, dhcp_clients_info_show, NULL);
            break;
        }

        osDelay(100);
    }
    is_connected = 1;
    return WIFI_SUCCESS;
}

int wifi_sta_disconnect(void) {
    if (!g_lwip_netif) {
        printf("[wifi_sta]netif error \r\n");
        return -1;
    }

    WifiErrorCode error = Disconnect();
    if (WIFI_SUCCESS != error) {
        printf("[wifi_sta]disconnect wifi failed, error=%d\r\n", error);
        return error;
    }

    error = DisableWifi();
    if (WIFI_SUCCESS != error) {
        printf("[wifi_sta]disable wifi failed, error=%d\r\n", error);
        return error;
    }

    error = UnRegisterWifiEvent(&wifi_event_handler);
    if (WIFI_SUCCESS != error) {
        printf("[wifi_sta]unregister wifi event failed, error=%d\r\n", error);
        return error;
    }
    printf("[wifi_sta]disconnect wifi success\r\n");
    is_connected = 0;
    return WIFI_SUCCESS;
}

static int get_wifi_local_ip() {
    if (!g_lwip_netif) {
        printf("[wifi_sta]netif error \r\n");
        return -1;
    }

    ip4_addr_t ip_addr;
    ip4_addr_t netmask_addr;
    ip4_addr_t gw_addr;
    if (netifapi_netif_get_addr(g_lwip_netif, &ip_addr, &netmask_addr, &gw_addr) == ERR_OK) {
        u32_t ip = ip_addr.addr;
        u32_t netmask = netmask_addr.addr;
        u32_t gw = gw_addr.addr;

        printf("ip %d.%d.%d.%d\n\r", (ip & 0xff), ((ip >> 8) & 0xff), ((ip >> 16) & 0xff), (ip >> 24));
        printf("netmask %d.%d.%d.%d\n\r", (netmask & 0xff), ((netmask >> 8) & 0xff), ((netmask >> 16) & 0xff),
               (netmask >> 24));
        printf("gw %d.%d.%d.%d\n\r", (gw & 0xff), ((gw >> 8) & 0xff), ((gw >> 16) & 0xff), (gw >> 24));
    } else {
        printf("netif get addr failed\r\n");
    }

    return 1;
}

int wifi_sta_get_addr(unsigned int *ip, unsigned int *netmask, unsigned int *gateway) {
    if (!g_lwip_netif) {
        printf("[wifi_sta]netif error \r\n");
        return 1;
    }

    ip4_addr_t ip_addr;
    ip4_addr_t netmask_addr;
    ip4_addr_t gw_addr;
    if (netifapi_netif_get_addr(g_lwip_netif, &ip_addr, &netmask_addr, &gw_addr) == ERR_OK) {
        *ip = ip_addr.addr;
        *netmask = netmask_addr.addr;
        *gateway = gw_addr.addr;
        return 0;
    }
    return 1;
}

int wifi_sta_get_cache(char *ssid, char *password) {
    genki_nv_sta_cache cache;

    unsigned int ret = hi_nv_read(NV_ID, &cache, sizeof(genki_nv_sta_cache), 0);
    if (ret != 0) {
        return -1;
    }
    if (cache.enable == 0) {
        return -2;
    }

    if (strlen(cache.ssid) == 0) {
        return -3;
    }
    if (strlen(cache.password) == 0) {
        return -4;
    }

    memcpy(ssid, cache.ssid, strlen(cache.ssid));
    memcpy(password, cache.password, strlen(cache.password));
    ssid[strlen(cache.ssid)] = '\0';
    password[strlen(cache.password)] = '\0';
    return 0;
}

int wifi_sta_set_cache(char *ssid, char *password) {
    genki_nv_sta_cache cache;
    cache.enable = 1;
    memcpy(cache.ssid, ssid, strlen(ssid));
    memcpy(cache.password, password, strlen(password));
    cache.ssid[strlen(ssid)] = '\0';
    cache.password[strlen(password)] = '\0';
    unsigned int ret = hi_nv_write(NV_ID, &cache, sizeof(genki_nv_sta_cache), 0);
    if (ret != 0) {
        return -1;
    }
    return 0;
}


int wifi_sta_clear_cache(void) {
    genki_nv_sta_cache cache;
    cache.enable = 0;
    memset(cache.ssid, 0, 1);
    memset(cache.password, 0, 1);
    unsigned int ret = hi_nv_write(NV_ID, &cache, sizeof(genki_nv_sta_cache), 0);
    if (ret != 0) {
        return -1;
    }
    return 0;
}

int wifi_sta_get_ssid(char *ssid) {

    int len = strlen(wifi_ssid);
    memcpy(ssid, wifi_ssid, len);
    ssid[len] = '\0';

    return 0;
}

bool wifi_sta_is_connected(void) {
    return is_connected;
}