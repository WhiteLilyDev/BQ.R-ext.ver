#include "pca9685.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "hi_i2c.h"
#include "hi_errno.h"

#define PCA9685_ADDRESS    0x40
#define MODE1              0x00
#define MODE2              0x01
#define SUBADR1            0x02
#define SUBADR2            0x03
#define SUBADR3            0x04
#define PRESCALE           0xFE
#define LED0_ON_L          0x06
#define LED0_ON_H          0x07
#define LED0_OFF_L         0x08
#define LED0_OFF_H         0x09
#define ALL_LED_ON_L       0xFA
#define ALL_LED_ON_H       0xFB
#define ALL_LED_OFF_L      0xFC
#define ALL_LED_OFF_H      0xFD

#define RESTART            0x80
#define SLEEP              0x10
#define ALLCALL            0x01
#define INVRT              0x10
#define OUTDRV             0x04

#define PCA9685_I2C_IDX       HI_I2C_IDX_0
#define PCA9685_READ_ADDR     ((PCA9685_ADDRESS<<1)|0x1)
#define PCA9685_WRITE_ADDR    ((PCA9685_ADDRESS<<1)|0x0)


static void setAllPwm(uint16_t on, uint16_t off);

static void writeRaw8(uint8_t value) {
    value = value & 0xFF;

    uint8_t buffer[] = { value };

    hi_i2c_data data = { 0 };
    data.send_buf = buffer;
    data.send_len = sizeof(buffer);

    hi_i2c_write(PCA9685_I2C_IDX, PCA9685_WRITE_ADDR, &data);
}

static void write8(uint8_t reg, uint8_t value) {
    value = value & 0xFF;

    uint8_t buffer[] = { 
        reg, 
        value
    };

    hi_i2c_data data = { 0 };
    data.send_buf = buffer;
    data.send_len = sizeof(buffer);

    uint32_t retval = hi_i2c_write(PCA9685_I2C_IDX, PCA9685_WRITE_ADDR, &data);
    if (retval != HI_ERR_SUCCESS) {
        printf("hi_i2c_write(%02X) failed, %0X!\n", buffer[0], retval);
    }
}

static uint32_t readU8(uint8_t reg, uint8_t *out_value) {
    hi_i2c_data data = { 0 };

    uint8_t buffer[] = { reg };
    data.receive_buf = buffer;
    data.receive_len = sizeof(buffer);

    uint32_t retval = hi_i2c_read(PCA9685_I2C_IDX, PCA9685_READ_ADDR, &data);
    if (retval != HI_ERR_SUCCESS) {
        printf("hi_i2c_read() failed, %0X!\n", retval);
        return retval;
    }

    *out_value = data.receive_buf[0];

    return HI_ERR_SUCCESS;
}


void pca9685_servo_reset(void) {
    writeRaw8(0x06);
}

static void initPCA9685(void) {
    setAllPwm(0, 0);
    write8(MODE2, OUTDRV);
    write8(MODE1, ALLCALL);
    usleep(5 * 1000);
    uint8_t mode1;
    readU8(MODE1, &mode1);
    mode1 = mode1 & ~SLEEP;
    write8(MODE1, mode1);
    usleep(5 * 1000);
}

static void setPwmFreq(uint32_t freq) {
    float prescaleval = 25000000.0f; // 25MHz
    prescaleval /= 4096.0; // 12-bit
    prescaleval /= freq;
    prescaleval -= 1.0;

    uint8_t prescale = floor(prescaleval + 0.5);
    uint8_t oldmode;
    readU8(MODE1, &oldmode);
    uint8_t newmode = (oldmode & 0x7F) | SLEEP;
    write8(MODE1, newmode);
    write8(PRESCALE, prescale);
    write8(MODE1, oldmode);
    usleep(5 * 1000);
    write8(MODE1, oldmode | RESTART);
}

static void setPwm(uint8_t channel, uint16_t on, uint16_t off) {
    write8(LED0_ON_L + 4 * channel, on & 0xFF);
    write8(LED0_ON_H + 4 * channel, on >> 8);
    write8(LED0_OFF_L + 4 * channel, off & 0xFF);
    write8(LED0_OFF_H + 4 * channel, off >> 8);
}

static void setAllPwm(uint16_t on, uint16_t off) {
    write8(ALL_LED_ON_L, on & 0xFF);
    write8(ALL_LED_ON_H, on >> 8);
    write8(ALL_LED_OFF_L, off & 0xFF);
    write8(ALL_LED_OFF_H, off >> 8);
}

void pca9685_servo_init(void) {
    initPCA9685();
    setPwmFreq(50);
}

void pca9685_servo_set_angle(uint8_t channel, int angle) {
    uint16_t value = 4096 * ((angle * 11) + 500) / 20000;
    setPwm(channel, 0, value);
}