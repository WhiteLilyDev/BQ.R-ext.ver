/*
 * Copyright (c) 2020 Nanjing Xiaoxiongpai Intelligent Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _LCD_SPI2_DRV_H_
#define _LCD_SPI2_DRV_H_

#include <stdint.h>
#include "iot_gpio.h"
#include "pin.h"


#define LCD_PWR_Pin         14

#define LCD_DC_Pin          7
#define LCD_DC_FUNC        IOT_IO_FUNC_7_GPIO

#define LCD_RST_Pin         5
#define LCD_RST_FUNC        IOT_IO_FUNC_5_GPIO

#define LCD_SCL_PIN         6
#define LCD_SCL_FUNC        IOT_IO_FUNC_6_SPI0_CK

#define LCD_SDA_PIN         8
#define LCD_SDA_FUNC        IOT_IO_FUNC_8_SPI0_TXD

#define    LCD_PWR(n)        (n?IoTGpioSetOutputVal(LCD_PWR_Pin,1):IoTGpioSetOutputVal(LCD_PWR_Pin,0))
#define    LCD_DC(n)        (n?IoTGpioSetOutputVal(LCD_DC_Pin,1):IoTGpioSetOutputVal(LCD_DC_Pin,0))
#define    LCD_RST(n)        (n?IoTGpioSetOutputVal(LCD_RST_Pin,1):IoTGpioSetOutputVal(LCD_RST_Pin,0))

extern uint16_t    POINT_COLOR;        // Default brush color
extern uint16_t    BACK_COLOR;        // Default background color

#define LCD_Width        240
#define LCD_Height        240

// 画笔颜色
#define WHITE              0xFFFF
#define BLACK              0x0000
#define BLUE              0x001F
#define BRED             0XF81F
#define GRED              0XFFE0
#define GBLUE             0X07FF
#define RED                0xF800
#define MAGENTA            0xF81F
#define GREEN              0x07E0
#define CYAN               0x7FFF
#define YELLOW             0xFFE0 // 黄色
#define BROWN              0XBC40 // 棕色
#define BRRED              0XFC07 // 棕红色
#define GRAY               0X8430 // 灰色
#define DARKBLUE           0X01CF    // 深蓝色
#define LIGHTBLUE           0X7D7C    // 浅蓝色  
#define GRAYBLUE            0X5458 // 灰蓝色
#define LIGHTGREEN          0X841F // 浅绿色
#define LGRAY              0XC618 // 浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE        0XA651 // 浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 // 浅棕蓝色(选择条目的反色)


void ST7789_LCD_Init(void);                                                                    // Init
void ST7789_LCD_DisplayOn(void);                                                                // Open display
void ST7789_LCD_DisplayOff(void);                                                                // Close display
void ST7789_LCD_Write_HalfWord(const uint16_t da);                                                // Write half a byte of data to LCD
void ST7789_LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);                // Setting up the data display area
void ST7789_LCD_Clear(uint16_t color);                                                            // Clean screen
void ST7789_LCD_Fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color);                // Filled monochrome
void ST7789_LCD_Draw_Point(uint16_t x, uint16_t y);                                                        // Draw points
void ST7789_LCD_Draw_ColorPoint(uint16_t x, uint16_t y, uint16_t color);                                        // Painting with color dots
void ST7789_LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);                                        // Draw line
void ST7789_LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);                                    // Draw rectangle
void ST7789_LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r);                                                // Circle drawing
void ST7789_LCD_ShowChar(uint16_t x, uint16_t y, char chr, uint8_t size);                                        // Display a character
void ST7789_LCD_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size);                                    // Display a number
void ST7789_LCD_ShowxNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode);                            // Display number
void ST7789_LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p);                    // display string
void ST7789_LCD_Show_Image(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *p);                    // display picture
void ST7789_LCD_Color_Fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,  uint16_t *buf);
#endif /* _LCD_SPI2_DRV_H_ */
