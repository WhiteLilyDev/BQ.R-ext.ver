#ifndef GENKI_CAR_SSD1306_I2C_H
#define GENKI_CAR_SSD1306_I2C_H


void OLED_Init(void);
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_Fill(unsigned char fill_Data);
void OLED_Fill_Row(unsigned char fill_Data, unsigned char row);
void OLED_CLS(void);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize);
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N);
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);



#endif //GENKI_CAR_SSD1306_I2C_H
