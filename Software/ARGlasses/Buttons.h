// filename ******** Buttons.h ************** 
// Lisong Sun, Kerry Tu, Dakota Britton, Casey Kim
// Created: October 26, 2021
// H file with button and flashlight functions.
// Lab 11
// TA: Matthew Yu
// Last edited: November 25, 2021

#ifndef BUTTONS_H
#define BUTTONS_H

#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>

#define PF0       (*((volatile uint32_t *)0x40025004))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF4       (*((volatile uint32_t *)0x40025040))

extern volatile uint8_t press_flag;


// Initialize buttons
// Port C for directional inputs. Port F for MODE and BACK inputs
void Buttons_Init(void);

// Initialize PortF
// PF0, PF4 are inputs. PF2 is heartbeat output
// PF0 BACK, PF4 MODE
void PortC_Init(void);


// Enable Timer2 for one-shot debouncing
void Debounce_Init(void);

// Disable button interrupts
void Buttons_Disable(void);

// Enable button interrupts
void Buttons_Enable(void);

// MODE/BACK input ISR
void GPIOPortC_Handler(void);

#endif
