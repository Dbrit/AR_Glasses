// filename ******** ICM20948.h ************** 
// Lisong Sun, Kerry Tu, Dakota Britton, Casey Kim
// Created: October 26, 2021
// H file with functions to control the ICM20948 magnetometer.
// Lab 11
// TA: Matthew Yu
// Last edited: November 25, 2021

#ifndef ICM20948_H
#define ICM20948_H

// ICM connections
// CLK: PD0
// CS: PD4 (PB5)
// MISO:  PD2
// MOSI:  PD3
// FSYNC: PD1

#include <stdint.h>
#include <stdio.h>
#include "../inc/ST7735.h"
#include "../inc/PLL.h"
#include "../inc/tm4c123gh6pm.h"
#include "ICMRegisters.h"
#include <math.h>

// do all Init functions + debug with LCD
void ICM_Boot(void);

// initialize the CS pin (PB5)
void CS_Init(void);

// initialize SSI1 on Port D
// DSS = 0xF, SPO = 1, SPH = 1
void SSI_Init(void);

// initialize the ICM
void ICM_Init(void);

// read ICM register; specify bank and reg address; outputs data
uint8_t readRegister8(uint8_t bank, uint8_t reg);

// write ICM register; specify bank, reg address, and data
void writeRegister8(uint8_t bank, uint8_t reg, uint8_t val);

uint16_t readRegister16(uint8_t bank, uint8_t reg);
void writeRegister16(uint8_t bank, uint8_t reg, uint16_t val);

// R/W magnetometer via I2C
uint8_t readAK09916Register8(uint8_t reg);
void writeAK09916Register8(uint8_t reg, uint8_t val);
uint16_t readAK09916Register16(uint8_t reg);
void writeAK09916Register16(uint8_t reg, uint16_t val);

// initialize the magnetometer via I2C
void ICM20948_magInit(void);

// fill data buffer with the magnetometer data
void getMagnetometerData(void);

// return angle
uint32_t ICM20948_GetDirection(void);

#endif
