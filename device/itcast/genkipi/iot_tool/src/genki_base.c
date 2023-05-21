#include "genki_base.h"

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "genki_wifi_ap.h"
#include "genki_wifi_sta.h"

#include "genki_web.h"
#include "genki_web_base.h"
#include "genki_web_pyrun.h"

#include "genki_shell.h"
#include "genki_shell_python.h"
#include "genki_shell_pyrun.h"
#include "genki_shell_wifi_sta.h"
#include "genki_shell_wifi_info.h"

#include "utils_file.h"
#include "hi_nv.h"
#include "hi_reset.h"
#include "hi_wifi_api.h"


typedef struct {
    hi_u8 name[32];
} genki_nv_py_exec_cache;
#define NV_ID 0x84

static int get_exec_name(char *name) {
    genki_nv_py_exec_cache cache;

    unsigned int ret = hi_nv_read(NV_ID, &cache, sizeof(genki_nv_py_exec_cache), 0);
    if (ret != 0) {
        return -1;
    }

    if (strlen(cache.name) == 0) {
        return -2;
    }

    memcpy(name, cache.name, strlen(cache.name));
    name[strlen(cache.name)] = '\0';

    return 0;
}

static void do_pyrun(void) {
    char name[32];
    int ret = get_exec_name(name);
    if (ret != 0) return;

    printf("get name: %s %lu\r\n", name, strlen(name));

    unsigned int fileSize;
    ret = UtilsFileStat(name, &fileSize);
    if (ret < 0) {
        printf("Open Python File Failed: %s\r\n", name);
        return;
    }

    DTPython_Init();   // 初始化Python环境
    //执行程序
    DTPython_RunFile(name);
    DTPython_Deinit(); // 清理Python环境
}

static void pyrun_task(void) {
    osThreadAttr_t attr;

    attr.name = "run_task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 4;
    attr.priority = 25;
    //主要负责接收服务器的数据
    if (osThreadNew((osThreadFunc_t) do_pyrun, NULL, &attr) == NULL) {
        printf("Create run task Failed!\r\n");
    }

}

static void start_ap(void) {
    char ssid[32];
    char password[64];
    if (wifi_ap_get_cache(ssid, password) == 0) {
        if (wifi_ap_start(ssid, password) == 0) {
            printf("======== AP start success ======= \n");
            printf("AP Name: %s\n", ssid);
            printf("AP Password: %s\n", password);
        } else {
            printf("======== AP start Failed ======= \n");
        }
    } else {
        hi_uchar mac[6] = {0};
        int ret = hi_wifi_get_macaddr((hi_char*)mac, 6);
        if (ret == 0) {
            printf("mac: %02x:%02x:%02x:%02x:%02x:%02x \n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        }

        char ssid[12];
        snprintf(ssid, 12, "itcast_%02x%02x", mac[4], mac[5]);

        if (wifi_ap_start(ssid, "12345678") == 0) {
            printf("======== AP start success ======= \n");
            printf("AP Name: %s\n", ssid);
            printf("AP Password: %s\n", "12345678");
        } else {
            printf("======== AP start Failed ======= \n");
        }
    }
}

static void start_sta() {
    char ssid[32] = {0};
    char password[64] = {0};
    if (wifi_sta_get_cache(ssid, password) == 0) {
        if (wifi_sta_connect(ssid, password, "GenkiPI") != 0) {
            // connect error
            // wifi_sta_clear_cache();
        }
    }
}

static void web_task(void) {

    start_ap();
    start_sta();

    genki_web_register(newBaseService());
    genki_web_register(newPyrunService());

    genki_web_start();

    pyrun_task();
}

static void shell_task(void) {
    genki_shell_register(newPythonShell());
    genki_shell_register(newPyrunShell());
    genki_shell_register(newWifiStaShell());
    genki_shell_register(newWifiInfoShell());

    genki_shell_start();
}

int genki_services_start(void) {
    osThreadAttr_t attr;
    attr.name = "web";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 10;
    attr.priority = 25;

    if (osThreadNew((osThreadFunc_t) web_task, NULL, &attr) == NULL) {
        printf("Create web task Failed!\r\n");
    }

    attr.name = "shell";
    attr.stack_size = 1024 * 10;
    attr.priority = 25;

    if (osThreadNew((osThreadFunc_t) shell_task, NULL, &attr) == NULL) {
        printf("Create shell task Failed!\r\n");
    }
}

