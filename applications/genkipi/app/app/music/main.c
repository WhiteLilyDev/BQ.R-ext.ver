//
// Created by itcast on 22-7-7.
//
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "iot_gpio.h"
#include "iot_pwm.h"
#include "iot_io.h"
#include "genki_pin.h"
#include <math.h>

static const uint16_t g_tuneFreqs[] = {
        0,
        1046.5, // 1046.5
        1174.7, // 1174.7
        1318.5, // 1318.5
        1396.9, // 1396.9
        1568, // 1568
        1760, // 1760
        1975.5, // 1975.5
        783.99 // 5_ 783.99 // 第一个八度的 5
};

// 曲谱音符
static const uint8_t g_scoreNotes[] = {
        // 《两只老虎》简谱：http://www.jianpu.cn/pu/33/33945.htm
        1, 2, 3, 1,        1, 2, 3, 1,        3, 4, 5,  3, 4, 5,
        5, 6, 5, 4, 3, 1,  5, 6, 5, 4, 3, 1,  1, 8, 1,  1, 8, 1, // 最后两个 5 应该是低八度的
};


// 曲谱时值
static const uint8_t g_scoreDurations[] = {
        4, 4, 4, 4,        4, 4, 4, 4,        4, 4, 8,  4, 4, 8,
        3, 1, 3, 1, 4, 4,  3, 1, 3, 1, 4, 4,  4, 4, 8,  4, 4, 8,
};

static void music(void*arg) {
    //初始化GPIO口14
    IoTGpioInit(IOT_IO_NAME_14);
    //设置IO口功能为GPIO
    IoTIoSetFunc(IOT_IO_NAME_14, IOT_IO_FUNC_14_PWM5_OUT);
    //设置IO口输出方向：输出
    IoTGpioSetDir(IOT_IO_NAME_14, IOT_GPIO_DIR_OUT);
    //初始化PWM功能
    IoTPwmInit(IOT_PWM_NAME_5);

    while (1){
        for (size_t i = 0; i < sizeof(g_scoreNotes)/sizeof(g_scoreNotes[0]); i++) {
            uint32_t tune = g_scoreNotes[i]; // 音符
            uint16_t freqDivisor = g_tuneFreqs[tune];
            uint32_t tuneInterval = g_scoreDurations[i] * (125*1000); // 音符时间
            printf("%d %d %d %d\r\n", tune, (40*1000*1000) / freqDivisor, freqDivisor, tuneInterval);
            IoTPwmStart(IOT_PWM_NAME_5, 50, freqDivisor*4);
            usleep(tuneInterval);
            IoTPwmStop(IOT_PWM_NAME_5);
        }
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

    if (osThreadNew(music, NULL, &attr) == NULL) {
        printf("Falied to create music!\r\n");
    }

}

APP_FEATURE_INIT(start);