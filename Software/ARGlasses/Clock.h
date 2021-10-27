// filename ******** Clock.h ************** 
// Lisong Sun and Kerry Tu
// Created: September 16, 2021
// Header file with LCD functions used for the clock display and menus
// Lab 4
// TA: Hassan Iqbal
// Last edited: September 20, 2021

#ifndef CLOCK_H
#define CLOCK_H

#include "ST7735.h"
#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>
#include "../inc/CortexM.h"

#define PF2       (*((volatile uint32_t *)0x40025010))

extern volatile uint32_t hour;
extern volatile uint32_t minute;
extern volatile uint32_t second;
extern volatile uint32_t time;
extern volatile uint8_t directionalPress_flag;
extern volatile int32_t timeout;
extern volatile uint8_t timeout_flag;

extern enum MenuMode currentMenu;

enum MenuMode {
	Clock = 0,
	TimeConfig = 1,
	AlarmConfig = 2,
	AlarmActivate = 3
};

// Initialize the main display
void Clock_Init(void);

// Update the clock
void Clock_UpdateClock(void);

// Display the clock menu
void Clock_ShowClock(void);

// Update the time config menu
void Clock_UpdateMenu1(void);

// Display the time config menu
void Clock_ShowMenu1(void);

// Update the alarm config menu
void Clock_UpdateMenu2(void);

// Display the alarm config menu
void Clock_ShowMenu2(void);

// Display the alarm activation menu
void Clock_ShowMenu3(void);

// pause clock ticking
void Clock_Pause(void);

// continue clock ticking
void Clock_Continue(void);

// Shutoff alarm
void Clock_AlarmShutoff(void);

// Snooze alarm
void Clock_AlarmSnooze(void);

// Initialize the clock timer to 1Hz
void Timer1_Init(void);

// Tick up the current time
void Timer1A_Handler(void);

#endif
