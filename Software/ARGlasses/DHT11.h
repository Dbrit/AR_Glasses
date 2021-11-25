// filename ******** DHT11.h ************** 
// Lisong Sun, Kerry Tu, Dakota Britton, Casey Kim
// Created: October 26, 2021
// H file with functions to communicate with the DHT11 temperature sensor.
// Lab 11
// TA: Matthew Yu
// Last edited: November 25, 2021

#ifndef DHT11_H
#define DHT11_H

#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>
#include "../inc/CortexM.h"
#include "../inc/SysTick.h"
#include "ST7735.h"

#define PC4       (*((volatile uint32_t *)0x40006040))
	
#define DHT11_WAKEUP 18
#define DHT11_TIMEOUT 80000 // max timeout is 100us = 8000 clock cycles
#define DHT11_READ_TIMEOUT 0
#define DHT11_READ_WAIT 1
#define DHT11_READ_CHECKFAIL 2
#define DHT11_READ_SUCCESS 3

// init the DHT11 on PC4
void DHT11_Init(void);

// READS CAN ONLY BE CALLED ONCE EVERY 2 SECONDS
// read temperature and humidity into globals
void DHT11_Read(void);

// return temp
uint32_t DHT11_GetTemp(void);

// return humidity
uint32_t DHT11_GetHumidity(void);

#endif
