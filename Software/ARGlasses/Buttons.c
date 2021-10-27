// filename ******** Buttons.c ************** 
// Lisong Sun and Kerry Tu
// Created: September 16, 2021
// C file for button functions
// Lab 4
// TA: Hassan Iqbal
// Last edited: September 20, 2021

#include "Buttons.h"
#include "Clock.h"

void Debounce_Init(void) {
	volatile uint32_t delay;
	SYSCTL_RCGCTIMER_R |= 0x10;   // 0) activate TIMER4
	delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
}

static void GPIOArm(void) {
	// arm PortF
	GPIO_PORTF_ICR_R = 0x11;      // (e) clear flag
  GPIO_PORTF_IM_R |= 0x11;      // (f) arm interrupt on PF4,0
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00600000; // (g) priority 3
  NVIC_EN0_R = 1<<30;               // enable interrupt 30 in NVIC
	// arm PortC
	GPIO_PORTC_ICR_R = 0xF0;          // clear flag
	GPIO_PORTC_IM_R |= 0xF0;          // arm interrupt on PF4, 0
	NVIC_PRI0_R = (NVIC_PRI0_R&0xFF0FFFFF)|0x00600000; // (g) priority 3
	NVIC_EN0_R = 1<<2;               // enable interrupt 2 in NVIC
}

static void Timer4Arm(void) {
	TIMER4_CTL_R &= ~TIMER_CTL_TAEN;  // disable timer4A 32-b, one-shot, interrupts
  TIMER4_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER4_TAMR_R = 0x00000001;   // 3) configure for periodic mode, default down-count settings
  TIMER4_TAILR_R = 15999999;         // 200 ms reload
  TIMER4_TAPR_R = 0;            // 5) bus clock resolution
  TIMER4_ICR_R = 0x00000001;    // 6) clear TIMER3A timeout flag
  TIMER4_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI17_R = (NVIC_PRI17_R&0xFF00FFFF)|(2<<21); // priority 2
// interrupts enabled in the main program after all devices initialized
// vector number 86, interrupt number 70
  NVIC_EN2_R = 0x00000040;      // 9) enable interrupt 70 in NVIC
  TIMER4_CTL_R |= TIMER_CTL_TAEN;    // 10) enable TIMER4A
}


// Initialize PortF
// PF0, PF4 are inputs. PF2 is heartbeat output
void PortF_Init(void) {
	SYSCTL_RCGCGPIO_R |= 0x20;        // 1) activate clock for Port F
  while((SYSCTL_PRGPIO_R&0x20)==0){}; // allow time for clock to start
                                    
	GPIO_PORTF_LOCK_R  = 0x4C4F434B;  // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R    = 0x1F;        // allow changes to PF4-0
  GPIO_PORTF_PCTL_R &= ~0x000F0F0F; // 3) regular GPIO
  GPIO_PORTF_AMSEL_R &= ~0x15;      // 4) disable analog function on PF2, PF4, PF0
  GPIO_PORTF_PUR_R |= 0x11;         // 5) pullup for PF4, PF0
  GPIO_PORTF_DIR_R |= 0x04;         // 5) set direction to output
  GPIO_PORTF_AFSEL_R &= ~0x15;      // 6) regular port function
  GPIO_PORTF_DEN_R |= 0x15;         // 7) enable digital port
		
	// enable interrupts
  GPIO_PORTF_IS_R &= ~0x11;         // PF4,0 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x11;        // PF4,0 is not both edges
	GPIO_PORTF_IEV_R &= ~0x11;         // PF4,0 is rising edge event
		
		// arm PortF
	GPIO_PORTF_ICR_R = 0x11;      // (e) clear flag
  GPIO_PORTF_IM_R |= 0x11;      // (f) arm interrupt on PF4,0
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00600000; // (g) priority 3
  NVIC_EN0_R = 1<<30;               // enable interrupt 30 in NVIC
	
};

