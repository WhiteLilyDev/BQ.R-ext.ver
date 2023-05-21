#ifndef __LCD_INIT_H
#define __LCD_INIT_H


#include <stdint.h>
#include "iot_gpio.h"

#include "pin.h"

#define USE_HORIZONTAL 0  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏


#define LCD_W 240
#define LCD_H 240


#define LCD_DC_Pin          7
#define LCD_DC_FUNC        IOT_IO_FUNC_7_GPIO

#define LCD_RST_Pin         5
#define LCD_RST_FUNC        IOT_IO_FUNC_5_GPIO

#define LCD_SCL_PIN         6
#define LCD_SCL_FUNC        IOT_IO_FUNC_6_SPI0_CK

#define LCD_SDA_PIN         8
#define LCD_SDA_FUNC        IOT_IO_FUNC_8_SPI0_TXD


//-----------------LCD端口定义---------------- 

#define LCD_RES_Clr()  IoTGpioSetOutputVal(LCD_RST_Pin,0)//RES
#define LCD_RES_Set()  IoTGpioSetOutputVal(LCD_RST_Pin,1)

#define LCD_DC_Clr()   IoTGpioSetOutputVal(LCD_DC_Pin,0)//DC
#define LCD_DC_Set()   IoTGpioSetOutputVal(LCD_DC_Pin,1)
 		     





void Gc9a01_LCD_GPIO_Init(void);//初始化GPIO
void Gc9a01_LCD_Writ_Bus(uint8_t dat);//模拟SPI时序
void Gc9a01_LCD_WR_DATA8(uint8_t dat);//写入一个字节
void Gc9a01_LCD_WR_DATA(uint16_t dat);//写入两个字节
void Gc9a01_LCD_WR_REG(uint8_t dat);//写入一个指令
void Gc9a01_LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);//设置坐标函数


void Gc9a01_LCD_Init(void);//LCD初始化
void Gc9a01_LCD_Color_Fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,  uint16_t *buf);


#endif




