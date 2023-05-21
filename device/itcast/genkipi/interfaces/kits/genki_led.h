
#ifndef GENKI_LED_H
#define GENKI_LED_H

#include <unistd.h>

void GenkiLedToggle(void);

void GenkiLedState(unsigned char *state);

void GenkiLedOpen(void);

void GenkiLedClose(void);

void GenkiLedPwmStart(unsigned short duty, unsigned int freq);

void GenkiLedPwmStop(void);

#endif //GENKI_LED_H
