#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "iot_gpio.h"
#include "iot_io.h"
#include "iot_pwm.h"

#include "genki_pin.h"
#include "genki_led.h"


static unsigned char led_init = 0;
static unsigned char led_state = 0;
static unsigned char led_pwm_init = 0;


static void GenkiLedInit(void) {
    // 初始化GPIO端口
    IoTGpioInit(IOT_IO_NAME_2);

    // 设置IO口功能为GPIO
    IoTIoSetFunc(IOT_IO_NAME_2, IOT_IO_FUNC_2_GPIO);

    // 设置IO口输出方式：输出
    IoTGpioSetDir(IOT_IO_NAME_2, IOT_GPIO_DIR_OUT);

    IoTGpioSetOutputVal(IOT_IO_NAME_2, IOT_GPIO_VALUE0);
    IoTGpioGetOutputVal(IOT_IO_NAME_2, &led_state);
}

void GenkiLedToggle(void) {
    if (led_init == 0) {
        GenkiLedInit();
        led_init = 1;
        if(led_pwm_init != 0) {
            GenkiLedPwmStop();
        }
        led_pwm_init = 0;
    }

    if (led_state == IOT_GPIO_VALUE0) {
        // to high
        IoTGpioSetOutputVal(IOT_IO_NAME_2, IOT_GPIO_VALUE1);
        IoTGpioGetOutputVal(IOT_IO_NAME_2, &led_state);
    } else {
        // to low
        IoTGpioSetOutputVal(IOT_IO_NAME_2, IOT_GPIO_VALUE0);
        IoTGpioGetOutputVal(IOT_IO_NAME_2, &led_state);
    }
}

void GenkiLedState(unsigned char *state) {
    IoTGpioGetOutputVal(IOT_IO_NAME_2, &led_state);
    *state = led_state;
}

void GenkiLedOpen(void) {
    if (led_state == IOT_GPIO_VALUE0) {
        GenkiLedToggle();
    }
}

void GenkiLedClose(void) {
    if (led_state == IOT_GPIO_VALUE1) {
        GenkiLedToggle();
    }
}

static void GenkiLedPwmInit(void) {
    // 初始化GPIO端口
    IoTGpioInit(IOT_IO_NAME_2);

    // 设置IO口功能为GPIO
    IoTIoSetFunc(IOT_IO_NAME_2, IOT_IO_FUNC_2_PWM2_OUT);

    // 设置IO口输出方式：输出
    IoTGpioSetDir(IOT_IO_NAME_2, IOT_GPIO_DIR_OUT);

    // 初始化PWM
    IoTPwmInit(IOT_IO_NAME_2);
}

void GenkiLedPwmStart(unsigned short duty, unsigned int freq) {
    if(led_pwm_init == 0) {
        GenkiLedPwmInit();
        led_pwm_init = 1;
        led_init = 0;
    }

    IoTPwmStart(IOT_IO_NAME_2, duty, freq);
}

void GenkiLedPwmStop(void) {
    IoTPwmStop(IOT_IO_NAME_2);
}