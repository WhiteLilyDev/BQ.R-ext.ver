#include "genki/car/motor.h"
#include "genki/car/config.h"

#include <stdlib.h>

#include "iot_gpio.h"
#include "iot_errno.h"
#include "iot_io.h"
#include "iot_pwm.h"
//#include "iot_watchdog.h"
//#include "hi_io.h"
//#include "hi_pwm.h"
//#include "hi_gpio.h"
//#include "hi_i2c.h"

#include <sys/time.h>


typedef struct {
    CarMotorCfg *cfg;
    int count;
    long start_time;
    char dir; // 1 or -1
    double rpm;
} MotorRpm;

typedef struct {
    CarMotorCfg *cfg;
    char dir; // 1 or -1;
    unsigned char pwm; // 0-99
} MotorDriver;

#define MIN_RPM_DURATION 50000
#define MAX_RPM_DURATION 1000000

//#define FREQ 64000
#define FREQ 10000

static unsigned char is_init = 0;
static unsigned char is_rpm_enable = 0;


static CarMotorCfg leftCfg;
static CarMotorCfg rightCfg;

static MotorRpm leftMR;
static MotorRpm rightMR;

static MotorDriver *leftMD;
static MotorDriver *rightMD;

static long get_us(void) {
    struct timeval time;
    gettimeofday(&time, NULL);

    return time.tv_sec * 1000000 + time.tv_usec;
}

static void check_wheel_stop(MotorRpm *mr) {
    long current_time = get_us();

    long duration = current_time - mr->start_time;
    if (duration > MAX_RPM_DURATION) {
        mr->rpm = 0;
    }
}

static void compute_rpm(MotorRpm *mr, unsigned char checkDuration) {
    long current_time = get_us();
    if (current_time <= mr->start_time) return;
    if (checkDuration) {
        if (current_time - mr->start_time < MIN_RPM_DURATION) return;
    }

    long duration = current_time - mr->start_time;

    if (duration > MAX_RPM_DURATION) {
        mr->rpm = 0;
    } else {
        // rpm: Revolutions Per Minute
        // mr->count / duration

        // mr->count North count, 1 round 6 North
        // 1:90
        // mr->count / 6 / 90

        // duration : us  ===> s ===> Minute
        // duration / 1000000 / 60

        // (mr->count / 6.0 / 90.0) / (duration / 1000000.0 / 60.0)
        mr->rpm = mr->cfg->motor_dir * mr->dir * mr->count * 1000000.0 / 9.0 / duration;
    }
    mr->start_time = current_time;
}

static void h1_rise_high(MotorRpm *mr) {
    // count
    mr->count++;
    // compute rpm
    compute_rpm(mr, 1);
}

static void h2_fall_low(MotorRpm *mr) {
    unsigned int val;
    unsigned int ret = IoTGpioGetInputVal(mr->cfg->pin_h1, &val);
    if (IOT_SUCCESS != ret) {
        printf("LEFT H1 read error\r\n");
        return;
    }

    if (val == IOT_GPIO_VALUE1) {
        // h2 fall , h1 high ===> motor run positive
        // last dir is negative, now positive ===> dir change
        if (mr->dir == -1) {
            compute_rpm(mr, 0);
            mr->count = 0;
        }
        // set positive
        mr->dir = 1;
    } else {
        // h2 fall , h1 low ===> motor run negative
        // last dir is negative, now is positive ===> dir change
        if (mr->dir == 1) {
            compute_rpm(mr, 0);
            mr->count = 0;
        }
        mr->dir = -1;
    }
}


static void left_wheel_h1_rise_high(char *arg) {
    (void) arg;
    h1_rise_high(&leftMR);
}

static void left_wheel_h2_fall_low(char *arg) {
    (void) arg;
    h2_fall_low(&leftMR);
}

static void right_wheel_h1_rise_high(char *arg) {
    (void) arg;
    h1_rise_high(&rightMR);
}

static void right_wheel_h2_fall_low(char *arg) {
    (void) arg;
    h2_fall_low(&rightMR);
}

static void rpm_init(void) {
    // left rpm
    // h1
    IoTGpioInit(leftMR.cfg->pin_h1);
    IoTGpioSetDir(leftMR.cfg->pin_h1, IOT_GPIO_DIR_IN);
    IoTIoSetFunc(leftMR.cfg->pin_h1, 0);
    IoTGpioRegisterIsrFunc(leftMR.cfg->pin_h1, IOT_INT_TYPE_EDGE, IOT_GPIO_EDGE_RISE_LEVEL_HIGH,
                           left_wheel_h1_rise_high,
                           NULL);

    // h2
    IoTGpioInit(leftMR.cfg->pin_h2);
    IoTGpioSetDir(leftMR.cfg->pin_h2, IOT_GPIO_DIR_IN);
    IoTIoSetFunc(leftMR.cfg->pin_h2, 0);
    IoTGpioRegisterIsrFunc(leftMR.cfg->pin_h2, IOT_INT_TYPE_EDGE, IOT_GPIO_EDGE_FALL_LEVEL_LOW, left_wheel_h2_fall_low,
                           NULL);

    // right rpm
    // h1
    IoTGpioInit(rightMR.cfg->pin_h1);
    IoTGpioSetDir(rightMR.cfg->pin_h1, IOT_GPIO_DIR_IN);
    IoTIoSetFunc(rightMR.cfg->pin_h1, 0);
    IoTGpioRegisterIsrFunc(rightMR.cfg->pin_h1, IOT_INT_TYPE_EDGE, IOT_GPIO_EDGE_RISE_LEVEL_HIGH,
                           right_wheel_h1_rise_high,
                           NULL);

    // h2
    IoTGpioInit(rightMR.cfg->pin_h2);
    IoTGpioSetDir(rightMR.cfg->pin_h2, IOT_GPIO_DIR_IN);
    IoTIoSetFunc(rightMR.cfg->pin_h2, 0);
    IoTGpioRegisterIsrFunc(rightMR.cfg->pin_h2, IOT_INT_TYPE_EDGE, IOT_GPIO_EDGE_FALL_LEVEL_LOW,
                           right_wheel_h2_fall_low,
                           NULL);
}

