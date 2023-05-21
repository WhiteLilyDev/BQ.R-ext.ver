#ifndef   __MPU6050_H_
#define   __MPU6050_H_

typedef struct
{
    short   Temperature;     //温度
    short   Accel[3];
    short   Gyro[3];
} MPU6050_Data_TypeDef;

extern MPU6050_Data_TypeDef MPU6050_Data;
#define	SMPLRT_DIV		0x19	//陀螺仪采样率，典型值：0x07(125Hz)
/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
#define MPU6050_GYRO_OUT            0x43  //MPU6050陀螺仪数据寄存器地址
#define MPU6050_ACC_OUT             0x3B  //MPU6050加速度数据寄存器地址
#define MPU6050_SLAVE_ADDRESS       0x68  //MPU6050器件读地址
#define MPU6050_ADDRESS_AD0_LOW     0x68 // address pin low (GND), default for InvenSense evaluation board
#define MPU6050_RA_CONFIG           0x1A //低通滤波频率，典型值：0x06(5Hz)
#define MPU6050_RA_ACCEL_CONFIG     0x1C //加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
#define MPU6050_RA_GYRO_CONFIG      0x1B //陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s) 
#define MPU6050_RA_FF_THR           0x1D
#define MPU6050_RA_FF_DUR           0x1E
#define MPU6050_RA_MOT_THR          0x1F  //运动检测阀值设置寄存器
#define MPU6050_RA_MOT_DUR          0x20  //运动检测时间阀值
#define MPU6050_RA_ZRMOT_THR        0x21
#define MPU6050_RA_ZRMOT_DUR        0x22
#define MPU6050_RA_FIFO_EN          0x23
#define MPU6050_RA_INT_PIN_CFG      0x37   //中断/旁路设置寄存器
#define MPU6050_RA_INT_ENABLE       0x38   //中断使能寄存器
#define MPU6050_RA_TEMP_OUT_H       0x41
#define MPU6050_RA_USER_CTRL        0x6A
#define MPU6050_RA_PWR_MGMT_1       0x6B  //电源管理，典型值：0x00(正常启用)
#define MPU6050_RA_WHO_AM_I         0x75


void MPU6050_GPIO_Init(void);
void MPU6050_Read_Data(void);
#endif 
