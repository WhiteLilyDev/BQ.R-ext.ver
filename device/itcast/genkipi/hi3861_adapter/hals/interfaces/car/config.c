
#include "genki/car/config.h"

#include <stdio.h>

#include "iot_errno.h"
#include "iot_gpio.h"
#include "iot_i2c.h"
#include "iot_io.h"

#include "genki_pin.h"


#define IDX 0

#define MOTOR_LEFT_H1 IOT_IO_NAME_7
#define MOTOR_LEFT_H2 IOT_IO_NAME_11
#define MOTOR_LEFT_M1 IOT_IO_NAME_14
#define MOTOR_LEFT_M2 IOT_IO_NAME_8
#define MOTOR_LEFT_M1_PORT IOT_PWM_NAME_5
#define MOTOR_LEFT_M2_PORT IOT_PWM_NAME_1
#define MOTOR_LEFT_DIR -1

#define MOTOR_RIGHT_H1 IOT_IO_NAME_12
#define MOTOR_RIGHT_H2 IOT_IO_NAME_5
#define MOTOR_RIGHT_M1 IOT_IO_NAME_6
#define MOTOR_RIGHT_M2 IOT_IO_NAME_13
#define MOTOR_RIGHT_M1_PORT IOT_PWM_NAME_3
#define MOTOR_RIGHT_M2_PORT IOT_PWM_NAME_4
#define MOTOR_RIGHT_DIR 1

static unsigned char is_gpio_init = 0;
static unsigned char is_i2c_init = 0;

static osMutexId_t i2c_mutex_id = NULL;
static unsigned char is_i2c_mutex_init = 0;


static void gpio_init(void) {
    if (is_gpio_init) return;

    // Motor
    IoTGpioInit(MOTOR_LEFT_M1);
    IoTIoSetFunc(MOTOR_LEFT_M1, 0);
    IoTGpioSetDir(MOTOR_LEFT_M1, IOT_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(MOTOR_LEFT_M1, IOT_GPIO_VALUE0);

    IoTGpioInit(MOTOR_LEFT_M2);
    IoTIoSetFunc(MOTOR_LEFT_M2, 0);
    IoTGpioSetDir(MOTOR_LEFT_M2, IOT_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(MOTOR_LEFT_M2, IOT_GPIO_VALUE0);

    IoTGpioInit(MOTOR_RIGHT_M1);
    IoTIoSetFunc(MOTOR_RIGHT_M1, 0);
    IoTGpioSetDir(MOTOR_RIGHT_M1, IOT_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(MOTOR_RIGHT_M1, IOT_GPIO_VALUE0);

    IoTGpioInit(MOTOR_RIGHT_M2);
    IoTIoSetFunc(MOTOR_RIGHT_M2, 0);
    IoTGpioSetDir(MOTOR_RIGHT_M2, IOT_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(MOTOR_RIGHT_M2, IOT_GPIO_VALUE0);

    is_gpio_init = 1;
}


static void i2c_init(void) {
    if (is_i2c_init) return;

    IoTI2cInit(IDX, 400000);
    IoTIoSetFunc(IOT_IO_NAME_10, IOT_IO_FUNC_10_I2C0_SDA);
    IoTIoSetFunc(IOT_IO_NAME_9, IOT_IO_FUNC_9_I2C0_SCL);

    is_i2c_init = 1;
}

static void i2c_mutex_init(void) {
    if (is_i2c_mutex_init) return;

    i2c_mutex_id = osMutexNew(NULL);
    if (i2c_mutex_id == NULL) {
        printf("Falied to create i2c Mutex!\n");
    } else {
        is_i2c_mutex_init = 1;
    }
}

void CarConfig_init(void) {
    gpio_init();
    i2c_init();
    i2c_mutex_init();
}

osMutexId_t CarConfig_getI2cMutexId(void) {
    return i2c_mutex_id;
}

void CarConfig_getMotorCfg(CarMotorCfg *leftCfg, CarMotorCfg *rightCfg) {
    CarMotorCfg lc;
    lc.pin_h1 = MOTOR_LEFT_H1;
    lc.pin_h2 = MOTOR_LEFT_H2;
    lc.pin_m1 = MOTOR_LEFT_M1;
    lc.pin_m2 = MOTOR_LEFT_M2;
    lc.port_m1 = MOTOR_LEFT_M1_PORT;
    lc.port_m2 = MOTOR_LEFT_M2_PORT;
    lc.motor_dir = MOTOR_LEFT_DIR;

    CarMotorCfg rc;
    rc.pin_h1 = MOTOR_RIGHT_H1;
    rc.pin_h2 = MOTOR_RIGHT_H2;
    rc.pin_m1 = MOTOR_RIGHT_M1;
    rc.pin_m2 = MOTOR_RIGHT_M2;
    rc.port_m1 = MOTOR_RIGHT_M1_PORT;
    rc.port_m2 = MOTOR_RIGHT_M2_PORT;
    rc.motor_dir = MOTOR_RIGHT_DIR;

    *leftCfg = lc;
    *rightCfg = rc;
}