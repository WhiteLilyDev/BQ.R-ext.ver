#include "genki/nano/buzzer.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "iot_gpio.h"
#include "iot_io.h"
#include "iot_pwm.h"
#include "genki_pin.h"


#define PIN     IOT_IO_NAME_14
#define FUNC    IOT_IO_FUNC_14_PWM5_OUT
#define PORT    IOT_PWM_NAME_5

static unsigned char is_init = 0;

static const double g_tuneLowFreqs[] = {
        0,
        523.25, // C
        587.33, // D
        659.26, // E
        698.46, // F
        783.99, // G
        880.00, // A
        987.77, // B
};

static const double g_tuneFreqs[] = {
        0,
        1046.5, // C
        1174.7, // D
        1318.5, // E
        1396.9, // F
        1568,   // G
        1760,   // A
        1975.5, // B
};

static void init(void) {
    if (is_init) return;

    //初始化GPIO口
    IoTGpioInit(PIN);
    //设置IO口功能为GPIO
    IoTIoSetFunc(PIN, FUNC);
    //设置IO口输出方向：输出
    IoTGpioSetDir(PIN, IOT_GPIO_DIR_OUT);
    //初始化PWM功能
    IoTPwmInit(PORT);

    is_init = 1;
}


void NanoBuzzer_play(int tune, unsigned char duration) {
    init();

    double freqDivisor;

    if (tune < 0) {
        freqDivisor = g_tuneLowFreqs[abs(tune)];
    } else {
        freqDivisor = g_tuneFreqs[tune];
    }

    if (tune != 0) {
        IoTPwmStart(PORT, 15, (unsigned int) (freqDivisor * 5));
    }
    unsigned int tuneInterval = duration * (125 * 1000); // 音符时间
    usleep(tuneInterval);
    IoTPwmStop(PORT);
}
