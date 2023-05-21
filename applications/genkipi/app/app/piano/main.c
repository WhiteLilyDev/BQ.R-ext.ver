//
// Created by itcast on 22-7-7.
//
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "genki_wifi_sta.h"
#include "lwip/sockets.h"

#include "iot_gpio.h"
#include "iot_pwm.h"
#include "iot_io.h"
#include "genki_pin.h"
#include <math.h>

#define WIFI_SSID "aoteman"
#define WIFI_PASSWORD "woshiaoteman"
#define HOSTNAME "mondraker"

//钢琴键对应的频率
static const uint16_t g_piano[] = {
        0,
        27.5,
        29.135,
        30.868,
        32.703,
        34.648,
        36.708,
        38.891,
        41.203,
        43.654,
        46.249,
        48.999,
        51.913,

        55.000,
        58.270,
        61.735,
        65.406,
        69.296,
        73.416,
        77.782,
        82.407,
        87.307,
        92.499,
        97.999,
        103.826,

        110.000,
        116.541,
        123.471,
        130.813,
        138.591,
        146.832,
        155.563,
        164.814,
        174.614,
        184.997,
        195.998,
        207.652,

        220.000,
        233.082,
        246.942,
        261.626,
        277.183,
        293.665,
        311.127,
        329.628,
        349.228,
        369.994,
        391.995,
        415.305,

        440.000,
        466.164,
        493.883,
        523.251,
        554.365,
        587.330,
        622.254,
        659.255,
        698.456,
        739.989,
        783.991,
        830.609,

        880.000,
        932.328,
        987.767,
        1046.502,
        1108.731,
        1174.659,
        1244.508,
        1318.510,
        1396.913,
        1479.978,
        1567.982,
        1661.219,

        1760.000,
        1864.655,
        1975.533,
        2093.005,
        2217.461,
        2349.318,
        2489.016,
        2637.020,
        2793.826,
        2959.955,
        3135.963,
        3322.438,

        3520.000,
        3729.310,
        3951.066,
        4186.009
};


static void play_task(uint8_t key) {
    printf("recv data %u ===========================================\n",key);
    uint16_t freqDivisor = g_piano[key];
    uint32_t tuneInterval = 4 * (125*1000); // 音符时间
    IoTPwmStart(IOT_PWM_NAME_5, 50, 4*freqDivisor);
    usleep(tuneInterval);
    IoTPwmStop(IOT_PWM_NAME_5);
}

static void music(void*arg) {
    //初始化GPIO口
    IoTGpioInit(IOT_IO_NAME_14);
    //设置IO口功能为GPIO
    IoTIoSetFunc(IOT_IO_NAME_14, IOT_IO_FUNC_14_PWM5_OUT);
    //设置IO口输出方向：输出
    IoTGpioSetDir(IOT_IO_NAME_14, IOT_GPIO_DIR_OUT);
    //初始化PWM功能
    IoTPwmInit(IOT_PWM_NAME_5);


    wifi_sta_connect(WIFI_SSID, WIFI_PASSWORD, HOSTNAME);

    // udp create
    int sock_fd;
    int ret;
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("sock_fd create error\r\n");
        return;
    }

    // config receive addr
    struct sockaddr_in recvfrom_addr;
    socklen_t recvfrom_addr_len = sizeof(recvfrom_addr);

    memset((void *) &recvfrom_addr, 0, recvfrom_addr_len);
    recvfrom_addr.sin_family = AF_INET;
    recvfrom_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    recvfrom_addr.sin_port = htons(8080);

    // bind receive addr
    // bind
    ret = bind(sock_fd, (struct sockaddr *) &recvfrom_addr, recvfrom_addr_len);
    if (ret == -1) {
        perror("bind error=================================\r\n");
        return;
    }
    printf("bind success=======================================");


    char recv_buf[4];
    int recv_len;
    while (1) {
        struct sockaddr_in sender_addr;
        int sender_addr_len;

        recv_len = recvfrom(sock_fd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *) &sender_addr,
                            sender_addr_len);

        if (recv_len <= 0) {
            continue;
        }
        printf("recv_len %d ================================\n",recv_len);
//        int recv_data;
//        memcpy(recv_data, recv_buf, recv_len);
//        printf("len: %d data: %c   %c  %c  %c\r\n", recv_len, recv_buf[0], recv_buf[1], recv_buf[2], recv_buf[3]);
        uint8_t key;
        memcpy(&key, &recv_buf[3], 1);

        printf("len: %d data: %u \r\n", recv_len,key);

        //开启新的线程
        osThreadAttr_t attr;

        attr.name = "play_thread";
        attr.attr_bits = 0U;
        attr.cb_mem = NULL;
        attr.cb_size = 0U;
        attr.stack_mem = NULL;
        attr.stack_size = 1024;
        attr.priority = 25;

        if (osThreadNew((osThreadFunc_t) play_task, key, &attr) == NULL) {
            printf("Create show task Failed!\r\n");
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