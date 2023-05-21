#include "genki_web_pyrun.h"

#include<stdio.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "utils_file.h"
#include "dtpython.h"
#include "hi_nv.h"
#include "hi_reset.h"


typedef struct {
    hi_u8 name[32];
} genki_nv_py_exec_cache;
#define NV_ID 0x84

static void run_task(void) {
    char *tmp = "tmp.py";
    printf("[pyrun] run code\r\n");
    DTPython_Init();
    DTPython_RunFile(tmp);
    DTPython_Deinit();
    printf("[pyrun] finish code\r\n");
}

static void pyrun_task(void) {
    osThreadAttr_t attr;

    attr.name = "pyrun_task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 10;
    attr.priority = 25;
    //主要负责接收服务器的数据
    if (osThreadNew((osThreadFunc_t) run_task, NULL, &attr) == NULL) {
        printf("Create py run task Failed!\r\n");
    }
}

static void do_reboot(void) {
    printf("Flash Finish! Ready Reboot\r\n");
    usleep(1000 * 1000);
    printf("Reboot 3\r\n");
    usleep(1000 * 1000);
    printf("Reboot 2\r\n");
    usleep(1000 * 1000);
    printf("Reboot 1\r\n");
    usleep(1000 * 1000);
    printf("Rebooting...\r\n");
    usleep(1000 * 1000);

    hi_soft_reboot(5);
}

static void reboot_task(void) {
    osThreadAttr_t attr;

    attr.name = "reboot_task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 4;
    attr.priority = 25;
    //主要负责接收服务器的数据
    if (osThreadNew((osThreadFunc_t) do_reboot, NULL, &attr) == NULL) {
        printf("Create py run task Failed!\r\n");
    }
}

static int set_exec_name(char *name) {
    genki_nv_py_exec_cache cache;
    memcpy(cache.name, name, strlen(name));
    cache.name[strlen(name)] = '\0';
    return hi_nv_write(NV_ID, &cache, sizeof(genki_nv_py_exec_cache), 0);
}

static int execute(genki_web_request_t *request, genki_web_response_t *response, int flash) {
    if (request->method == GET) return -1;

    char *name = request->getHeader(request, "pyname");
    if (name == NULL) {
        response->write(response, "-1", 2);
        return -1;
    }

    int contentLength = 0;
    char *cl = request->getHeader(request, "Content-Length");
    if (cl == NULL) {
        response->write(response, "-2", 2);
        return -1;
    }
    contentLength = atoi(cl);

    char buf[1024];
    int num;
    int ret;
    int offset = 0;

    char *tmp = "tmp.py";
    int fd = UtilsFileOpen(tmp, O_CREAT_FS | O_TRUNC_FS | O_WRONLY_FS, 0);
    if (fd < 0) {
        // response
        response->write(response, "-3", 2);
        return -1;
    }

    while (offset < contentLength) {
        num = request->read(request, buf, 1024);
        if (num <= 0) {
            break;
        }
        ret = UtilsFileSeek(fd, offset, 0);
        ret = UtilsFileWrite(fd, buf, num);

        offset += num;
    }

    //关闭文件
    ret = UtilsFileClose(fd);
    if (ret != 0) {
        // response
        response->write(response, "-3", 2);
        return -1;
    }

    if (flash) {
        ret = UtilsFileDelete(name);
        ret = UtilsFileMove(tmp, name);
        ret = UtilsFileDelete(tmp);

        set_exec_name(name);

        reboot_task();
    } else {
        pyrun_task();
    }
    // response
    response->write(response, "1", 1);
    return 1;
}

static int doPyflash(genki_web_request_t *request, genki_web_response_t *response) {
    return execute(request, response, 1);
}

static int doPyrun(genki_web_request_t *request, genki_web_response_t *response) {
    return execute(request, response, 0);
}


genki_web_service_t *newPyrunService(void) {
    genki_web_service_t *service = genki_web_newService("PYRUN");
    service->addFilter(service, "/pyrun/run", doPyrun);
    service->addFilter(service, "/pyrun/flash", doPyflash);
    return service;
}