static void motor_stop(MotorDriver *md) {
    IoTPwmStop(md->cfg->port_m1);
    IoTPwmDeinit(md->cfg->port_m1);

    IoTPwmStop(md->cfg->port_m2);
    IoTPwmDeinit(md->cfg->port_m2);

    IoTGpioSetOutputVal(md->cfg->pin_m1, IOT_GPIO_VALUE0); // low
    IoTGpioSetOutputVal(md->cfg->pin_m2, IOT_GPIO_VALUE0); // low
}

static void motor_forward(MotorDriver *md, unsigned int pwm) {
    // stop m1
    IoTPwmStop(md->cfg->port_m1);
    IoTPwmDeinit(md->cfg->port_m1);

    IoTGpioSetOutputVal(md->cfg->pin_m1, IOT_GPIO_VALUE1); // high

    // start m2
    IoTPwmInit(md->cfg->port_m2);
    IoTPwmStart(md->cfg->port_m2, pwm, FREQ);
}

static void motor_backend(MotorDriver *md, unsigned int pwm) {
    // stop m2
    IoTPwmStop(md->cfg->port_m2);
    IoTPwmDeinit(md->cfg->port_m2);

    IoTGpioSetOutputVal(md->cfg->pin_m2, IOT_GPIO_VALUE1); // high

    // start m1
    IoTPwmInit(md->cfg->port_m1);
    IoTPwmStart(md->cfg->port_m1, pwm, FREQ);
}


static void pwm_init(unsigned char pin, unsigned pwm_port) {
    // 初始化GPIO端口
    IoTGpioInit(pin);
    // 设置IO口功能为GPIO
    IoTIoSetFunc(pin, 5);
    // 设置IO口输出方式：输出
    IoTGpioSetDir(pin, IOT_GPIO_DIR_OUT);

    IoTGpioSetOutputVal(pin, IOT_GPIO_VALUE0);

//    // 初始化PWM
//    IoTPwmInit(pwm_port);

    printf("pin: %d  pwm_port: %d\r\n", pin, pwm_port);
}

static void motor_init(void) {
    // left motor
    // m1
    pwm_init(leftMD->cfg->pin_m1, leftMD->cfg->port_m1);
    // m2
    pwm_init(leftMD->cfg->pin_m2, leftMD->cfg->port_m2);

    // right motor
    // m1
    pwm_init(rightMD->cfg->pin_m1, rightMD->cfg->port_m1);
    // m2
    pwm_init(rightMD->cfg->pin_m2, rightMD->cfg->port_m2);
}

static void init(void) {
    if (is_init) return;

    CarConfig_getMotorCfg(&leftCfg, &rightCfg);

    // init left motor rpm
    leftMR.cfg = &leftCfg;
    leftMR.count = 0;
    leftMR.start_time = 0;
    leftMR.rpm = 0;
    leftMR.dir = 1;

    rightMR.cfg = &rightCfg;
    rightMR.count = 0;
    rightMR.start_time = 0;
    rightMR.rpm = 0;
    rightMR.dir = 1;


    leftMD = malloc(sizeof(MotorDriver));
    leftMD->cfg = &leftCfg;

    rightMD = malloc(sizeof(MotorDriver));
    rightMD->cfg = &rightCfg;

    motor_init();
//    rpm_init();

    is_init = 1;
}

unsigned char CarMotor_getRpm(double *left, double *right) {
    init();

    check_wheel_stop(&leftMR);
    *left = leftMR.rpm;

    check_wheel_stop(&rightMR);
    *right = rightMR.rpm;

    return 1;
}


static void pwm_ctrl_left_motor(int pwm) {
    if (pwm == 0) {
        motor_stop(leftMD);
        return;
    }

    if (pwm > 0) {
        motor_backend(leftMD, pwm);
    } else {
        motor_forward(leftMD, -pwm);
    }
}

static void pwm_ctrl_right_motor(int pwm) {
    if (pwm == 0) {
        motor_stop(rightMD);
        return;
    }

    if (pwm > 0) {
        motor_forward(rightMD, pwm);
    } else {
        motor_backend(rightMD, -pwm);
    }
}

unsigned char CarMotor_pwmCtrl(int left, int right) {
    init();

//    printf("%d %d\r\n", left, right);

    if (left < -99) {
        left = -99;
    }
    if (left > 99) {
        left = 99;
    }

    if (right < -99) {
        right = -99;
    }
    if (right > 99) {
        right = 99;
    }

    pwm_ctrl_left_motor(left);
    pwm_ctrl_right_motor(right);
}