#include "genki/car/oled.h"
#include "ssd1306_i2c.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "iot_errno.h"
#include "iot_gpio.h"
#include "iot_i2c.h"
#include "iot_io.h"

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "genki/car/config.h"


#define IDX 0
#define ADDRESS_READ 0x78
#define ADDRESS_WRITE 0x79

static unsigned char is_oled_init = 0;
static unsigned char is_oled_enable = 2;
static osMutexId_t mutex_id = NULL;

static void lock_i2c(void) {
    osMutexAcquire(mutex_id, osWaitForever);
}

static void unlock_i2c(void) {
    osMutexRelease(mutex_id);
}

static void init(void) {
    if (is_oled_init) return;

    // get mutex lock id
    mutex_id = CarConfig_getI2cMutexId();

    lock_i2c();
    OLED_Init();
    unlock_i2c();

    is_oled_init = 1;
}

unsigned char CarOLED_isEnable(void) {
    if(is_oled_enable != 2) return is_oled_enable;

    lock_i2c();
    unsigned char data[1] = {0};
    unsigned int ret = IoTI2cRead(IDX, ADDRESS_READ, data, 1);
    unlock_i2c();
    if (ret != IOT_SUCCESS) {
        is_oled_enable = 0;
        return 0;
    }
    is_oled_enable = 1;
    return 1;
}

unsigned char CarOLED_clear(void) {
    init();

    lock_i2c();
    OLED_Fill(0x00);
    unlock_i2c();
}

unsigned char CarOLED_showRpm(double left, double right) {
    init();

    lock_i2c();
    OLED_Fill_Row(0x00, 2);
    OLED_Fill_Row(0x00, 3);
    unlock_i2c();

    char* output = malloc(sizeof(char) * 50);

    snprintf(output, 50, "%.2f %.2f", left, right);
//            CarOLED_clear();

    CarOLED_showStr(0, 2, output, 2);
}

unsigned char CarOLED_showIP(unsigned int ip) {
    lock_i2c();
    OLED_Fill_Row(0x00, 0);
    unlock_i2c();

    int size = 19;
    char p[size];
    snprintf(p, size, "%d.%d.%d.%d", (ip & 0xff), (ip >> 8) & 0xff, (ip >> 16) & 0xff, (ip >> 24) & 0xff);

    CarOLED_showStr(0, 0, p, 1);
}

unsigned char CarOLED_showStr(unsigned char x, unsigned char y, unsigned char* text, unsigned char text_size) {
    init();

    lock_i2c();
    OLED_ShowStr(x, y, text, text_size);
    unlock_i2c();

    free(text);
}