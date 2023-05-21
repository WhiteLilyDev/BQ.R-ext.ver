
#ifndef GENKI_CAR_MOTOR_H
#define GENKI_CAR_MOTOR_H

unsigned char CarMotor_getRpm(double *left, double *right);

unsigned char CarMotor_pwmCtrl(int left, int right);

#endif //GENKI_CAR_MOTOR_H
