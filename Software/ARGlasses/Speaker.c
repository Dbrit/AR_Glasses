// filename ******** Speaker.c ************** 
// Lisong Sun and Kerry Tu
// Created: September 16, 2021
// C file with alarm functions
// Lab 4
// TA: Hassan Iqbal
// Last edited: September 20, 2021

#include "Speaker.h"

// Initialize speaker.
// Utilize Timer1
void Speaker_Init(void) {
	PortB_Init();
	Timer0_Init_500Hz();
	PB1 = 0;
}

// Turn on the speaker
void Speaker_Enable(void) {
	NVIC_EN0_R = 1<<19;              // enable interrupt 19 in NVIC
}

// Turn off the speaker
void Speaker_Disable(void) {
	NVIC_DIS0_R = 1<<19;              // disable interrupt 19 in NVIC
}

// PB1 is output
void PortB_Init(void) {
  SYSCTL_RCGCGPIO_R     |= 0x02;            // activate port B
  while((SYSCTL_PRGPIO_R & 0x02) == 0){};   // Wait
                                      
  GPIO_PORTB_PCTL_R     &= ~0x000000F0;     // GPIO
  GPIO_PORTB_DIR_R      |=  0x02;           // make PB3-0 out
  GPIO_PORTB_AFSEL_R    &= ~0x02;           // regular port function
  GPIO_PORTB_DEN_R      |=  0x02;           // enable digital I/O on PB3-0
  GPIO_PORTB_AMSEL_R    &= ~0x02;           // disable analog functionality on PB3-0
  
}

// Initialize Timer 0 to 500Hz
void Timer0_Init_500Hz(void) {
	volatile uint32_t delay;
  DisableInterrupts();
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x01;      // activate timer0
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer0A initialization ****
                                   // configure for periodic mode
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  TIMER0_TAILR_R = 79999;         // start value for 500 Hz interrupts
	
	// **** debug for time dumping
	//TIMER0_TAILR_R = 0xFFFFFFFF;
	//TIMER0_TAPR_R = 0;               // bus clock resolution
	
  TIMER0_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// clear timer0A timeout flag
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer0A=priority 4
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x80000000; // top 3 bits
  NVIC_DIS0_R = 1<<19;              // disable interrupt 19 in NVIC
}

// Toggle output square wave
void Timer0A_Handler(void) {
	TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
  PB1 ^= 0x02;                   // toggle square wave
}
