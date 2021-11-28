// filename ******** ICM20948.c ************** 
// Lisong Sun, Kerry Tu, Dakota Britton, Casey Kim
// Created: October 26, 2021
// C file with functions to control the ICM20948 magnetometer.
// Lab 11
// TA: Matthew Yu
// Last edited: November 25, 2021

#include <stdint.h>
#include <stdio.h>
#include "../inc/ST7735.h"
#include "../inc/PLL.h"
#include "../inc/tm4c123gh6pm.h"
#include "ICM20948.h"
#include "../inc/SysTick.h"

uint8_t sendAfterWaiting(uint8_t code);
void ICM_Write(uint8_t reg, uint8_t data);
uint8_t ICM_Read(uint8_t reg);
void switchBank(uint8_t newBank);
void ICM20948_reset(void);
void ICM20948_sleep(uint8_t sleep);
void enableI2CMaster(void);
void enableMagDataRead(uint8_t reg, uint8_t bytes);
void resetMag(void);
void setMagOpMode(AK09916_opMode opMode);

#define PD4       (*((volatile uint32_t *)0x40007040))

int16_t magData[3] = {0}; // magnetometer data; x, y, z

static uint8_t currentBank;  // current register bank selected
static uint8_t regVal;   // intermediate storage of register values


// do all Init functions + debug with LCD
void ICM_Boot(void) {
	ICM_Init();
	//test ICM
	ST7735_SetCursor(7,3);
	ST7735_OutString("Init IMU...");
	while (readRegister8(0, ICM20948_WHO_AM_I) != 0xEA) {
		//ST7735_FillScreen(ST7735_BLACK);
		ST7735_SetCursor(7,3);
		ST7735_OutString("Init IMU...");
		SysTick80_Wait10ms(100);
		ICM_Init();
	}
	ST7735_OutString("S");
	
	ICM20948_magInit();
	//test mag
	ST7735_SetCursor(7,4);
	ST7735_OutString("Init Mag...");
	while (readAK09916Register16(AK09916_WIA_1) != AK09916_WHO_AM_I) {
		//ST7735_FillScreen(ST7735_BLACK);
		ST7735_SetCursor(7,4);
		ST7735_OutString("Init Mag...");
		SysTick80_Wait10ms(100);
		ICM20948_magInit();
	}
	ST7735_OutString("S");
	
	setMagOpMode(AK09916_CONT_MODE_100HZ); 
}

// initialize the CS pin (PD4)
void CS_Init(void) {
	SYSCTL_RCGCGPIO_R     |= 0x02;            // activate port B
  while((SYSCTL_PRGPIO_R & 0x02) == 0){};   // Wait
	GPIO_PORTB_PCTL_R     &= ~0x00F00000;     // GPIO
  GPIO_PORTB_DIR_R      |=  0x20;           // make PD4 out
  GPIO_PORTB_AFSEL_R    &= ~0x20;           // regular port function
  GPIO_PORTB_DEN_R      |=  0x20;           // enable digital I/O on PD4
  GPIO_PORTB_AMSEL_R    &= ~0x20;           // disable analog functionality on PD4
}

// initialize SSI1 on Port D
// DSS = 0xF, SPO = 1, SPH = 1
void SSI_Init(void) {
	SYSCTL_RCGCSSI_R |= 0x02;    // activate SSI1
	SYSCTL_RCGCGPIO_R |= 0x08;   // activate Port D
	while ((SYSCTL_PRGPIO_R&0x08) == 0) {}; // ready?
	SSI1_CR1_R = 0;              // disable SSI, master mode
	GPIO_PORTD_AFSEL_R |= 0x0F;  // enable alt func on PD3,2,1,0
	GPIO_PORTD_DEN_R |= 0x1F;    // enable dig I/O on PD4,3,2,1,0
  GPIO_PORTD_DIR_R |= 0x10;    // make PD4 output
	GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R & 0xFFFF0000) + 0x00002222; // set PD3,2,1,0 SSI functions
	GPIO_PORTD_AMSEL_R &= ~0x1F; // disable analog func on PD4,3,2,1,0
	
	
	SSI1_CPSR_R = 16;          // SSI Clk prescale; 80MHz/16 = 5MHz SSIClk (should work up to 7MHz)
	SSI1_CR0_R &= ~(0x0000FFF0); // SSI Control 0; SCR(serial clock rate, changes clock speed) = 0 (no more prescaling past CPSR) 
		                           // SPH (serial clock phase) = 1 (data captured on second clock edge)
	SSI1_CR0_R += 0xC0;          // SPO (serial clock polarity) = 1 (high when no data transfered)
	                             // FRF (frame format) = 0 (Freescale SPI frame format)
	SSI1_CR0_R |= 0x07;          // DSS (data size select) = 8-bit data
	SSI1_CR1_R |= 0x02;          // enable SSI1
}

