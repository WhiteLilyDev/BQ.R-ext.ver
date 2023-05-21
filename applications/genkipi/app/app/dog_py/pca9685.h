#ifndef PCA9685_H__
#define PCA9685_H__

#include <stdint.h>


void pca9685_servo_init(void);

void pca9685_servo_reset(void);

void pca9685_servo_set_angle(uint8_t channel, int angle);


#endif /* PCA9685_H__ */