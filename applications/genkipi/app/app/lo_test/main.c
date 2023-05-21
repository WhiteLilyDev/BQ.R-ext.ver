//
// Created by itcast on 22-7-16.
//
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "hi_io.h"
#include "iot_gpio.h"
#include "iot_io.h"
#include "genki_pin.h"
#include "iot_spi.h"
#include "lcd.h"
#include "Weather.h"
#include "ssd1306_i2c.h"

#define IDX 0
#define I2C_Freq 400000

static void init_i2c(void) {
    IoTI2cInit(IDX, I2C_Freq);
    IoTIoSetFunc(IOT_IO_NAME_10, IOT_IO_FUNC_10_I2C0_SDA);
    IoTIoSetFunc(IOT_IO_NAME_9, IOT_IO_FUNC_9_I2C0_SCL);
}

static void init_spi(){
    //5:RES 6:SCL(时钟) 7:DC(片选) 8:SDA(数据)
    /*------------------ 初始化引脚 ------------------*/
    //时钟和数据线
    IoTIoSetFunc(IOT_IO_NAME_6, IOT_IO_FUNC_6_SPI0_CK);

    IoTIoSetFunc(IOT_IO_NAME_8, IOT_IO_FUNC_8_SPI0_TXD);
    IoTIoSetDriverStrength(IOT_IO_NAME_6, IOT_IO_DRIVER_STRENGTH_2);


    //RES
    IoTIoSetFunc(IOT_IO_NAME_5, IOT_IO_FUNC_5_GPIO);
    IoTGpioSetDir(IOT_IO_NAME_5, IOT_GPIO_DIR_OUT);


    //DC
    IoTIoSetFunc(IOT_IO_NAME_7, IOT_IO_FUNC_7_GPIO);
    IoTGpioSetDir(IOT_IO_NAME_7, IOT_GPIO_DIR_OUT);


    /*------------------ spi配置 ------------------*/
    IoTSpiCfgBasicInfo spi_cfg_basic_info;
    spi_cfg_basic_info.cpha = IOT_SPI_CFG_CLOCK_CPHA_1;
    spi_cfg_basic_info.cpol = IOT_SPI_CFG_CLOCK_CPOL_1;
    spi_cfg_basic_info.dataWidth = IOT_SPI_CFG_DATA_WIDTH_E_8BIT;
    spi_cfg_basic_info.endian = 0;
    spi_cfg_basic_info.framMode = IOT_SPI_CFG_FRAM_MODE_MOTOROLA;
    spi_cfg_basic_info.freq = 40000000; /* set frequency 2000000 */

    IoTSpiCfgInitParam spi_init_param = {0};
    spi_init_param.isSlave = HI_FALSE;
    uint32_t ret = IoTSpiInit(IOT_SPI_ID_0, spi_init_param, &spi_cfg_basic_info);
    if (ret != HI_ERR_SUCCESS) {
        printf("init fail!\n");
    }
}

static void Lcd_show(){
    LCD_Clear(BLACK);
    sleep(1);
    LCD_Clear(WHITE);
    //OLED_SetPos(0,0);
    sleep(1);
    LCD_Clear(BLACK);

    LCD_Clear(BLACK);//清屏
    LCD_ShowString(10,20,"lcd_task OK!",BLUE);
    sleep(5);
    LCD_Clear(BLACK);
/*
    LCD_ShowChar96(0+48*0,0,'0',0,WHITE);
    LCD_ShowChar96(0+48*1,0,'5',0,WHITE);
    LCD_ShowChar96(0+48*2,0,':',0,WHITE);
    LCD_ShowChar96(0+48*3,0,'2',0,WHITE);
    LCD_ShowChar96(0+48*4,0,'8',0,WHITE);

    LCD_ShowChar42(63+35+24*0,101,'2',0,WHITE);
    LCD_ShowChar42(63+35+24*1,101,'0',0,WHITE);
    LCD_ShowChar42(63+35+24*2,101,'2',0,WHITE);
    LCD_ShowChar42(63+35+24*3,101,'1',0,WHITE);

    LCD_ShowChar42(63+35+24*0,101+43,'0',0,WHITE);
    LCD_ShowChar42(63+35+24*1,101+43,'9',0,WHITE);
    LCD_ShowChar42(63+35+24*2,101+43,'/',0,WHITE);
    LCD_ShowChar42(63+35+24*3,101+43,'0',0,WHITE);
    LCD_ShowChar42(63+35+24*4,101+43,'8',0,WHITE);

    LCD_ShowChinese2(63+35+48*0,101+43+42,0,42,YELLOW);
    LCD_ShowChinese2(63+35+48*1,101+43+42,2,42,YELLOW);

    LCD_DrawLine(0,100,239,100,WHITE);
*/
    //显示下雨天动图
    //show_rain();
    //show_snow();
    while(1)
    {
        Draw_Circle(40,40,10,YELLOW);
        Draw_Circle(90,40,10,YELLOW);
        sleep(5);
        LCD_Clear(BLACK);
        LCD_DrawLine(30,40,50,40,YELLOW);
        LCD_DrawLine(80,40,100,40,YELLOW);
        sleep(0.5);
        LCD_Clear(BLACK);
    }
}
static void lcd_task(void *arg) {
    //初始化spi
    init_spi();
    //初始化lcd板
    Lcd_Init();
    //水平镜像翻转
    LCD_Rotation();
    //显示
    Lcd_show();
}

static void oled_show(void) {
    OLED_Fill(0x00);//clear
    sleep(1);
    OLED_Fill(0xff);//full white
    //OLED_SetPos(0,0);
    sleep(1);
    OLED_CLS();//clear

    OLED_ShowString(0, 0, "****************", 6*8);
    OLED_ShowString(0, 1, "** HarmonyOS! **", 6*8);
    OLED_ShowString(0, 2, "****************", 6*8);
    OLED_ShowString(0, 3, "****************", 6*8);
    sleep(5);
    OLED_CLS();//clear
    //OLED_DrawPoint(1,1,1);
    //OLED_ShowChar(0,0,'A',6*8);
    //OLED_ColorTurn(0);
    OLED_ShowString(0,2,"oled_task is OK!",6*8);
    sleep(5);
    OLED_CLS();//clear

    while(1)
    {
        OLED_ShowString(0,2,"awddddefabrt",6*8);
        sleep(2);
        OLED_CLS();//clear
        OLED_ShowString(0,2,"arbfnxndxnxn",6*8);
        sleep(2);
        OLED_CLS();
    }
}
static void oled_task(void *arg){
    //初始化i2c
    init_i2c();
    //初始化oled板
    OLED_Init();
    //显示
    oled_show();
}

static void start(void) {
    osThreadAttr_t attr;

    attr.name = "LCD_TASK";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 4;
    attr.priority = 25;

    if (osThreadNew((osThreadFunc_t) lcd_task, NULL, &attr) == NULL) {
        printf("Falied to create lcd_task!\r\n");
    }

    attr.name = "OLED_TASK";

    if (osThreadNew((osThreadFunc_t) oled_task, NULL, &attr) == NULL) {
        printf("Falied to create oled_task!\n\n");
    }
}

APP_FEATURE_INIT(start);