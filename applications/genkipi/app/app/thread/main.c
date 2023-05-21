//
// 多线程demo
//

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

static void task_1(void) {
    int count = 0;
    while (1) {
        printf("task_1----%d\r\n", count++);
        usleep(0.5 * 1000 * 1000);
    }
}

static void task_2(void) {
    int count = 0;
    while (1) {
        printf("task_2----%d\r\n", count++);
        usleep(1 * 1000 * 1000);
    }
}

static void start(void) {
    osThreadAttr_t attr;

    attr.name = "thread_1";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 4;
    attr.priority = 25;

    if (osThreadNew((osThreadFunc_t) task_1, NULL, &attr) == NULL) {
        printf("Falied to create task_1!\r\n");
    }

    attr.name = "thread_2";

    if (osThreadNew((osThreadFunc_t) task_2, NULL, &attr) == NULL) {
        printf("Falied to create task_2!\n\n");
    }
}

APP_FEATURE_INIT(start);
