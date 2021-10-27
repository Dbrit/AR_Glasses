// filename ******** Lab3.c ************** 
// Lisong Sun and Kerry Tu
// Created: September 16, 2021
// C file with debugging functions as well as main functions for alarm clock.
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


#include "Lab3.h"

#define TIMEOUT 5 // second until auto timeout

// globals for time
extern volatile uint32_t hour;
extern volatile uint32_t minute;
extern volatile uint32_t second;
extern volatile uint32_t time;

// semaphores for button inputs
extern volatile uint8_t menuPress_flag; 
extern volatile uint8_t backPress_flag;
extern volatile uint8_t directionalPress_flag;
extern volatile uint8_t press_flag; //menu, back, dir

// current menu global
extern enum MenuMode currentMenu;

// timeout global
extern volatile int32_t timeout;
extern volatile uint8_t timeout_flag;

// time dumping vars. To use these, uncomment the corresponding lines in Speaker.c (use Timer0 for timing)
//static volatile uint32_t ctime = 0;
//static volatile uint32_t ltime = 0;
//static volatile uint32_t dtime = 0;
//static volatile uint32_t timebuffer[10];
//static volatile uint32_t bufferi = 0;

int test_button(void) {
	Buttons_Init();
	uint8_t menuPressed = 0;
	uint8_t directionPressed = 0;
	EnableInterrupts();
	while (1) {
		if (menuPress_flag == 1) {
			menuPress_flag = 0;
			menuPressed ^= 1;
		}
		if (directionalPress_flag == 1) {
			directionalPress_flag = 0;
			directionPressed ^= 1;
		}			
	}
}

int test_speaker(void) {
	PLL_Init(Bus80MHz); 
	Speaker_Init();
	Buttons_Init();
	Speaker_Enable();
	EnableInterrupts();
	while(1);
}

int test_clock(void) {
	PLL_Init(Bus80MHz); 
	Clock_Init();
	EnableInterrupts();
	//enum MenuMode currentMenu = Clock;
	while (1) {
		Clock_ShowClock();
		uint32_t volatile time;
		int32_t n = 500;
		while(n){
			Clock_UpdateClock();
			time = 727240*2/91;  // 10msec
			while(time){
				time--;
			}
			n--;
		}
		Clock_ShowMenu1();
		n = 500;
		while(n){
			Clock_UpdateMenu1();
			time = 727240*2/91;  // 10msec
			while(time){
				time--;
			}
			n--;
		}
		Clock_ShowMenu2();
		n = 500;
		while(n){
			Clock_UpdateMenu2();
			time = 727240*2/91;  // 10msec
			while(time){
				time--;
			}
			n--;
		}
		Clock_ShowMenu3();
		n = 500;
		while(n){
			time = 727240*2/91;  // 10msec
			while(time){
				time--;
			}
			n--;
		}
	}
}


void ChangeMenu(void) {
	switch (currentMenu) {
		case Clock: {
			Clock_ShowClock();
		} break;
		case TimeConfig: {
			Clock_ShowMenu1();
		} break;
		case AlarmConfig: {
			Clock_ShowMenu2();
		} break;
		case AlarmActivate: {
			Clock_ShowMenu3();
		} break;
		default:
			break;
	}
}


int Lab3_main(void){
	// Initialization
  // PLL_Init(Bus80MHz); 
	DisableInterrupts();
  Clock_Init();
	Buttons_Init();
	Speaker_Init();
	timeout_flag = 0;
	
	EnableInterrupts();
	
	// Main logic
  while(1){
		
		// Debug. Calculate time to refresh frames
		/*
		ctime = TIMER0_TAR_R;
		dtime = ltime - ctime;
		ltime = ctime;
		if (bufferi < 10) {
			timebuffer[bufferi++] = dtime;
		} else {
			while (1);
		}
		*/
		uint8_t press = press_flag;
		press_flag = 0;
		uint8_t menu = press /100;
		uint8_t back = (press%100)/10;
		uint8_t dir = press%10;
		
		if (menu || back || dir) {
			timeout = TIMEOUT;
			timeout_flag = 1;
		}
		if (timeout <= 0) {
			timeout_flag = 0;
			timeout = TIMEOUT;
			currentMenu = Clock;
			ChangeMenu();
		}
		if (menu == 1) {
			menuPress_flag = 0;
			if(currentMenu == AlarmActivate){
				Speaker_Disable();
				Clock_AlarmShutoff();
				currentMenu = Clock;
			} else if (++currentMenu > 2) {
				currentMenu = Clock;
			}
			ChangeMenu();
		}
		if (back == 1) {
			backPress_flag = 0;
			if(currentMenu == AlarmActivate){
				Speaker_Disable();
				Clock_AlarmSnooze();
				currentMenu = Clock;
			} else {
				currentMenu = Clock;
			}
			ChangeMenu();
		}
		
		switch (currentMenu) {
			case Clock: {
				Clock_UpdateClock();
			} break;
			case TimeConfig: {
				Clock_UpdateMenu1();
			} break;
			case AlarmConfig: {
				Clock_UpdateMenu2();
			} break;
			case AlarmActivate: {
				Clock_ShowMenu3();
				Speaker_Enable();
			} break;
			default:
				break;
		}
  }
} 

