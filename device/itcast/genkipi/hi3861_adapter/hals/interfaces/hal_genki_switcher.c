//
// Created by poplar on 2022/2/23.
//
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"
#include "iot_io.h"
#include "genki_pin.h"
#include "genki/switcher.h"
#include <hi_time.h>

#define THE_CYCLES_OF_PWM_CONTROL 20000
#define FREQUENCY_PWM_CONTROL 50
static unsigned char is_init = 0;

static void init_gpio(){
    //初始化GPIO口
    IoTGpioInit(IOT_IO_NAME_13);
    //设置IO口功能为GPIO
    IoTIoSetFunc(IOT_IO_NAME_13, IOT_IO_FUNC_13_GPIO);
    //设置IO口输出方向：输出
    IoTGpioSetDir(IOT_IO_NAME_13, IOT_GPIO_DIR_OUT);


    //初始化GPIO口
    IoTGpioInit(IOT_IO_NAME_2);
    //设置IO口功能为GPIO
    IoTIoSetFunc(IOT_IO_NAME_2, IOT_IO_FUNC_2_GPIO);
    //设置IO口输出方向：输出
    IoTGpioSetDir(IOT_IO_NAME_2, IOT_GPIO_DIR_OUT);
}

static void init(void) {
    if (is_init) return;

    init_gpio();

    is_init = 1;
}

unsigned int GenkiSwitch_update_switcher(char* url){

    printf("work: %s %d\r\n", url, strlen(url));

    if (strcasecmp(url, "/switch/close") == 0) {
        printf("---------->close \r\n");
        switch_close();
    } else if (strcasecmp(url, "/switch/restore") == 0) {
        printf("---------->restore \r\n");
        switch_restore();
    } else if (strcasecmp(url, "/switch/open") == 0) {
        printf("---------->open \r\n");
        switch_open();
    }
};

unsigned int GenkiSwitch_switch_open() {
    GenkiSwitch_set_angle(55);
    hi_udelay(200 * 1000);
}
unsigned int GenkiSwitch_switch_close() {
    GenkiSwitch_set_angle(125);
    hi_udelay(200 * 1000);
}
unsigned int GenkiSwitch_switch_restore() {
    GenkiSwitch_set_angle(90);
    hi_udelay(200 * 1000);
}

unsigned int GenkiSwitch_pre_angle = -1;
/**
 * 控制舵机转动
 * @param utime
 */
unsigned int GenkiSwitch_set_angle(unsigned int angle_degrees) {
    init();

    IoTGpioSetOutputVal(IOT_IO_NAME_2, angle_degrees <= 90 ? IOT_GPIO_VALUE1 : IOT_GPIO_VALUE0);

//    unsigned int frequency = 50; // hz
//    unsigned int period = 1000 / frequency;  // 20ms
//    unsigned int sleep_utime = period * 1000; // 20000
    hi_u32 utime = (hi_u32) (500 + (angle_degrees * 1.0f * (2480 - 500) / 180.0f));

    int degrees_diff = 180;
    if (GenkiSwitch_pre_angle != -1){
        degrees_diff = abs((int)angle_degrees - (int)GenkiSwitch_pre_angle);
    }
    float move_time = 1.0f * degrees_diff / 180.0f;
    uint32_t fraq_count = FREQUENCY_PWM_CONTROL * move_time + 10;
    printf("steering start -------------angle: %d utime: %d fraq_count: %d\n", angle_degrees, utime, fraq_count);

    GenkiSwitch_pre_angle = angle_degrees;

    for (int i = 0; i < fraq_count; ++i) {
        IoTGpioSetOutputVal(IOT_IO_NAME_13, IOT_GPIO_VALUE1);
        //保持高电平或低电平若干毫秒使用hi_delay而不是usleep
        hi_udelay(utime);

        IoTGpioSetOutputVal(IOT_IO_NAME_13, IOT_GPIO_VALUE0);
        hi_udelay(THE_CYCLES_OF_PWM_CONTROL - utime);
    }

    printf("steering end ------------------------ %d\n", angle_degrees);

    return 1;
}