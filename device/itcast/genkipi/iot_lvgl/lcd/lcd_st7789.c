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


#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

#include "iot_gpio.h"
#include "iot_io.h"
#include "iot_errno.h"

#include "lcd_st7789.h"
//#include "font.h"
#include "iot_spi.h"


#define LCD_TOTAL_BUF_SIZE    (240*240*2)
#define LCD_Buf_Size 1152

static uint8_t lcd_buf[LCD_Buf_Size];

uint16_t    POINT_COLOR = BLACK;    // 画笔颜色    默认为黑色
uint16_t    BACK_COLOR     = WHITE;    // 背景颜色    默认为白色

/**
 *@brief        LCD控制引脚初始化
 *@param        none
 *@retval        none
*/
static void ST7789_LCD_GPIO_Init(void)
{
    int ret;

    IoTGpioInit(LCD_SDA_PIN);
    IoTIoSetFunc(LCD_SDA_PIN, LCD_SDA_FUNC);

    IoTGpioInit(LCD_SCL_PIN);
    IoTIoSetFunc(LCD_SCL_PIN, LCD_SCL_FUNC);
  
    IoTGpioInit(LCD_RST_Pin);
    IoTIoSetFunc(LCD_RST_Pin, LCD_RST_FUNC);
    IoTGpioSetDir(LCD_RST_Pin, IOT_GPIO_DIR_OUT);

    IoTGpioInit(LCD_DC_Pin);
    IoTIoSetFunc(LCD_DC_Pin, LCD_DC_FUNC);
    IoTGpioSetDir(LCD_DC_Pin, IOT_GPIO_DIR_OUT);

   
    
    /* 复位LCD */
    //LCD_PWR(0);
    LCD_RST(0);
    usleep(100*1000);
    LCD_RST(1);

    IoTSpiCfgInitParam oledInitParam;

    IoTSpiCfgBasicInfo oledBasicInfo;
    oledBasicInfo.cpol = IOT_SPI_CFG_CLOCK_CPOL_1;
    oledBasicInfo.cpha = IOT_SPI_CFG_CLOCK_CPHA_1;
    oledBasicInfo.framMode = IOT_SPI_CFG_FRAM_MODE_MOTOROLA;
    oledBasicInfo.dataWidth = IOT_SPI_CFG_DATA_WIDTH_E_8BIT;    
    oledBasicInfo.endian = IOT_SPI_CFG_ENDIAN_LITTLE;
    oledBasicInfo.pad =31;
    oledBasicInfo.freq= 40000000;
    
    ret=IoTSpiInit(0,oledInitParam, &oledBasicInfo);

}

/**
 * @brief    LCD底层SPI发送数据函数
 * @param   data —— 数据的起始地址
 * @param   size —— 发送数据字节数
 * @return  none
 */
static void ST7789_LCD_SPI_Send(uint8_t *data, uint32_t size)
{
    IoTSpiHostWrite(0,(char *)data,size);
}

/**
 * @brief    写命令到LCD
 *
 * @param   cmd        需要发送的命令
 *
 * @return  void
 */
static void ST7789_LCD_Write_Cmd(uint8_t cmd)
{
    LCD_DC(0);

    ST7789_LCD_SPI_Send(&cmd, 1);
}

/**
 * @brief    写数据到LCD
 *
 * @param   cmd        需要发送的数据
 *
 * @return  void
 */
static void ST7789_LCD_Write_Data(uint8_t data)
{
    LCD_DC(1);

    ST7789_LCD_SPI_Send(&data, 1);
}

/**
 * @brief    写半个字的数据到LCD
 *
 * @param   cmd        需要发送的数据
 *
 * @return  void
 */
void ST7789_LCD_Write_HalfWord(const uint16_t da)
{
    uint8_t data[2] = {0};

    data[0] = da >> 8;
    data[1] = da;

    LCD_DC(1);
    ST7789_LCD_SPI_Send(data, 2);
}


