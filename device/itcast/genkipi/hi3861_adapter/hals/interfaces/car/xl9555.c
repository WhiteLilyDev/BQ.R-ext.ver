#include <stdio.h>

#include "iot_errno.h"
#include "iot_gpio.h"
#include "iot_i2c.h"
#include "iot_io.h"

#include "xl9555.h"

#define IDX 0
#define ADDRESS_READ 0x41
#define ADDRESS_WRITE 0x40

#define INPUT_PORT0 0x00
#define INPUT_PORT1 0x01
#define OUTPUT_PORT0 0x02
#define OUTPUT_PORT1 0x03
#define POLARITY_PORT0 0x04
#define POLARITY_PORT1 0x05
#define CONFIG_PORT0 0x06
#define CONFIG_PORT1 0x07


static unsigned int XL9555_read8(unsigned char reg, unsigned char *val) {
    unsigned char send_data[1] = {reg};

    unsigned int ret = IoTI2cWrite(IDX, ADDRESS_READ, send_data, 1);
    if (ret != IOT_SUCCESS) {
        return ret;
    }

    unsigned char data[1] = {0};
    unsigned int dataLen = 1;
    ret = IoTI2cRead(IDX, ADDRESS_READ, data, dataLen);
    if (ret != IOT_SUCCESS) {
        return ret;
    }
    *val = data[0];

    return IOT_SUCCESS;
}

static unsigned int XL9555_write8(unsigned char reg, unsigned char val) {
    unsigned char send_data[2] = {reg, val};

    unsigned int ret = IoTI2cWrite(IDX, ADDRESS_WRITE, send_data, 2);

    if (ret != IOT_SUCCESS) {
        printf("[XL9555]Write8 error: %x\r\n", ret);
        return ret;
    }
    return IOT_SUCCESS;
}

static unsigned int XL9555_digital_write(unsigned char port, unsigned char pin, unsigned char state) {
    unsigned char val;
    if (XL9555_read8(port, &val) == 0) {
        unsigned char dst;
        if (((val >> pin) & 0x01) != state) {
            unsigned char flag;
            if (state == 0) {
                flag = ((unsigned char) 0x01) << pin;
                flag ^= ((unsigned char) 0xff);

                dst = val & flag;
            } else {
                flag = ((unsigned char) 0x01) << pin;

                dst = val | flag;
            }
        } else {
            dst = val;
        }

        if (XL9555_write8(port, dst) == 0) {
            return XL9555_SUCCESS;
        }
        return XL9555_WRITE_ERROR;
    }
    return XL9555_READ_ERROR;
}

static unsigned int XL9555_digital_read(unsigned char port, unsigned char pin, unsigned char *state) {
    unsigned char val;
    if (XL9555_read8(port, &val) == 0) {
        *state = (val >> pin) & 0x01;
        return XL9555_SUCCESS;
    }
    return XL9555_READ_ERROR;
}

unsigned int XL9555_is_enable(void) {
    unsigned char val;
    if (XL9555_read8(INPUT_PORT0, &val) == 0) {
        return XL9555_SUCCESS;
    }
    return XL9555_READ_ERROR;
}

unsigned int XL9555_config_dir(XL9555PinName pin, XL9555Dir dir) {
    if (pin >= XL9555_PIN_MAX) {
        return XL9555_OUT_OF_PIN;
    }

    unsigned char port = CONFIG_PORT0;
    unsigned index = pin;
    if (pin > 7) {
        port = CONFIG_PORT1;
        index = pin - 8;
    }
    return XL9555_digital_write(port, index, dir);
}

unsigned int XL9555_config_all_dir(unsigned short dirs) {
    unsigned char val0 = dirs & 0xff;
    unsigned char val1 = (dirs >> 8) & 0xff;

    if (XL9555_write8(CONFIG_PORT0, val0) != 0) {
        printf("config port 0 write error\r\n");
        return XL9555_WRITE_ERROR;
    }
    if (XL9555_write8(CONFIG_PORT1, val1) != 0) {
        printf("config port 1 write error\r\n");
        return XL9555_WRITE_ERROR;
    }
    return XL9555_SUCCESS;
}

unsigned int XL9555_set_output(XL9555PinName pin, XL9555Value val) {
    if (pin >= XL9555_PIN_MAX) {
        return XL9555_OUT_OF_PIN;
    }

    unsigned char port = OUTPUT_PORT0;
    unsigned index = pin;
    if (pin > 7) {
        port = OUTPUT_PORT1;
        index = pin - 8;
    }
    return XL9555_digital_write(port, index, val);
}

unsigned int XL9555_set_all_output(unsigned short val) {
    unsigned char val0 = val & 0xff;
    unsigned char val1 = (val >> 8) & 0xff;

    if (XL9555_write8(OUTPUT_PORT0, val0) == 0 && XL9555_write8(OUTPUT_PORT1, val1) == 0) {
        return XL9555_SUCCESS;
    }
    return XL9555_WRITE_ERROR;
}

unsigned int XL9555_get_output(XL9555PinName pin, XL9555Value *val) {
    if (pin >= XL9555_PIN_MAX) {
        return XL9555_OUT_OF_PIN;
    }

    unsigned char port = OUTPUT_PORT0;
    unsigned index = pin;
    if (pin > 7) {
        port = OUTPUT_PORT1;
        index = pin - 8;
    }

    return XL9555_digital_read(port, index, val);
}

unsigned int XL9555_get_all_output(unsigned short *val) {
    unsigned char val0;
    unsigned char val1;

    if (XL9555_read8(OUTPUT_PORT0, val0) == 0 && XL9555_read8(OUTPUT_PORT1, val1) == 0) {
        *val = (val1 << 8) | val0;
        return XL9555_SUCCESS;
    }
    return XL9555_WRITE_ERROR;
}

unsigned int XL9555_get_input(XL9555PinName pin, XL9555Value *val) {
    if (pin >= XL9555_PIN_MAX) {
        return XL9555_OUT_OF_PIN;
    }

    unsigned char port = INPUT_PORT0;
    unsigned index = pin;
    if (pin > 7) {
        port = INPUT_PORT1;
        index = pin - 8;
    }

    return XL9555_digital_read(port, index, val);
}

unsigned int XL9555_get_all_input(unsigned short *val) {
    unsigned char val0;
    unsigned char val1;

    if (XL9555_read8(INPUT_PORT0, &val0) == 0 && XL9555_read8(INPUT_PORT1, &val1) == 0) {
        *val = (val1 << 8) | val0;
        return XL9555_SUCCESS;
    }
    return XL9555_WRITE_ERROR;
}