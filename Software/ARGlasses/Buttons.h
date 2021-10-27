// filename ******** Buttons.h ************** 
// Lisong Sun and Kerry Tu
// Created: September 16, 2021
// Header file for button functions
// Lab 4
// TA: Hassan Iqbal
// Last edited: September 20, 2021

#ifndef BUTTONS_H
#define BUTTONS_H

#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>

#define PF0       (*((volatile uint32_t *)0x40025004))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF4       (*((volatile uint32_t *)0x40025040))
#define PC4       (*((volatile uint32_t *)0x40006040))
#define PC5       (*((volatile uint32_t *)0x40006080))
#define PC6       (*((volatile uint32_t *)0x40006100))
#define PC7       (*((volatile uint32_t *)0x40006200))

extern volatile uint8_t menuPress_flag; // semaphore for button inputs
extern volatile uint8_t directionalPress_flag;
extern volatile uint8_t backPress_flag;
extern volatile uint8_t press_flag;


// Initialize buttons
// Port C for directional inputs. Port F for MODE and BACK inputs
void Buttons_Init(void);

// Initialize PortF
// PF0, PF4 are inputs. PF2 is heartbeat output
// PF0 BACK, PF4 MODE
void PortF_Init(void);

// Initialize PortC
// PC4,PC5,PC6,PC7 are inputs
// PC4 up, PC5 right, PC6 left, PC7 down
void PortC_Init(void);

// Enable Timer2 for one-shot debouncing
void Debounce_Init(void);

// Disable button interrupts
void Buttons_Disable(void);

// Enable button interrupts
void Buttons_Enable(void);

// MODE/BACK input ISR
void GPIOPortF_Handler(void);

// Directional input ISR
void GPIOPortC_Handler(void);

#endif
