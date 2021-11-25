// filename ******** Buttons.c ************** 
// Lisong Sun, Kerry Tu, Dakota Britton, Casey Kim
// Created: October 26, 2021
// C file with button and flashlight functions.
// Lab 11
// TA: Matthew Yu
// Last edited: November 25, 2021

#include "Buttons.h"
#include "Clock.h"


void Debounce_Init(void) {
	volatile uint32_t delay;
	SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate TIMER2
	delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
}

static void GPIOArm(void) {
	// arm PortC
	GPIO_PORTC_ICR_R = 0x20;          // clear flag
	GPIO_PORTC_IM_R |= 0x20;          // arm interrupt on PC5
	NVIC_PRI0_R = (NVIC_PRI0_R&0xFF0FFFFF)|0x00600000; // (g) priority 3
	NVIC_EN0_R = 1<<2;               // enable interrupt 2 in NVIC
}

static void Timer2Arm(void) {
	TIMER2_CTL_R &= ~TIMER_CTL_TAEN; // disable timer2A during setup
  TIMER2_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer2A initialization ****
                                   // configure for one-shot mode
  TIMER2_TAMR_R = 0x0000001;
  TIMER2_TAILR_R = 15999999;         // 200 ms reload
	TIMER2_TAPR_R = 0;               // bus clock resolution
  TIMER2_IMR_R = 0x00000001;// enable timeout interrupt
  TIMER2_ICR_R = TIMER_ICR_TATOCINT;// clear timer0A timeout flag
  // **** interrupt initialization ****
                                   // Timer2A=priority 2
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x40000000; // top 3 bits
  NVIC_EN0_R |= 1<<23;              // enable interrupt 19 in NVIC
	
	TIMER2_CTL_R |= TIMER_CTL_TAEN;  // enable timer2A 32-b, one-shot, interrupts
}

// Initialize PortC
// PC5 is input (misc)
// PC6 is flashlight (mode)
void PortC_Init(void) {
	SYSCTL_RCGCGPIO_R     |=  0x04;         // Activate clock for Port C
  while((SYSCTL_PRGPIO_R & 0x04) != 0x04){};  // Allow time for clock to start
   
  GPIO_PORTC_PCTL_R     &= ~0x0FF00000;   // regular GPIO
  GPIO_PORTC_AMSEL_R    &= ~0x60;         // disable analog function 
  GPIO_PORTC_DIR_R      &= ~0x20;         // inputs on PC5
	GPIO_PORTC_DIR_R      |= 0x40;					// output on PC6
  GPIO_PORTC_AFSEL_R    &= ~0x60;         // regular port function
  GPIO_PORTC_PUR_R       =  0x20;         // enable pull-up on PC5
  GPIO_PORTC_DEN_R      |=  0x60;         // enable digital port 
	
	//enable interrupts
	GPIO_PORTC_IS_R &= ~0x20;         // PC5 is edge-sensitive
  GPIO_PORTC_IBE_R &= ~0x20;        // PC5 is not both edges
	GPIO_PORTC_IEV_R &= ~0x20;         // PC5 is rising edge event
};

// Disable button interrupts
void Buttons_Disable(void) {
	GPIO_PORTC_IM_R &= ~0x20; // disarm interrupt on PC5
}

// Enable button interrupts
void Buttons_Enable(void) {
	GPIO_PORTC_IM_R |= 0x20; // enable interrupt on PC5
}

// Initialize buttons
// Port C for directional inputs. Port F for MODE and BACK inputs
void Buttons_Init(void) {
	PortC_Init();
	GPIOArm();
	Debounce_Init();
};

// Directional input ISR
void GPIOPortC_Handler(void) {
	Buttons_Disable();

	Timer2Arm();
};

void Timer2A_Handler(void) {
	TIMER2_IMR_R = 0x00000000;// disarm interrupts
	TIMER2_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer2A timeout
	// process PortC inputs
	if (GPIO_PORTC_RIS_R & 0x20) { //PCfive, MODE/MISC
	  press_flag = 1;
	}
	
	GPIOArm();
}
