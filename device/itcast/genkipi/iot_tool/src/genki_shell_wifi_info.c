#include "genki_shell_wifi_info.h"
#include <stdio.h>

#include "genki_wifi_ap.h"
#include "genki_wifi_sta.h"

static char *ip4_str(unsigned int src) {
    char p[16] = {0};
    sprintf(p, "%d.%d.%d.%d", (src & 0xff), (src >> 8) & 0xff, (src >> 16) & 0xff, (src >> 24) & 0xff);

    int len = strlen(p);
    char *dst = (char *) calloc(len + 1, sizeof(char));
    memcpy(dst, p, len);
    dst[len] = '\0';

    return dst;
}

static int doPipeline(genki_shell_pipe_t *pipe) {
    unsigned int a_ip, a_netmask, a_gateway;
    unsigned int ret = wifi_ap_get_addr(&a_ip, &a_netmask, &a_gateway);

    char buf[512] = {0};

    if (ret != 0) {
        printf("GET WIFI AP Info Error\n");
        return -1;
    }

    char *str_aip = ip4_str(a_ip);
    char *str_anetmask = ip4_str(a_netmask);
    char *str_agateway = ip4_str(a_gateway);
    char aname[32] = {0};
    wifi_ap_get_ssid(aname);

    printf("============ WIFI AP Info =============\n");
    printf("IP: %s\n", str_aip);
    printf("Gateway: %s\n", str_agateway);
    printf("Netmask: %s\n", str_anetmask);

    if (wifi_sta_is_connected()) {
        unsigned int s_ip, s_netmask, s_gateway;
        unsigned int ret = wifi_sta_get_addr(&s_ip, &s_netmask, &s_gateway);
        if (ret == 0) {

            char *str_sip = ip4_str(s_ip);
            char *str_snetmask = ip4_str(s_netmask);
            char *str_sgateway = ip4_str(s_gateway);
            char sname[32] = {0};
            wifi_sta_get_ssid(sname);

            printf("============ WIFI STA Info =============\n");
            printf("IP: %s\n", str_sip);
            printf("Gateway: %s\n", str_sgateway);
            printf("Netmask: %s\n", str_snetmask);

            free(str_sip);
            free(str_snetmask);
            free(str_sgateway);
        }
    }

    free(str_aip);
    free(str_anetmask);
    free(str_agateway);
    return 0;
}



genki_shell_service_t *newWifiInfoShell(void) {
    genki_shell_service_t *service = genki_shell_newService("wifiinfo");
    service->doPipeline = doPipeline;
    return service;
}

