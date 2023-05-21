#ifndef __IMU_H
#define __IMU_H
#include  <math.h>

void IMUupdate(float gx, float gy, float gz, float ax, float ay, float az);
extern float Pitch,Roll,Yaw;


#endif




