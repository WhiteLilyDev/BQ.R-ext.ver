#include "lcd_gc9a01.h"

#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

#include "iot_gpio.h"
#include "iot_io.h"
#include "iot_errno.h"


#include "iot_spi.h"

void Gc9a01_LCD_GPIO_Init(void)
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
    LCD_RES_Clr();
    usleep(100*1000);
    LCD_RES_Set();

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
static void Gc9a01_LCD_SPI_Send(uint8_t *data, uint32_t size)
{
    IoTSpiHostWrite(0,(char *)data,size);
}
/******************************************************************************
      函数说明：LCD串行数据写入函数
      入口数据：dat  要写入的串行数据
      返回值：  无
******************************************************************************/
void Gc9a01_LCD_Writ_Bus(uint8_t dat) 
{	
	Gc9a01_LCD_SPI_Send(&dat, 1);
}


/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void Gc9a01_LCD_WR_DATA8(uint8_t dat)
{
	Gc9a01_LCD_Writ_Bus(dat);
}


/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void Gc9a01_LCD_WR_DATA(uint16_t dat)
{
	uint8_t data[2] = {0};

    data[0] = dat >> 8;

    data[1] = dat;

    LCD_DC_Set();
    Gc9a01_LCD_SPI_Send(data, 2);
}


/******************************************************************************
      函数说明：LCD写入命令
      入口数据：dat 写入的命令
      返回值：  无
******************************************************************************/
void Gc9a01_LCD_WR_REG(uint8_t dat)
{
	LCD_DC_Clr();//写命令
	Gc9a01_LCD_Writ_Bus(dat);
	LCD_DC_Set();//写数据
}


/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：x1,x2 设置列的起始和结束地址
                y1,y2 设置行的起始和结束地址
      返回值：  无
******************************************************************************/
void Gc9a01_LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
		Gc9a01_LCD_WR_REG(0x2a);//列地址设置
		Gc9a01_LCD_WR_DATA(x1);
		Gc9a01_LCD_WR_DATA(x2);
		Gc9a01_LCD_WR_REG(0x2b);//行地址设置
		Gc9a01_LCD_WR_DATA(y1);
		Gc9a01_LCD_WR_DATA(y2);
		Gc9a01_LCD_WR_REG(0x2c);//储存器写
}



void Gc9a01_LCD_Color_Fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,  uint16_t *buf)
{          
	uint16_t num;
    uint32_t size = 0;

    Gc9a01_LCD_Address_Set(x0, y0, x1, y1);

    size = ((x1 - x0) + 1) * ((y1 - y0) + 1)*2;

    LCD_DC_Set();
    Gc9a01_LCD_SPI_Send((uint8_t *)buf, size);		  	    
}

