
#ifndef GENKI_NANO_LCD_H
#define GENKI_NANO_LCD_H

#include <stdlib.h>


typedef struct {
    signed short int x;
    signed short int y;
} lcd_point_t;

typedef struct {
    signed short int x;
    signed short int y;
    unsigned short int width;
    unsigned short int height;
} lcd_rect_t;

typedef struct {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
} lcd_color_t;

typedef struct {
    signed short int width;
    signed short int dash_width;
    signed short int dash_gap;
    char rounded;
    lcd_color_t *color;
} lcd_line_style_t;

typedef struct {
    signed short int width;
    char rounded;
    char filled;
    lcd_color_t *color;
} lcd_arc_style_t;

typedef struct __lcd_text_style_t {
    char fontSize;
    lcd_color_t *color;
} lcd_text_style_t;

typedef struct {
    signed short int borderWidth;
    char filled;
    char rounded;
    lcd_color_t *color;
} lcd_rect_style_t;


void NanoLCD_drawLine(const lcd_point_t points[], short point_num, lcd_line_style_t *style);

void
NanoLCD_drawArc(const lcd_point_t center, short radius, short start_angle, short end_angle, lcd_arc_style_t *style);

void NanoLCD_drawText(const lcd_point_t pos, const char *text, lcd_text_style_t *style);

void NanoLCD_setBgColor(lcd_color_t *color);

void NanoLCD_drawRect(const lcd_rect_t rect, lcd_rect_style_t *style);

void NanoLCD_clear(void);

#endif //GENKI_NANO_LCD_H