// initialize the ICM
void ICM_Init(void) {
	// CS_Init();
	SSI_Init();
	SysTick_Init();							 // enable SysTick to utilize delays
	
	// being initializing the ICM
	ICM20948_reset();            // reset
	ICM20948_sleep(0);           // wake up
}

// initialize I2C for magnetometer
void ICM20948_magInit(void) {
	enableI2CMaster();
	resetMag();
//	ICM20948_reset();
//	ICM20948_sleep(0);
	writeRegister8(2, ICM20948_ODR_ALIGN_EN, 1); // aligns ODR
	enableI2CMaster();
}
// send 8-bit code, wait for reply, return reply
uint8_t sendAfterWaiting(uint8_t code){
	uint8_t dummy;
	while(SSI1_SR_R&SSI_SR_RNE){
		dummy = SSI1_DR_R;									// flush out leftover bytes in receiver
	}
	while((SSI1_SR_R&SSI_SR_TFE)==0){};   // wait until FIFO empty
	SSI1_DR_R = code;                     // data out
	while((SSI1_SR_R&SSI_SR_RNE)==0){};   // wait until response
	return SSI1_DR_R;                     // acknowledge response
}

// write to ICM instruction
void ICM_Write(uint8_t reg, uint8_t data) {
	PD4 = 0;
	sendAfterWaiting(0x7F&reg);
	sendAfterWaiting(data);
	PD4 = 0x10;
}

// read ICM instruction
uint8_t ICM_Read(uint8_t reg) {
	uint8_t data;
	PD4 = 0;
	sendAfterWaiting(0x80|reg);
	data = sendAfterWaiting(0);
	PD4 = 0x10;
	
	return data;
}

// switch register bank
void switchBank(uint8_t newBank){
    if(newBank != currentBank){
		  currentBank = newBank;
			ICM_Write(ICM20948_REG_BANK_SEL,currentBank<<4);
    }
}

// write to ICM register 8 bit
void writeRegister8(uint8_t bank, uint8_t reg, uint8_t val){
  switchBank(bank);
	ICM_Write(reg, val);
}

// read ICM register 8 bit
uint8_t readRegister8(uint8_t bank, uint8_t reg){
  switchBank(bank);
  uint8_t regValue = 0;
	regValue = ICM_Read(reg);
	return regValue;
}

// R/W ICM register 16 bit
void writeRegister16(uint8_t bank, uint8_t reg, uint16_t val) {
	switchBank(bank);
	int8_t MSByte = (int8_t)((val>>8) & 0xFF);
	uint8_t LSByte = val & 0xFF;
	PD4 = 0;
	sendAfterWaiting(0x7F&reg);
	sendAfterWaiting(MSByte);
	sendAfterWaiting(LSByte);
	PD4 = 0x10;
}

uint16_t readRegister16(uint8_t bank, uint8_t reg) {
	switchBank(bank);
  uint8_t MSByte = 0, LSByte = 0;
	int16_t reg16Val = 0;
	PD4 = 0;
	sendAfterWaiting(0x80|reg);
	MSByte = sendAfterWaiting(0);
	LSByte = sendAfterWaiting(0);
	PD4 = 0x10;
	reg16Val = (MSByte<<8) + LSByte;
	return reg16Val;
}

// reset internal registers and restore to default settings
void ICM20948_reset(void){
    writeRegister8(0, ICM20948_PWR_MGMT_1, ICM20948_RESET);
    SysTick80_Wait10ms(1);  // wait for registers to reset
}

// sets ICM to sleep mode. 1 = sleep, 0 = wake
void ICM20948_sleep(uint8_t sleep){
    regVal = readRegister8(0, ICM20948_PWR_MGMT_1);
    if(sleep){
        regVal |= ICM20948_SLEEP;
    }
    else{
        regVal &= ~ICM20948_SLEEP;
    }
    writeRegister8(0, ICM20948_PWR_MGMT_1, regVal);
}

