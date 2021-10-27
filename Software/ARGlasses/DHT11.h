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


#endif
