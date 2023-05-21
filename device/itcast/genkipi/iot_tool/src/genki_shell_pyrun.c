#include "genki_shell_pyrun.h"

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "utils_file.h"
#include "hi_nv.h"
#include "hi_reset.h"

#include "ohos_init.h"
#include "cmsis_os2.h"

static osThreadId_t tid = NULL;

typedef struct {
    hi_u8 name[32];
} genki_nv_py_exec_cache;
#define NV_ID 0x84


static void run_task(void) {
    char *tmp = "tmp.py";
    printf("[python code flash] run code\r\n");
    DTPython_Init();
    DTPython_RunFile(tmp);
    DTPython_Deinit();
    printf("[python code flash] finish code\r\n");
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
    tid = osThreadNew((osThreadFunc_t) run_task, NULL, &attr);
    if (tid == NULL) {
        printf("Create run task Failed!\r\n");
    }
}

static int set_exec_name(char *name) {
    genki_nv_py_exec_cache cache;
    memcpy(cache.name, name, strlen(name));
    cache.name[strlen(name)] = '\0';
    return hi_nv_write(NV_ID, &cache, sizeof(genki_nv_py_exec_cache), 0);
}

static int get_file_name(genki_shell_pipe_t *pipe, char *buf, int size) {
    int i = 0;
    char c = '\0';
    int n;

    while (i < size - 1) {
        if(c == 2) {
            break;
        }
        n = pipe->read(&c, 1);
        if (n > 0) {
            if (c != 2) {
                buf[i] = c;
                i++;
            }
        }
    }
    buf[i] = '\0';
    return (i);
}

static int* get_file_content(genki_shell_pipe_t *pipe, char *buf, int size) {
    int i = 0;
    char c = '\0';
    int n;
    int f = 0;

    while (i < size - 1) {
        if (c == 4) {
            f = 1;
            break;
        }
        n = pipe->read(&c, 1);
        if (n > 0) {
            if (c == 3) {
                n = pipe->read(&c, 1);
                if ((n > 0) && (c == 4)) {
                    buf[i] = c;
                } else {
                    buf[i] = 3;
                    i++;
                    buf[i] = c;
                    i++;
                }
            } else if (c == 4) {
                buf[i] = c;
            } else {
                buf[i] = c;
                i++;
            }
        } else {
            c = 4;
            i++;
        }
    }
    buf[i] = '\0';
    int *r = malloc(2 * sizeof(int));
    r[0] = i;
    r[1] = f;
    return r;
}

static int doPipeline(genki_shell_pipe_t *pipe) {
    char *tmp = "tmp.py";
    char recv_buf[1024] = {0};
    int recv_len;
    char c;
    unsigned int n;

    n = pipe->read(&c, 1);
    printf("[python code flash] start...\r\n");

    if (n <= 0) {
        printf("[python code flash] error[-1]\r\n");
        return;
    }

    if (c == 1) {
        // response
        printf("[python code flash] read type\r\n");

        char flash = 1;
        n = pipe->read(&c, 1);
        if (c == 0) {
            // run
            flash = 0;
        }

        // start head
        n = get_file_name(pipe, recv_buf, 1024);

        // response
        printf("[python code flash] read file name\r\n");

        char name[n + 1];
        memcpy(name, recv_buf, n);
        name[n] = '\0';
        printf("name: %s\r\n", recv_buf);
        int fd = UtilsFileOpen(tmp, O_CREAT_FS | O_TRUNC_FS | O_WRONLY_FS, 0);
        if (fd < 0) {
            // response
            printf("[python code flash] open tmp file error\r\n");
            return;
        }

        // response
        printf("[python code flash] transporting\r\n");
        unsigned int offset = 0;
        while (1) {
            int* r = get_file_content(pipe, recv_buf, 1024);
            n = r[0];
            int f = r[1];
            free(r);

            int ret = UtilsFileSeek(fd, offset, 0);
            ret = UtilsFileWrite(fd, recv_buf, n);
            offset += n;

            printf("#");

            if (f) {
                break;
            }
        }
        printf(" 100%c\r\n", '%');

        // response
        printf("[python code flash] transported\r\n");

        //关闭文件
        int ret = UtilsFileClose(fd);
        if (ret != 0) {
            // response
            printf("[python code flash] close tmp file error\r\n");
            return;
        }

        if (flash) {
            // 删除原来的app.py
            ret = UtilsFileDelete(name);
            //将temp.py移动为app.py
            ret = UtilsFileMove(tmp, name);
            ret = UtilsFileDelete(tmp);

            set_exec_name(name);

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
        } else {
            if (tid) {
                osStatus_t status = osThreadTerminate(tid);
                printf("stop: %d\r\n", status);
            }
            pyrun_task();
        }
    } else {
        // response
        printf("[python code flash] error[-2], %d\r\n", c);
    }

    return 1;
}


genki_shell_service_t* newPyrunShell(void) {
    genki_shell_service_t *service = genki_shell_newService("pyrun");
    service->doPipeline = doPipeline;
    return service;
}