// Initialize PortC
// PC4,PC5,PC6,PC7 are inputs
// PC4 up, PC5 right, PC6 left, PC7 down
void PortC_Init(void) {
	SYSCTL_RCGCGPIO_R     |=  0x04;         // Activate clock for Port C
  while((SYSCTL_PRGPIO_R & 0x04) != 0x04){};  // Allow time for clock to start
   
  GPIO_PORTC_PCTL_R     &= ~0xFFFF0000;   // regular GPIO
  GPIO_PORTC_AMSEL_R    &= ~0xF0;         // disable analog function 
  GPIO_PORTC_DIR_R      &= ~0xF0;         // inputs on PC7-PC4
  GPIO_PORTC_AFSEL_R    &= ~0xF0;         // regular port function
  GPIO_PORTC_PUR_R       =  0xF0;         // enable pull-up on PC7-PC4	
  GPIO_PORTC_DEN_R      |=  0xF0;         // enable digital port 
	
	//enable interrupts
	GPIO_PORTC_IS_R &= ~0xF0;         // PF4,0 is edge-sensitive
  GPIO_PORTC_IBE_R &= ~0xF0;        // PF4,0 is not both edges
	GPIO_PORTC_IEV_R &= ~0xF0;         // PF4,0 is rising edge event
		
		// arm PortC
	GPIO_PORTC_ICR_R = 0xF0;          // clear flag
	GPIO_PORTC_IM_R |= 0xF0;          // arm interrupt on PF4, 0
	NVIC_PRI0_R = (NVIC_PRI0_R&0xFF0FFFFF)|0x00600000; // (g) priority 3
	NVIC_EN0_R = 1<<2;               // enable interrupt 2 in NVIC
};
// Disable button interrupts
void Buttons_Disable(void) {
	GPIO_PORTF_IM_R &= ~0x11; //disable interrupts on PF4,0
	GPIO_PORTC_IM_R &= ~0xF0; // disarm interrupt on PC4,5,6,7
}

// Enable button interrupts
void Buttons_Enable(void) {
	GPIO_PORTF_IM_R |= 0x11; //enable interrupts on PF4,0
	GPIO_PORTC_IM_R |= 0xF0; //arm interrupt on PC4,5,6,7
}

// Initialize buttons
// Port C for directional inputs. Port F for MODE and BACK inputs
void Buttons_Init(void) {
	PortF_Init();
	PortC_Init();
	GPIOArm();
	Debounce_Init();
};

// MODE/BACK input ISR
void GPIOPortF_Handler(void) {
	//GPIO_PORTF_ICR_R = 0x11;      // (e) clear flag
	Buttons_Disable();

	Timer4Arm();
};

// Directional input ISR
void GPIOPortC_Handler(void) {
	//GPIO_PORTC_ICR_R = 0xF0;          // clear flag
	Buttons_Disable();

	Timer4Arm();
};

void Timer4A_Handler(void) {
	TIMER4_IMR_R = 0x00000000;// disarm interrupts
	TIMER4_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER4A timeout
	// process PortF inputs
	if (GPIO_PORTF_RIS_R & 0x10) { //PF4, MODE
	  menuPress_flag = 1;
	}
	if (GPIO_PORTF_RIS_R & 0x01) { //PF0, BACK
		backPress_flag = 1;
	}
	// process PortC inputs
	if (GPIO_PORTC_RIS_R & 0x10) { //PC4, UP
		directionalPress_flag = 1;
	} else if (GPIO_PORTC_RIS_R & 0x20) { //PC5, RIGHT
		directionalPress_flag = 2;
	} else if (GPIO_PORTC_RIS_R & 0x40) { //PC6, LEFT
		directionalPress_flag = 4;
	} else if (GPIO_PORTC_RIS_R & 0x80) { //PC7, DOWN
		directionalPress_flag = 3;
	}
	press_flag = menuPress_flag*100 + backPress_flag*10 + directionalPress_flag;
	
	GPIOArm();
}
