#include "genki_shell_wifi_sta.h"
#include <stdio.h>

#include "genki_wifi_ap.h"
#include "genki_wifi_sta.h"


static int doPipeline(genki_shell_pipe_t *pipe) {
    printf("input wifi name: \n");


    char buf[512];
    while (1) {
        // read wifi name
        int num = pipe->readLine(buf, 512);
        if (num <= 0) {
            continue;
        }
        char name[num];
        memcpy(name, buf, num);
        name[num] = '\0';

        // read wifi password
        printf("input wifi password: \n");

        num = pipe->readLine(buf, 512);
        char password[num];
        memcpy(password, buf, num);
        password[num] = '\0';


        if (wifi_sta_connect(name, password, "GenkiPI") == 0) {
            // connect success
            wifi_sta_set_cache(name, password);

            printf("WIFI STA connected Success!!!\n");
        } else {
            printf("WIFI STA connected Failed!!!\n");
        }
        break;
    }

    return 0;
}



genki_shell_service_t *newWifiStaShell(void) {
    genki_shell_service_t *service = genki_shell_newService("sta");
    service->doPipeline = doPipeline;
    return service;
}

