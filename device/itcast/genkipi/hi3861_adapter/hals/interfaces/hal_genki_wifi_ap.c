#include "genki_wifi_ap.h"

#include "wifi_hotspot.h"
#include "cmsis_os2.h"
#include "lwip/netifapi.h"
#include <string.h>

#include "hi_nv.h"

static struct netif *g_iface = NULL;
static volatile int g_hotspotStarted = 0;
static volatile int g_joinedStations = 0;

static char *wifi_ssid = NULL;

typedef struct {
    hi_u8 ssid[33];
    hi_u8 password[65];
} genki_nv_ap_cache;

#define NV_ID 0x85

static void OnHotspotStateChanged(int state) {
    printf("[Debug]OnHotspotStateChanged: %d.\r\n", state);
    if (state == WIFI_HOTSPOT_ACTIVE) {
        g_hotspotStarted = 1;
    } else {
        g_hotspotStarted = 0;
    }
}

static void OnHotspotStaJoin(StationInfo *info) {
    g_joinedStations++;
    printf("[Debug]+OnHotspotStaJoin: active stations = %d.\r\n", g_joinedStations);
}

static void OnHotspotStaLeave(StationInfo *info) {
    g_joinedStations--;
    printf("[Debug]-OnHotspotStaLeave: active stations = %d.\r\n", g_joinedStations);
}

WifiEvent g_defaultWifiEventListener = {
        .OnHotspotStaJoin = OnHotspotStaJoin,
        .OnHotspotStaLeave = OnHotspotStaLeave,
        .OnHotspotStateChanged = OnHotspotStateChanged,
};

int wifi_ap_start(const char *ssid, const char *password) {
    WifiErrorCode err_code = WIFI_SUCCESS;

    err_code = RegisterWifiEvent(&g_defaultWifiEventListener);
    printf("[Debug]RegisterWifiEvent:%d\r\n", err_code);

    int len = strlen(ssid);
    wifi_ssid = (char *) calloc(len + 1, sizeof(char));
    memcpy(wifi_ssid, ssid, len);
    wifi_ssid[len] = '\0';

    HotspotConfig config = {0};
    strcpy(config.ssid, ssid);
    strcpy(config.preSharedKey, password);
    config.securityType = WIFI_SEC_TYPE_PSK;
    config.band = HOTSPOT_BAND_TYPE_2G;
    config.channelNum = 7;

    err_code = SetHotspotConfig(&config);
    printf("[Debug]SetHotspotConfig:%d\r\n", err_code);

    g_hotspotStarted = 0;
    err_code = EnableHotspot();
    printf("[Debug]EnableHotspot: %d\r\n", err_code);

    while (!g_hotspotStarted) {
        osDelay(10);
    }

    printf("[Debug]g_hotspotStarted = %d.\r\n", g_hotspotStarted);

    g_iface = netifapi_netif_find("ap0");
    if (g_iface) {
        ip4_addr_t ipaddr, gateway, netmask;

        IP4_ADDR(&ipaddr, 192, 168, 10, 1);
        IP4_ADDR(&gateway, 192, 168, 10, 1);
        IP4_ADDR(&netmask, 255, 255, 255, 0);
        err_t ret = netifapi_netif_set_addr(g_iface, &ipaddr, &netmask, &gateway);
        printf("[Debug]netifapi_netif_set_addr: %d\r\n", ret);

        ret = netifapi_dhcps_start(g_iface, 0, 0);
        printf("[Debug]netifapi_dhcp_start: %d\r\n", ret);
    }
    return err_code;
}

void wifi_ap_stop(void) {
    if (g_iface) {
        err_t ret = netifapi_dhcps_stop(g_iface);
        printf("[Debug]netifapi_dhcps_stop: %d\r\n", ret);
    }

    WifiErrorCode err_code = UnRegisterWifiEvent(&g_defaultWifiEventListener);
    printf("[Debug]UnRegisterWifiEvent: %d\r\n", err_code);

    err_code = DisableHotspot();
    printf("EnableHotspot: %d\r\n", err_code);
}

int wifi_ap_get_addr(unsigned int *ip, unsigned int *netmask, unsigned int *gateway) {

    // 0xC0A80A01
    *ip = 0x010AA8C0;
    // 0xFFFFFF00
    *netmask = 0x00FFFFFF;
    // 0xC0A80A01
    *gateway = 0x010AA8C0;

    return WIFI_SUCCESS;
}

int wifi_ap_get_ssid(char *ssid) {
    int len = strlen(wifi_ssid);
    memcpy(ssid, wifi_ssid, len);
    ssid[len] = '\0';

    return 0;
}

int wifi_ap_get_cache(char *ssid, char *password) {
    genki_nv_ap_cache cache;

    unsigned int ret = hi_nv_read(NV_ID, &cache, sizeof(genki_nv_ap_cache), 0);
    if (ret != 0) {
        return -1;
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

int wifi_ap_set_cache(char *ssid, char *password) {
    genki_nv_ap_cache cache;
    memcpy(cache.ssid, ssid, strlen(ssid));
    memcpy(cache.password, password, strlen(password));
    cache.ssid[strlen(ssid)] = '\0';
    cache.password[strlen(password)] = '\0';
    unsigned int ret = hi_nv_write(NV_ID, &cache, sizeof(genki_nv_ap_cache), 0);
    if (ret != 0) {
        return -1;
    }
    return 0;
}

int wifi_ap_clear_cache(void) {
    genki_nv_ap_cache cache;
    memset(cache.ssid, 0, 1);
    memset(cache.password, 0, 1);
    unsigned int ret = hi_nv_write(NV_ID, &cache, sizeof(genki_nv_ap_cache), 0);
    if (ret != 0) {
        return -1;
    }
    return 0;
}