void enableI2CMaster(void){
    writeRegister8(0, ICM20948_USER_CTRL, ICM20948_I2C_MST_EN); //enable I2C master
    writeRegister8(3, ICM20948_I2C_MST_CTRL, 0x07); // set I2C clock to 345.60 kHz, 46.67% duty cycle (max of magnetometer)
    SysTick80_Wait10ms(1);
}

// set I2CSLV0 to allow reads
void enableMagDataRead(uint8_t reg, uint8_t bytes) {
	writeRegister8(3, ICM20948_I2C_SLV0_ADDR, AK09916_ADDRESS | AK09916_READ); // read AK09916
	writeRegister8(3, ICM20948_I2C_SLV0_REG, reg); // define AK09916 register to be read
	writeRegister8(3, ICM20948_I2C_SLV0_CTRL, 0x80 | bytes); //enable read | number of bytes
	SysTick80_Wait10ms(1);
}

// R/W magnetometer
void writeAK09916Register8(uint8_t reg, uint8_t val) {
	writeRegister8(3, ICM20948_I2C_SLV0_ADDR, AK09916_ADDRESS); // write AK09916
	writeRegister8(3, ICM20948_I2C_SLV0_REG, reg); // define AK09916 register to be written to
	writeRegister8(3, ICM20948_I2C_SLV0_DO, val);
}

uint8_t readAK09916Register8(uint8_t reg) {
	enableMagDataRead(reg, 0x01);
	regVal = readRegister8(0, ICM20948_EXT_SLV_SENS_DATA_00);
	enableMagDataRead(AK09916_HXL, 0x08);
	return regVal;
}

void writeAK09916Register16(uint8_t reg, uint16_t val) { //unused
}

uint16_t readAK09916Register16(uint8_t reg) {
	int16_t regValue = 0;
	enableMagDataRead(reg, 0x02);
	regValue = readRegister16(0, ICM20948_EXT_SLV_SENS_DATA_00);
	enableMagDataRead(AK09916_HXL, 0x08);
	return regValue;
}

void resetMag(void) {
	writeAK09916Register8(AK09916_CNTL_3, 0x01);
	SysTick80_Wait10ms(10);
}

void setMagOpMode(AK09916_opMode opMode) {
  writeAK09916Register8(AK09916_CNTL_2, opMode);
	SysTick80_Wait10ms(1);
	if(opMode!=AK09916_PWR_DOWN){
			enableMagDataRead(AK09916_HXL, 0x08);
	}
}

void getMagnetometerData() {
	uint8_t HXL, HXH, HYL, HYH, HZL, HZH;
	// read raw values
	HXL = readRegister8(0,ICM20948_EXT_SLV_SENS_DATA_00);
	HXH = readRegister8(0,ICM20948_EXT_SLV_SENS_DATA_01);
	HYL = readRegister8(0,ICM20948_EXT_SLV_SENS_DATA_02);
	HYH = readRegister8(0,ICM20948_EXT_SLV_SENS_DATA_03);
	HZL = readRegister8(0,ICM20948_EXT_SLV_SENS_DATA_04);
	HZH = readRegister8(0,ICM20948_EXT_SLV_SENS_DATA_05);
	
	magData[0] = (int16_t)(HXH) << 8 | HXL;
	magData[1] = (int16_t)(HYH) << 8 | HYL;
	magData[2] = (int16_t)(HZH) << 8 | HZL;
}

uint32_t ICM20948_GetDirection() {
	double X, Y, Z, Xo, Yo, Zo, Xc, Yc;
	getMagnetometerData();
	X = (double) magData[0] * AK09916_MAG_LSB;
	Y = (double) magData[1] * AK09916_MAG_LSB;
	Xo = X - 15.149748;
	Yo = Y - 37.309036;
	Zo = Z - -17.643598;
	Xc = 0.967531 * Xo + -0.074893 * Yo + -0.038452 * Zo;
	Yc = -0.074893 * Xo + 0.965406 * Yo + -0.070214 * Zo;
	double angle = ((180*atan2(Yc, Xc))/3.14)+180;
	return (uint32_t)(angle+360) % 360;
}

