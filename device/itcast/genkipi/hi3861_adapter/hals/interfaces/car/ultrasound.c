#include "genki/car/ultrasound.h"

#include <stdio.h>
#include <unistd.h>

#include "iot_errno.h"
#include "iot_gpio.h"
#include "iot_i2c.h"
#include "iot_io.h"

#include <sys/time.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "genki/car/config.h"


#define IDX 0
#define ADDRESS_READ 0xaf
#define ADDRESS_WRITE 0xae

static unsigned char is_init = 0;
static osMutexId_t mutex_id = NULL;


static void lock_i2c(void) {
    osMutexAcquire(mutex_id, osWaitForever);
}

static void unlock_i2c(void) {
    osMutexRelease(mutex_id);
}

static void init(void) {
    if (is_init) return;

    // get mutex lock id
    mutex_id = CarConfig_getI2cMutexId();

    is_init = 1;
}


static unsigned int read24(unsigned char *val) {
    unsigned int dataLen = 3;
    unsigned int ret = IoTI2cRead(IDX, ADDRESS_READ, val, dataLen);
    if (ret != IOT_SUCCESS) {
        printf("[ULTRANSOUND]Read24 error: %x\r\n", ret);
        return ret;
    }

    return IOT_SUCCESS;
}

static unsigned int write8(unsigned char reg) {
    unsigned char send_data[1] = {reg};

    unsigned int ret = IoTI2cWrite(IDX, ADDRESS_WRITE, send_data, 1);

    if (ret != IOT_SUCCESS) {
        printf("[ULTRANSOUND]Write8 error: %x\r\n", ret);
        return ret;
    }
    return IOT_SUCCESS;
}

unsigned char CarUltrasound_isEnable(void) {
    init();

    lock_i2c();
    if(write8(0x01) == 0) {
        unlock_i2c();
        return 1;
    }
    unlock_i2c();
    return 0;
}

unsigned char CarUltrasound_measure(unsigned int *distance){
    init();

    lock_i2c();
    write8(0x01);
    unlock_i2c();

    usleep(200 * 1000);

    lock_i2c();
    unsigned char data[3];
    read24(data);
    unlock_i2c();

    int a = ((data[0] << 16) + (data[1] << 8) + data[2]) / 1000;

//    printf("xx: %d %d %d %d %d\r\n", reg, a, data[0], data[1], data[2]);
//    printf("b%d %d %d %d\r\n", a, data[0], data[1], data[2]);

    *distance = a;
    if(a >= 10 && a <= 9000) {
        return 1;
    }
    return 0;
}

