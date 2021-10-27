// filename ******** Speaker.h ************** 
// Lisong Sun and Kerry Tu
// Created: September 16, 2021
// Header file with alarm functions
// Lab 4
// TA: Hassan Iqbal
// Last edited: September 20, 2021

#ifndef SPEAKER_H
#define SPEAKER_H

#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>
#include "../inc/CortexM.h"

#define PB1       (*((volatile uint32_t *)0x40005008))

// Initialize speaker.
// Utilize Timer0
void Speaker_Init(void);

// Turn on the speaker
void Speaker_Enable(void);

// Turn off the speaker
void Speaker_Disable(void);

// PB1 is output
void PortB_Init(void);

// Initialize Timer 0 to 500Hz
void Timer0_Init_500Hz(void);

// Toggle output square wave
void Timer0A_Handler(void);

#endif