/**
 * 设置数据写入LCD缓存区域
 *
 * @param   x1,y1    起点坐标
 * @param   x2,y2    终点坐标
 *
 * @return  void
 */
void ST7789_LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    ST7789_LCD_Write_Cmd(0x2a);
    ST7789_LCD_Write_Data(x1 >> 8);
    ST7789_LCD_Write_Data(x1);
    ST7789_LCD_Write_Data(x2 >> 8);
    ST7789_LCD_Write_Data(x2);

    ST7789_LCD_Write_Cmd(0x2b);
    ST7789_LCD_Write_Data(y1 >> 8);
    ST7789_LCD_Write_Data(y1);
    ST7789_LCD_Write_Data(y2 >> 8);
    ST7789_LCD_Write_Data(y2);

    ST7789_LCD_Write_Cmd(0x2C);
}

/**
 * 打开LCD显示
 *
 * @param   void
 *
 * @return  void
 */
void ST7789_LCD_DisplayOn(void)
{
    //LCD_PWR(1);
}
/**
 * 关闭LCD显示
 *
 * @param   void
 *
 * @return  void
 */
void ST7789_LCD_DisplayOff(void)
{
    //LCD_PWR(0);
}

/**
 * 以一种颜色清空LCD屏
 *
 * @param   color    清屏颜色
 *
 * @return  void
 */
void ST7789_LCD_Clear(uint16_t color)
{
    uint16_t i, j;
    uint8_t data[2] = {0};

    data[0] = color >> 8;
    data[1] = color;

    ST7789_LCD_Address_Set(0, 0, LCD_Width - 1, LCD_Height - 1);

    for (j = 0; j < LCD_Buf_Size / 2; j++)
    {
        lcd_buf[j * 2] =  data[0];
        lcd_buf[j * 2 + 1] =  data[1];
    }

    LCD_DC(1);

    for (i = 0; i < (LCD_TOTAL_BUF_SIZE / LCD_Buf_Size); i++)
    {
        ST7789_LCD_SPI_Send(lcd_buf, LCD_Buf_Size);
    }
}

/**
 * 用一个颜色填充整个区域
 *
 * @param   x_start,y_start     起点坐标
 * @param   x_end,y_end            终点坐标
 * @param   color               填充颜色
 *
 * @return  void
 */
void ST7789_LCD_Fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color)
{
    uint16_t i = 0;
    uint32_t size = 0, size_remain = 0;

    size = (x_end - x_start + 1) * (y_end - y_start + 1) * 2;

    if(size > LCD_Buf_Size)
    {
        size_remain = size - LCD_Buf_Size;
        size = LCD_Buf_Size;
    }

    ST7789_LCD_Address_Set(x_start, y_start, x_end, y_end);

    while(1)
    {
        for (i = 0; i < size / 2; i++)
        {
            lcd_buf[2 * i] = color >> 8;
            lcd_buf[2 * i + 1] = color;
        }

        LCD_DC(1);
        ST7789_LCD_SPI_Send(lcd_buf, size);

        if (size_remain == 0) {
            break;
        }
            

        if (size_remain > LCD_Buf_Size)
        {
            size_remain = size_remain - LCD_Buf_Size;
        } else {
            size = size_remain;
            size_remain = 0;
        }
    }
}

/**
 * 画点函数
 *
 * @param   x,y        画点坐标
 *
 * @return  void
 */
void ST7789_LCD_Draw_Point(uint16_t x, uint16_t y)
{
    ST7789_LCD_Address_Set(x, y, x, y);
    ST7789_LCD_Write_HalfWord(POINT_COLOR);
}

/**
 * 画点带颜色函数
 *
 * @param   x,y        画点坐标
 *
 * @return  void
 */
void ST7789_LCD_Draw_ColorPoint(uint16_t x, uint16_t y, uint16_t color)
{
    ST7789_LCD_Address_Set(x, y, x, y);
    ST7789_LCD_Write_HalfWord(color);
}



