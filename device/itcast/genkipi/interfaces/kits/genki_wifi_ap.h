#ifndef GENKIPI_GENKI_WIFI_AP_H
#define GENKIPI_GENKI_WIFI_AP_H

#include <stdint.h>

/**
 * start wifi ap
 * @param ssid
 * @param password
 * @return 0 success
 */
int wifi_ap_start(const char* ssid, const char* password);

/**
 * stop wifi ap
 * @return 0 success
 */
void wifi_ap_stop(void);

/**
 *
 * @param ip
 * @param netmask
 * @param gateway
 * @return 0 success
 */
int wifi_ap_get_addr(unsigned int *ip, unsigned int *netmask, unsigned int *gateway);

int wifi_ap_get_ssid(char *ssid);

int wifi_ap_get_cache(char *ssid, char *password);

int wifi_ap_set_cache(char *ssid, char *password);

int wifi_ap_clear_cache(void);


#endif //GENKIPI_GENKI_WIFI_AP_H