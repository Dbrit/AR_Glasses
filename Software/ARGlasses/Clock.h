// filename ******** Clock.h ************** 
// Lisong Sun, Kerry Tu, Dakota Britton, Casey Kim
// Created: October 26, 2021
// H file with LCD functions for each menu.
// Lab 11
// TA: Matthew Yu
// Last edited: November 25, 2021

#ifndef CLOCK_H
#define CLOCK_H

#include <stdlib.h>
#include <string.h>
#include "ST7735.h"
#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>
#include "../inc/CortexM.h"

#define PF2       (*((volatile uint32_t *)0x40025010))
#define PC6       (*((volatile uint32_t *)0x40006100)) //flashlight

extern volatile uint32_t hour;
extern volatile uint32_t minute;
extern volatile uint32_t second;
extern volatile uint32_t time;
extern volatile uint8_t directionalPress_flag;

extern enum MenuMode currentMenu;

enum MenuMode {
	TimeMode= 0,
	WeatherMode = 1,
	TempMode = 2,
	CompassMode = 3,
	LightMode = 4,
	BlankMode = 5,
};

// Initialize the main display
void Clock_Init(void);

void Clock_ShowTimeMenu(void);
void Clock_ShowWeatherMenu(void);
void Clock_ShowTempMenu(void);
void Clock_ShowCompassMenu(void);
void Clock_ShowBlankMenu(void);
void Clock_ShowLightMenu(void);

void Clock_UpdateTimeMenu(void);
void Clock_UpdateWeatherMenu(void);
void Clock_UpdateTempMenu(void);
void Clock_UpdateCompassMenu(void);
void Clock_UpdateBlankMenu(void);
void Clock_UpdateLightMenu(void);

#endif