/**
 * @brief    画线函数(直线、斜线)
 *
 * @param   x1,y1    起点坐标
 * @param   x2,y2    终点坐标
 *
 * @return  void
 */
void ST7789_LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;
    uint16_t i = 0;

    if (y1 == y2) {
        /* 快速画水平线 */
        ST7789_LCD_Address_Set(x1, y1, x2, y2);

        for (i = 0; i < (x2 - x1); i++) {
            lcd_buf[2 * i] = POINT_COLOR >> 8;
            lcd_buf[2 * i + 1] = POINT_COLOR;
        }

        LCD_DC(1);
        ST7789_LCD_SPI_Send(lcd_buf, (x2 - x1) * 2);
        return;
    }

    delta_x = x2 - x1;
    delta_y = y2 - y1;
    row = x1;
    col = y1;

    if (delta_x > 0) {
        incx = 1;
    } else if (delta_x == 0) {
        incx = 0;
    } else {
        incx = -1;
        delta_x = -delta_x;
    }

    if(delta_y > 0){
        incy = 1;
    } else if (delta_y == 0){
        incy = 0;
    } else {
        incy = -1;
        delta_y = -delta_y;
    }

    if (delta_x > delta_y){
        distance = delta_x;
    }
    else {
        distance = delta_y;
    }
    for (t = 0; t <= distance + 1; t++)
    {
        ST7789_LCD_Draw_Point(row, col);
        xerr += delta_x ;
        yerr += delta_y ;

        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}
/**
 * @brief    画一个矩形
 *
 * @param   x1,y1    起点坐标
 * @param   x2,y2    终点坐标
 *
 * @return  void
 */
void ST7789_LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    ST7789_LCD_DrawLine(x1, y1, x2, y1);
    ST7789_LCD_DrawLine(x1, y1, x1, y2);
    ST7789_LCD_DrawLine(x1, y2, x2, y2);
    ST7789_LCD_DrawLine(x2, y1, x2, y2);
}

/**
 * @brief    画一个圆
 *
 * @param   x0,y0    圆心坐标
 * @param   r       圆半径
 *
 * @return  void
 */
void ST7789_LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);

    while(a <= b)
    {
        ST7789_LCD_Draw_Point(x0 - b, y0 - a);
        ST7789_LCD_Draw_Point(x0 + b, y0 - a);
        ST7789_LCD_Draw_Point(x0 - a, y0 + b);
        ST7789_LCD_Draw_Point(x0 - b, y0 - a);
        ST7789_LCD_Draw_Point(x0 - a, y0 - b);
        ST7789_LCD_Draw_Point(x0 + b, y0 + a);
        ST7789_LCD_Draw_Point(x0 + a, y0 - b);
        ST7789_LCD_Draw_Point(x0 + a, y0 + b);
        ST7789_LCD_Draw_Point(x0 - b, y0 + a);
        a++;

        // Bresenham
        if (di < 0) {
            di += 4 * a + 6;
        }
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }

        ST7789_LCD_Draw_Point(x0 + a, y0 + b);
    }
}

// 把指定区域的显示缓冲区写入屏幕
void ST7789_LCD_Color_Fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,  uint16_t *buf)
{
    uint16_t num;
    uint32_t size = 0;

    ST7789_LCD_Address_Set(x0, y0, x1, y1);

    size = ((x1 - x0) + 1) * ((y1 - y0) + 1)*2;

    LCD_DC(1);
    ST7789_LCD_SPI_Send((uint8_t *)buf, size);
    
}



/**
 * @brief    LCD初始化
 * @param   none
 * @return  none
 */
