#include "genki/nano/lcd.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "lvgl.h"

#define DEFAULT_LINE_WIDTH 2
#define DEFAULT_LINE_COLOR lv_palette_main(LV_PALETTE_BLUE)
#define DEFAULT_LINE_ROUNDED 0
#define DEFAULT_LINE_DASH_WIDTH 0
#define DEFAULT_LINE_DASH_GAP 0

#define DEFAULT_ARC_WIDTH 2
#define DEFAULT_ARC_COLOR lv_palette_main(LV_PALETTE_BLUE)
#define DEFAULT_ARC_ROUNDED 0
#define DEFAULT_ARC_FILLED 0

static lv_obj_t *root = NULL;


static void ui_tick(void) {
    while (root) {
        lv_obj_refresh_style(root, LV_PART_MAIN, LV_STYLE_PROP_ANY);
        lv_tick_inc(2);
        lv_task_handler();
        usleep(10);
    }
}

static void init(void) {
    if (root) return;

    lv_init();
    lv_port_disp_init();

    root = lv_scr_act();
    static lv_style_t s;
    lv_style_init(&s);
    // lv_obj_set_style_bg_color(src, lv_color_black(), 0);
    lv_style_set_bg_color(&s, lv_color_white());
    lv_obj_add_style(root, &s, 0);

    osThreadAttr_t attr;
    attr.name = "lvgl";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 4;
    attr.priority = 25;

    if (osThreadNew((osThreadFunc_t) ui_tick, NULL, &attr) == NULL) {
        printf("Create lvgl task Failed!\r\n");
    }

    usleep(1000 * 1000);
}


void NanoLCD_drawLine(const lcd_point_t points[], short point_num, lcd_line_style_t *style) {
    init();

    lv_point_t *line_points = (lv_point_t *) malloc(point_num * sizeof(lv_point_t));
    for (int i = 0; i < point_num; ++i) {
        lv_point_t *p = (lv_point_t *) malloc(sizeof(lv_point_t * ));
        p->x = points[i].x;
        p->y = points[i].y;
        line_points[i] = *p;
    }

    lv_obj_t *line;
    line = lv_line_create(root);
    lv_line_set_points(line, line_points, point_num);

    // color
    lcd_color_t *c = style->color;
    lv_color_t color;
    if (c) {
        color = lv_color_make(c->red, c->green, c->blue);
        free(c);
    } else {
        color = lv_color_make(0, 0, 0);
    }
    lv_obj_set_style_line_color(line, color, 0);

    // rounded
    lv_obj_set_style_line_rounded(line, style->rounded, 0);

    if(style->dash_width > 0) {
        // dash_width
        lv_obj_set_style_line_dash_width(line, style->dash_width, 0);
        // dash_gap
        lv_obj_set_style_line_dash_gap(line, style->dash_gap, 0);
    } else {
        // width
        lv_obj_set_style_line_width(line, style->width, 0);
    }

    free(points);
    free(style);
}

void
NanoLCD_drawArc(const lcd_point_t center, short radius, short start_angle, short end_angle, lcd_arc_style_t *style) {
    init();

    lv_obj_t *arc = lv_arc_create(root);
    lv_arc_set_mode(arc, LV_ARC_MODE_NORMAL);

    // angle, radius, pos
    lv_arc_set_bg_angles(arc, start_angle, end_angle);
    lv_obj_set_size(arc, radius * 2, radius * 2);
    lv_obj_set_pos(arc, center.x - radius, center.y - radius);

    // filled
    char filled = style->filled;

    // width
    int width = style->width;

    if (filled) {
        lv_obj_set_style_arc_width(arc, radius, 0);
    } else {
        lv_obj_set_style_arc_width(arc, width, 0);
    }

    // rounded
    char rounded = style->rounded;
    lv_obj_set_style_arc_rounded(arc, rounded, 0);

    // color
    lcd_color_t *c = style->color;
    lv_color_t color;
    if (c) {
        color = lv_color_make(c->red, c->green, c->blue);
        free(c);
    } else {
        color = lv_color_make(0, 0, 0);
    }
    lv_obj_set_style_arc_color(arc, color, 0);

    free(style);
}

void NanoLCD_drawText(const lcd_point_t pos, const char *text, lcd_text_style_t *style) {
    init();

    lv_obj_t *label = lv_label_create(root);
    lv_label_set_recolor(label, true);

    char buf[512] = {0};
    lcd_color_t *c = style->color;
    if(c) {
        sprintf(buf, "#%s%x%s%x%s%x %s#", c->red < 16 ? "0" : "", c->red,
                c->green < 16 ? "0" : "", c->green,
                c->blue < 16 ? "0" : "", c->blue, text);
    } else {
        sprintf(buf, "%s", text);
    }

    lv_label_set_text(label, buf);
    lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);

    lv_style_t *s = (lv_style_t *) malloc(sizeof(lv_style_t));
    lv_style_init(s);
    lv_obj_add_style(label, s, 0);

    char fontSize = style->fontSize;
    if (fontSize % 16 != 0) {
        fontSize += fontSize % 16;
    }

    if (fontSize < 16) {
        fontSize = 16;
    }
    if (fontSize > 32) {
        fontSize = 32;
    }

    int height = fontSize;
    int width = LV_HOR_RES_MAX - pos.x;

    lv_obj_set_size(label, width, height);
    lv_obj_set_pos(label, pos.x, pos.y);

    if (fontSize == 16) {
        const lv_font_t *font = &lv_font_montserrat_16;
        lv_style_set_text_font(s, font);
    } else if (fontSize == 32) {
        lv_font_t *font = &lv_font_montserrat_32;
        lv_style_set_text_font(s, font);
        free(font);
    }

    free(style);
}

void NanoLCD_setBgColor(lcd_color_t *c) {
    init();

    lv_color_t color = lv_color_make(c->red, c->green, c->blue);
    lv_obj_set_style_bg_color(root, color, 0);

    free(c);
}

void NanoLCD_drawRect(const lcd_rect_t r, lcd_rect_style_t *style) {
    init();

    lv_obj_t *rect = lv_obj_create(root);
    lv_obj_set_pos(rect, r.x, r.y);
    lv_obj_set_size(rect, r.width, r.height);

    lcd_color_t *c = style->color;
    lv_color_t color;
    if (c) {
        color = lv_color_make(c->red, c->green, c->blue);
        free(c);
    } else {
        color = lv_color_make(0, 0, 0);
    }

    char filled = style->filled;
    if (filled) {
        lv_obj_set_style_bg_color(rect, color, 0);
    } else {
        lv_obj_set_style_border_color(rect, color, 0);

        lv_obj_set_style_border_width(rect, style->borderWidth, 0);
    }

    lv_obj_set_style_radius(rect, style->rounded, 0);

    free(style);
}

void NanoLCD_clear(void) {
    if(root) {
        lv_obj_clean(root);
    }
}