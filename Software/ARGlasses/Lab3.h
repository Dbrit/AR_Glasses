// filename ******** Lab3.h ************** 
// Lisong Sun and Kerry Tu
// Created: September 16, 2021
// Header file with main functions for alarm clock.
// Lab 4
// TA: Hassan Iqbal
// Last edited: September 20, 2021

// ST7735 Connections
// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected 
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO)
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

// Button Connections
// PC4 up, PC5 right, PC6 left, PC7 down
// PF0, PF4 are inputs. PF2 is heartbeat output

// Speaker Connections
// PB1 is speaker output into MOSFET

#ifndef LAB3_H
#define LAB3_H

//#include <stdio.h>
#include <stdint.h>
//#include "string.h"
#include "../inc/PLL.h"
#include "../inc/tm4c123gh6pm.h"
#include "../inc/LaunchPad.h"
#include "../inc/CortexM.h"
#include "Buttons.h"
#include "Speaker.h"
#include "Clock.h"

// main function for the alarm
int Lab3_main(void);
	
#endif
