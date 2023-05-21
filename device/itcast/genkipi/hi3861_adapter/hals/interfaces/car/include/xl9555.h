
#ifndef GENKI_XL9555_H
#define GENKI_XL9555_H


typedef enum {
    XL9555_PIN_0,
    XL9555_PIN_1,
    XL9555_PIN_2,
    XL9555_PIN_3,
    XL9555_PIN_4,
    XL9555_PIN_5,
    XL9555_PIN_6,
    XL9555_PIN_7,
    XL9555_PIN_8,
    XL9555_PIN_9,
    XL9555_PIN_10,
    XL9555_PIN_11,
    XL9555_PIN_12,
    XL9555_PIN_13,
    XL9555_PIN_14,
    XL9555_PIN_15,
    XL9555_PIN_MAX
} XL9555PinName;

typedef enum {
    XL9555_OUTPUT,
    XL9555_INPUT
} XL9555Dir;

typedef enum {
    XL9555_VALUE_LOW,
    XL9555_VALUE_HIGH
} XL9555Value;

typedef enum {
    XL9555_SUCCESS,
    XL9555_OUT_OF_PIN,
    XL9555_READ_ERROR,
    XL9555_WRITE_ERROR
} XL9555ERRNO;


/**
 *
 * @return
 */
unsigned int XL9555_is_enable(void);

/**
 *
 * @param pin
 * @param dir
 * @return
 */
unsigned int XL9555_config_dir(XL9555PinName pin, XL9555Dir dir);

/**
 * config all pin dir, 0: output 1: input
 * @param dirs : short value for 16 pin,
 *          for example: 0b1111000000001111, 0: output 1: input
 *          first 8 value  11110000 for pin 8-15, from right to left
 *          last 8 value   00001111 for pin 0-7,  from right to left
 * @return XL9555ERRNO
 */
unsigned int XL9555_config_all_dir(unsigned short dirs);

unsigned int XL9555_set_output(XL9555PinName pin, XL9555Value val);

/**
 * set all pin output, 0: low 1: high
 * @param dirs : short value for 16 pin,
 *          for example: 0b1111000000001111, 0: output 1: input
 *          first 8 value  11110000 for pin 8-15, from right to left
 *          last 8 value   00001111 for pin 0-7,  from right to left
 * @return XL9555ERRNO
 */
unsigned int XL9555_set_all_output(unsigned short val);

unsigned int XL9555_get_output(XL9555PinName pin, XL9555Value *val);

/**
 * get all pin output, 0: low 1: high
 * @param dirs : short value for 16 pin,
 *          for example: 0b1111000000001111, 0: low 1: high
 *          first 8 value  11110000 for pin 8-15, from right to left
 *          last 8 value   00001111 for pin 0-7,  from right to left
 * @return XL9555ERRNO
 */
unsigned int XL9555_get_all_output(unsigned short *val);

unsigned int XL9555_get_input(XL9555PinName pin, XL9555Value *val);

/**
 * get all pin input, 0: low 1: high
 * @param dirs : short value for 16 pin,
 *          for example: 0b1111000000001111, 0: low 1: hight
 *          first 8 value  11110000 for pin 8-15, from right to left
 *          last 8 value   00001111 for pin 0-7,  from right to left
 * @return XL9555ERRNO
 */
unsigned int XL9555_get_all_input(unsigned short *val);


#endif //GENKI_XL9555_H
