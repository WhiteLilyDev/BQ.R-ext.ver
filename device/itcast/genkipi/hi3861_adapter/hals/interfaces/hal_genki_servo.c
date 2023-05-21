#include "genki/Servo.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include "genki_pin.h"
#include "iot_gpio.h"
#include "iot_io.h"
#include "hi_time.h"


#define DUTY_START 500
#define DUTY_END 2500
#define ANGLE_START 0
#define ANGLE_END 180
#define PWM_CTRL 20000


void GenkiServo_setAngle(unsigned char pin, int angle) {
    unsigned char func = 0;
    if(pin == 13 || pin == 14) {
        func = 4;
    }

    if(angle < 0) {
        angle = angle % 180 + 180;
    }

    if (angle > 180) {
        angle = angle % 180;
    }

    // 初始化GPIO端口
    IoTGpioInit(pin);
    // 设置IO口功能为GPIO
    IoTIoSetFunc(pin, func);
    // 设置IO口输出方式：输出
    IoTGpioSetDir(pin, IOT_GPIO_DIR_OUT);

    uint32_t duty = DUTY_START + (DUTY_END - DUTY_START) * angle / 180;
    uint32_t count = 50;
    for (int i = 0; i < count; i++) {
        IoTGpioSetOutputVal(pin, IOT_GPIO_VALUE1);
        hi_udelay(duty);
        IoTGpioSetOutputVal(pin, IOT_GPIO_VALUE0);
        hi_udelay(PWM_CTRL - duty);
    }
//    IoTGpioSetOutputVal(pin, IOT_GPIO_VALUE1);
//    hi_udelay(5);
//    IoTGpioSetOutputVal(pin, IOT_GPIO_VALUE0);
//    hi_udelay(5);

    IoTGpioDeinit(pin);
}