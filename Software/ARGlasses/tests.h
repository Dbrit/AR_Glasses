// filename ******** tests.h ************** 
// Lisong Sun, Kerry Tu, Dakota Britton, Casey Kim
// Created: October 26, 2021
// H file with test/debugging functions for each module.
// Lab 11
// TA: Matthew Yu
// Last edited: November 25, 2021

#ifndef TESTS_H
#define TESTS_H

//#include <stdio.h>
#include <stdint.h>
//#include "string.h"
#include "../inc/PLL.h"
#include "../inc/tm4c123gh6pm.h"
#include "../inc/LaunchPad.h"
#include "../inc/CortexM.h"
#include "Buttons.h"
#include "APICaller.h"
#include "Clock.h"
#include "DHT11.h"
#include "ICM20948.h"
#include "../inc/Unified_Port_Init.h"

// test the 3D stereoscopic effect of various ST7735 functions
void Test_LCD(void);

// tests the ESP and all the used API calls
void Test_API(void);

// test BMP images
void Test_Image(void);

// test DHT11 readings
void Test_DHT11(void);

// test magnetometer
void Test_ICM20948(void);
#endif