void Gc9a01_LCD_Init(void)
{
	Gc9a01_LCD_GPIO_Init();//初始化GPIO
	
	LCD_RES_Clr();//复位
	usleep(120*1000);
	LCD_RES_Set();
	usleep(120*1000);
	
	//LCD_BLK_Set();//打开背光
	//usleep(120*1000);
	
	Gc9a01_LCD_WR_REG(0xEF);
	Gc9a01_LCD_WR_REG(0xEB);
	Gc9a01_LCD_WR_DATA8(0x14); 
	
	Gc9a01_LCD_WR_REG(0xFE);			 
	Gc9a01_LCD_WR_REG(0xEF); 

	Gc9a01_LCD_WR_REG(0xEB);	
	Gc9a01_LCD_WR_DATA8(0x14); 

	Gc9a01_LCD_WR_REG(0x84);			
	Gc9a01_LCD_WR_DATA8(0x40); 

	Gc9a01_LCD_WR_REG(0x85);			
	Gc9a01_LCD_WR_DATA8(0xFF); 

	Gc9a01_LCD_WR_REG(0x86);			
	Gc9a01_LCD_WR_DATA8(0xFF); 

	Gc9a01_LCD_WR_REG(0x87);			
	Gc9a01_LCD_WR_DATA8(0xFF);

	Gc9a01_LCD_WR_REG(0x88);			
	Gc9a01_LCD_WR_DATA8(0x0A);

	Gc9a01_LCD_WR_REG(0x89);			
	Gc9a01_LCD_WR_DATA8(0x21); 

	Gc9a01_LCD_WR_REG(0x8A);			
	Gc9a01_LCD_WR_DATA8(0x00); 

	Gc9a01_LCD_WR_REG(0x8B);			
	Gc9a01_LCD_WR_DATA8(0x80); 

	Gc9a01_LCD_WR_REG(0x8C);			
	Gc9a01_LCD_WR_DATA8(0x01); 

	Gc9a01_LCD_WR_REG(0x8D);			
	Gc9a01_LCD_WR_DATA8(0x01); 

	Gc9a01_LCD_WR_REG(0x8E);			
	Gc9a01_LCD_WR_DATA8(0xFF); 

	Gc9a01_LCD_WR_REG(0x8F);			
	Gc9a01_LCD_WR_DATA8(0xFF); 


	Gc9a01_LCD_WR_REG(0xB6);
	Gc9a01_LCD_WR_DATA8(0x00);
	Gc9a01_LCD_WR_DATA8(0x20);

	Gc9a01_LCD_WR_REG(0x36);
	if(USE_HORIZONTAL==0)Gc9a01_LCD_WR_DATA8(0x08);
	else if(USE_HORIZONTAL==1)Gc9a01_LCD_WR_DATA8(0xC8);
	else if(USE_HORIZONTAL==2)Gc9a01_LCD_WR_DATA8(0x68);
	else Gc9a01_LCD_WR_DATA8(0xA8);

	Gc9a01_LCD_WR_REG(0x3A);			
	Gc9a01_LCD_WR_DATA8(0x05); 


	Gc9a01_LCD_WR_REG(0x90);			
	Gc9a01_LCD_WR_DATA8(0x08);
	Gc9a01_LCD_WR_DATA8(0x08);
	Gc9a01_LCD_WR_DATA8(0x08);
	Gc9a01_LCD_WR_DATA8(0x08); 

	Gc9a01_LCD_WR_REG(0xBD);			
	Gc9a01_LCD_WR_DATA8(0x06);
	
	Gc9a01_LCD_WR_REG(0xBC);			
	Gc9a01_LCD_WR_DATA8(0x00);	

	Gc9a01_LCD_WR_REG(0xFF);			
	Gc9a01_LCD_WR_DATA8(0x60);
	Gc9a01_LCD_WR_DATA8(0x01);
	Gc9a01_LCD_WR_DATA8(0x04);

	Gc9a01_LCD_WR_REG(0xC3);			
	Gc9a01_LCD_WR_DATA8(0x13);
	Gc9a01_LCD_WR_REG(0xC4);			
	Gc9a01_LCD_WR_DATA8(0x13);

	Gc9a01_LCD_WR_REG(0xC9);			
	Gc9a01_LCD_WR_DATA8(0x22);

	Gc9a01_LCD_WR_REG(0xBE);			
	Gc9a01_LCD_WR_DATA8(0x11); 

	Gc9a01_LCD_WR_REG(0xE1);			
	Gc9a01_LCD_WR_DATA8(0x10);
	Gc9a01_LCD_WR_DATA8(0x0E);

	Gc9a01_LCD_WR_REG(0xDF);			
	Gc9a01_LCD_WR_DATA8(0x21);
	Gc9a01_LCD_WR_DATA8(0x0c);
	Gc9a01_LCD_WR_DATA8(0x02);

	Gc9a01_LCD_WR_REG(0xF0);   
	Gc9a01_LCD_WR_DATA8(0x45);
	Gc9a01_LCD_WR_DATA8(0x09);
	Gc9a01_LCD_WR_DATA8(0x08);
	Gc9a01_LCD_WR_DATA8(0x08);
	Gc9a01_LCD_WR_DATA8(0x26);
 	Gc9a01_LCD_WR_DATA8(0x2A);

 	Gc9a01_LCD_WR_REG(0xF1);    
 	Gc9a01_LCD_WR_DATA8(0x43);
 	Gc9a01_LCD_WR_DATA8(0x70);
 	Gc9a01_LCD_WR_DATA8(0x72);
 	Gc9a01_LCD_WR_DATA8(0x36);
 	Gc9a01_LCD_WR_DATA8(0x37);  
 	Gc9a01_LCD_WR_DATA8(0x6F);


 	Gc9a01_LCD_WR_REG(0xF2);   
 	Gc9a01_LCD_WR_DATA8(0x45);
 	Gc9a01_LCD_WR_DATA8(0x09);
 	Gc9a01_LCD_WR_DATA8(0x08);
 	Gc9a01_LCD_WR_DATA8(0x08);
 	Gc9a01_LCD_WR_DATA8(0x26);
 	Gc9a01_LCD_WR_DATA8(0x2A);

 	Gc9a01_LCD_WR_REG(0xF3);   
 	Gc9a01_LCD_WR_DATA8(0x43);
 	Gc9a01_LCD_WR_DATA8(0x70);
 	Gc9a01_LCD_WR_DATA8(0x72);
 	Gc9a01_LCD_WR_DATA8(0x36);
 	Gc9a01_LCD_WR_DATA8(0x37); 
 	Gc9a01_LCD_WR_DATA8(0x6F);

	Gc9a01_LCD_WR_REG(0xED);	
	Gc9a01_LCD_WR_DATA8(0x1B); 
	Gc9a01_LCD_WR_DATA8(0x0B); 

	Gc9a01_LCD_WR_REG(0xAE);			
	Gc9a01_LCD_WR_DATA8(0x77);
	
	Gc9a01_LCD_WR_REG(0xCD);			
	Gc9a01_LCD_WR_DATA8(0x63);		


	Gc9a01_LCD_WR_REG(0x70);			
	Gc9a01_LCD_WR_DATA8(0x07);
	Gc9a01_LCD_WR_DATA8(0x07);
	Gc9a01_LCD_WR_DATA8(0x04);
	Gc9a01_LCD_WR_DATA8(0x0E); 
	Gc9a01_LCD_WR_DATA8(0x0F); 
	Gc9a01_LCD_WR_DATA8(0x09);
	Gc9a01_LCD_WR_DATA8(0x07);
	Gc9a01_LCD_WR_DATA8(0x08);
	Gc9a01_LCD_WR_DATA8(0x03);

	Gc9a01_LCD_WR_REG(0xE8);			
	Gc9a01_LCD_WR_DATA8(0x34);

	Gc9a01_LCD_WR_REG(0x62);			
	Gc9a01_LCD_WR_DATA8(0x18);
	Gc9a01_LCD_WR_DATA8(0x0D);
	Gc9a01_LCD_WR_DATA8(0x71);
	Gc9a01_LCD_WR_DATA8(0xED);
	Gc9a01_LCD_WR_DATA8(0x70); 
	Gc9a01_LCD_WR_DATA8(0x70);
	Gc9a01_LCD_WR_DATA8(0x18);
	Gc9a01_LCD_WR_DATA8(0x0F);
	Gc9a01_LCD_WR_DATA8(0x71);
	Gc9a01_LCD_WR_DATA8(0xEF);
	Gc9a01_LCD_WR_DATA8(0x70); 
	Gc9a01_LCD_WR_DATA8(0x70);

	Gc9a01_LCD_WR_REG(0x63);			
	Gc9a01_LCD_WR_DATA8(0x18);
	Gc9a01_LCD_WR_DATA8(0x11);
	Gc9a01_LCD_WR_DATA8(0x71);
	Gc9a01_LCD_WR_DATA8(0xF1);
	Gc9a01_LCD_WR_DATA8(0x70); 
	Gc9a01_LCD_WR_DATA8(0x70);
	Gc9a01_LCD_WR_DATA8(0x18);
	Gc9a01_LCD_WR_DATA8(0x13);
	Gc9a01_LCD_WR_DATA8(0x71);
	Gc9a01_LCD_WR_DATA8(0xF3);
	Gc9a01_LCD_WR_DATA8(0x70); 
	Gc9a01_LCD_WR_DATA8(0x70);

	Gc9a01_LCD_WR_REG(0x64);			
	Gc9a01_LCD_WR_DATA8(0x28);
	Gc9a01_LCD_WR_DATA8(0x29);
	Gc9a01_LCD_WR_DATA8(0xF1);
	Gc9a01_LCD_WR_DATA8(0x01);
	Gc9a01_LCD_WR_DATA8(0xF1);
	Gc9a01_LCD_WR_DATA8(0x00);
	Gc9a01_LCD_WR_DATA8(0x07);

	Gc9a01_LCD_WR_REG(0x66);			
	Gc9a01_LCD_WR_DATA8(0x3C);
	Gc9a01_LCD_WR_DATA8(0x00);
	Gc9a01_LCD_WR_DATA8(0xCD);
	Gc9a01_LCD_WR_DATA8(0x67);
	Gc9a01_LCD_WR_DATA8(0x45);
	Gc9a01_LCD_WR_DATA8(0x45);
	Gc9a01_LCD_WR_DATA8(0x10);
	Gc9a01_LCD_WR_DATA8(0x00);
	Gc9a01_LCD_WR_DATA8(0x00);
	Gc9a01_LCD_WR_DATA8(0x00);

	Gc9a01_LCD_WR_REG(0x67);			
	Gc9a01_LCD_WR_DATA8(0x00);
	Gc9a01_LCD_WR_DATA8(0x3C);
	Gc9a01_LCD_WR_DATA8(0x00);
	Gc9a01_LCD_WR_DATA8(0x00);
	Gc9a01_LCD_WR_DATA8(0x00);
	Gc9a01_LCD_WR_DATA8(0x01);
	Gc9a01_LCD_WR_DATA8(0x54);
	Gc9a01_LCD_WR_DATA8(0x10);
	Gc9a01_LCD_WR_DATA8(0x32);
	Gc9a01_LCD_WR_DATA8(0x98);

	Gc9a01_LCD_WR_REG(0x74);			
	Gc9a01_LCD_WR_DATA8(0x10);	
	Gc9a01_LCD_WR_DATA8(0x85);	
	Gc9a01_LCD_WR_DATA8(0x80);
	Gc9a01_LCD_WR_DATA8(0x00); 
	Gc9a01_LCD_WR_DATA8(0x00); 
	Gc9a01_LCD_WR_DATA8(0x4E);
	Gc9a01_LCD_WR_DATA8(0x00);					
	
    Gc9a01_LCD_WR_REG(0x98);			
	Gc9a01_LCD_WR_DATA8(0x3e);
	Gc9a01_LCD_WR_DATA8(0x07);

	Gc9a01_LCD_WR_REG(0x35);	
	Gc9a01_LCD_WR_REG(0x21);

	Gc9a01_LCD_WR_REG(0x11);
	usleep(120*1000);
	Gc9a01_LCD_WR_REG(0x29);
	usleep(20*1000);
} 