void ST7789_LCD_Init(void)
{
    ST7789_LCD_GPIO_Init();

    usleep(120*1000);
    
    /* Sleep Out */
    ST7789_LCD_Write_Cmd(0x11);
    /* wait for power stability */
    usleep(120*1000);

    /* Memory Data Access Control */
    ST7789_LCD_Write_Cmd(0x36);
    ST7789_LCD_Write_Data(0x00);

    /* RGB 5-6-5-bit  */
    ST7789_LCD_Write_Cmd(0x3A);
    ST7789_LCD_Write_Data(0x65);

    /* Porch Setting */
    ST7789_LCD_Write_Cmd(0xB2);
    ST7789_LCD_Write_Data(0x0C);
    ST7789_LCD_Write_Data(0x0C);
    ST7789_LCD_Write_Data(0x00);
    ST7789_LCD_Write_Data(0x33);
    ST7789_LCD_Write_Data(0x33);


    /*  Gate Control */
    ST7789_LCD_Write_Cmd(0xB7);
    ST7789_LCD_Write_Data(0x72);

    /* VCOM Setting */
    ST7789_LCD_Write_Cmd(0xBB);
    ST7789_LCD_Write_Data(0x3D);   // Vcom=1.625V

    /* LCM Control */
    ST7789_LCD_Write_Cmd(0xC0);
    ST7789_LCD_Write_Data(0x2C);

    /* VDV and VRH Command Enable */
    ST7789_LCD_Write_Cmd(0xC2);
    ST7789_LCD_Write_Data(0x01);

    /* VRH Set */
    ST7789_LCD_Write_Cmd(0xC3);
    ST7789_LCD_Write_Data(0x19);

    /* VDV Set */
    ST7789_LCD_Write_Cmd(0xC4);
    ST7789_LCD_Write_Data(0x20);

    /* Frame Rate Control in Normal Mode */
    ST7789_LCD_Write_Cmd(0xC6);
    ST7789_LCD_Write_Data(0x01);    // 60MHZ

    /* Power Control 1 */
    ST7789_LCD_Write_Cmd(0xD0);
    ST7789_LCD_Write_Data(0xA4);
    ST7789_LCD_Write_Data(0xA1);

    /* Positive Voltage Gamma Control */
    ST7789_LCD_Write_Cmd(0xE0);
    ST7789_LCD_Write_Data(0xD0);
    ST7789_LCD_Write_Data(0x04);
    ST7789_LCD_Write_Data(0x0D);
    ST7789_LCD_Write_Data(0x11);
    ST7789_LCD_Write_Data(0x13);
    ST7789_LCD_Write_Data(0x2B);
    ST7789_LCD_Write_Data(0x3F);
    ST7789_LCD_Write_Data(0x54);
    ST7789_LCD_Write_Data(0x4C);
    ST7789_LCD_Write_Data(0x18);
    ST7789_LCD_Write_Data(0x0D);
    ST7789_LCD_Write_Data(0x0B);
    ST7789_LCD_Write_Data(0x1F);
    ST7789_LCD_Write_Data(0x23);

    /* Negative Voltage Gamma Control */
    ST7789_LCD_Write_Cmd(0xE1);
    ST7789_LCD_Write_Data(0xD0);
    ST7789_LCD_Write_Data(0x04);
    ST7789_LCD_Write_Data(0x0C);
    ST7789_LCD_Write_Data(0x11);
    ST7789_LCD_Write_Data(0x13);
    ST7789_LCD_Write_Data(0x2C);
    ST7789_LCD_Write_Data(0x3F);
    ST7789_LCD_Write_Data(0x44);
    ST7789_LCD_Write_Data(0x51);
    ST7789_LCD_Write_Data(0x2F);
    ST7789_LCD_Write_Data(0x1F);
    ST7789_LCD_Write_Data(0x1F);
    ST7789_LCD_Write_Data(0x20);
    ST7789_LCD_Write_Data(0x23);

    /* Display Inversion On */
    ST7789_LCD_Write_Cmd(0x21);

    ST7789_LCD_Write_Cmd(0x29);

    ST7789_LCD_Address_Set(0, 0, LCD_Width - 1, LCD_Height - 1);

    ST7789_LCD_Clear(WHITE);

}


