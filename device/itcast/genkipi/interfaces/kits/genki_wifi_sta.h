#ifndef GENKIPI_GENKI_WIFI_STA_H
#define GENKIPI_GENKI_WIFI_STA_H

/**
 * link wifi
 * @param ssid
 * @param password
 * @param hostname
 * @return 0 success
 */
int wifi_sta_connect(const char* ssid, const char* password, const char* hostname);
/**
 * disconnect wifi
 * @return 0 success
 */
int wifi_sta_disconnect(void);

/**
 * get wifi addr
 * @param ip
 * @param netmask
 * @param gateway
 * @return 0 success
 */
int wifi_sta_get_addr(unsigned int *ip, unsigned int *netmask, unsigned int *gateway);


int wifi_sta_get_cache(char *ssid, char *password);

int wifi_sta_set_cache(char *ssid, char *password);

int wifi_sta_clear_cache(void);

int wifi_sta_get_ssid(char *ssid);

bool wifi_sta_is_connected(void);


#endif //GENKIPI_GENKI_WIFI_STA_H