
#ifndef GENKI_CAR_OLED_H
#define GENKI_CAR_OLED_H

unsigned char CarOLED_isEnable(void);

unsigned char CarOLED_clear(void);

unsigned char CarOLED_showRpm(double left, double right);

unsigned char CarOLED_showIP(unsigned int ip);

unsigned char CarOLED_showStr(unsigned char x, unsigned char y, unsigned char text[], unsigned char text_size);

#endif //GENKI_CAR_OLED_H
