#include "genki_thread.h"

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>


static unsigned char count = 0;


int GenkiThread_run(osThreadFunc_t func, void *argument) {
    return GenkiThread_run2(func, argument, 2);
}

int GenkiThread_run2(osThreadFunc_t func, void *argument, unsigned int size) {
    char buf[16];
    sprintf(buf, "T-%d", count++);

    osThreadAttr_t attr;
    memcpy(&(attr.name), buf, strlen(buf));
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * size;
    attr.priority = 25;

    if (osThreadNew(func, argument, &attr) == NULL) {
        printf("Create %s task Failed!\r\n", attr.name);
        return 0;
    }
    return 1;
}
