
#ifndef GENKI_CAR_CONFIG_H
#define GENKI_CAR_CONFIG_H

#include "ohos_init.h"
#include "cmsis_os2.h"


typedef struct {
    unsigned char pin_h1;
    unsigned char pin_h2;
    unsigned char pin_m1;
    unsigned char pin_m2;
    unsigned char port_m1;
    unsigned char port_m2;
    char motor_dir; // 1 or -1
} CarMotorCfg;


void CarConfig_init(void);

void CarConfig_getMotorCfg(CarMotorCfg *leftCfg, CarMotorCfg *rightCfg);

osMutexId_t CarConfig_getI2cMutexId(void);


#endif //GENKI_CAR_CONFIG_H
