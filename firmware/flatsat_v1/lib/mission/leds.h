#ifndef __LEDS_H
#define __LEDS_H
#include <Arduino.h>
void ledsConfigure(void);
void ledsTurnOff(void);
void ledsTurnOn(void);
void ledsBlink(int count, int time_off);
#endif