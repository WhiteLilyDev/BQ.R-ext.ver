#ifndef GENKIPI_SWITCHER_H
#define GENKIPI_SWITCHER_H


extern unsigned int GenkiSwitch_pre_angle;

unsigned int GenkiSwitch_update_switcher(char* url);

unsigned int GenkiSwitch_set_angle(unsigned int angle_degrees);

unsigned int GenkiSwitch_switch_open();
unsigned int GenkiSwitch_switch_close();
unsigned int GenkiSwitch_switch_restore();

#endif //GENKIPI_SWITCHER_H
