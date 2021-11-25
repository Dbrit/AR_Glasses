// filename ******** main.c ************** 
// Lisong Sun, Kerry Tu, Dakota Britton, Casey Kim
// Created: October 26, 2021
// C file with main functions for the HUDset.
// Lab 11
// TA: Matthew Yu
// Last edited: November 25, 2021

// ST7735 Connections
// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected 
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// TFT_CS2 connected to PB4
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO)
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

// Port C Connections
// PC4 connected to DHT11 signal
// PC5 connected to the button
// PC6 connected to the flashlight LED

// ESP8266 Connections
// PE0 to 8266_GPIO2
// PE1 to 8266_Reset
// PE4 to 8266_TX
// PE5 to 8266_RX

// ICM20948 Connections
// PD0 to SSI1_SCK
// PD1 to SSI1_FS
// PD2 to SSI1_MISO
// PD3 to SSI1_MOSI
// PD4 to SSI1_CS

//#include <stdio.h>
#include <stdint.h>
//#include "string.h"
#include "../inc/PLL.h"
#include "../inc/tm4c123gh6pm.h"
#include "../inc/LaunchPad.h"
#include "../inc/CortexM.h"
#include "Buttons.h"
#include "Clock.h"
#include "DHT11.h"
#include "esp8266.h"
#include "APICaller.h"
#include "ICM20948.h"
#include "../inc/Unified_Port_Init.h"
#include "tests.h"

// semaphores for button inputs
volatile uint8_t press_flag;

// current menu global
enum MenuMode currentMenu;

// Initialize the next menu
void ChangeMenu(void) {
	switch (currentMenu) {
		case TimeMode: {
			Clock_ShowTimeMenu();
		} break;
		case WeatherMode: {
			Clock_ShowWeatherMenu();
		} break;
		case TempMode: {
			Clock_ShowTempMenu();
		} break;
		case CompassMode: {
			Clock_ShowCompassMenu();
		} break;
		case BlankMode:{
			Clock_ShowBlankMenu();
		} break;
		case LightMode:{
			Clock_ShowLightMenu();
		} break;
		default:
			break;
	}
}

// Display startup animation
void Welcome_Screen(void) {
	ST7735_FillScreen(ST7735_BLACK);
	for (int i = 0; i <= 31; i++) {
		uint16_t color = (i<<11)+(i<<6)+i;
		ST7735_SetTextColor(color); 
		ST7735_SetCursor(4,5);
		ST7735_OutString("Welcome to HUD");
		SysTick80_Wait10ms(3);
	}
	
	SysTick80_Wait10ms(150);
	
	for (int i = 31; i >= 0; i--) {
		uint16_t color = (i<<11)+(i<<6)+i;
		ST7735_SetTextColor(color); 
		ST7735_SetCursor(4,5);
		ST7735_OutString("Welcome to HUD");
		SysTick80_Wait10ms(3);
	}
	
	ST7735_SetTextColor(ST7735_WHITE);
	SysTick80_Wait10ms(100);
}
// Initialize the ST7735 + chip selects
void LCD_Init(void) {
	// startup LCDs
	Port_B_Init(); // chip select
  ST7735_InitR(INITR_REDTAB);
	PA3_Init();    // chip select
	ST7735_SetBGColor(ST7735_BLACK);
	ST7735_SetTextColor(ST7735_WHITE);
	ST7735_SetRotation(3);
	ST7735_FillScreen(ST7735_BLACK);
}

int main(void){
	// Initialization
  PLL_Init(Bus80MHz); 
	DisableInterrupts();
	SysTick_Init();
	// Bootup
	LCD_Init();
	//Test_LCD();
	//Test_Image();
	
	Buttons_Init();
	
	ST7735_SetCursor(7,2);
	ST7735_OutString("Init DHT...");
	DHT11_Init();
	ST7735_OutString("S");
	//Test_DHT11();
	ICM_Boot();
	//Test_ICM20948();
	ST7735_SetCursor(7,5);
	ST7735_OutString("Init ESP...");
	APICaller_Init();
	//Test_API();
	ST7735_OutString("S");
	Clock_Init();
	ST7735_SetCursor(7,6);
	ST7735_OutString("Starting up!");
	Welcome_Screen();
	
	EnableInterrupts();
	ChangeMenu();
	
	// Main logic
  while(1){

		uint8_t press = press_flag;
		press_flag = 0;
		uint8_t menu = press;
		
		if (menu == 1) {
			press_flag = 0;
			
			if(currentMenu == BlankMode){
				currentMenu = TimeMode;
			}
			else{
				currentMenu = (currentMenu + 1);
			}
			ChangeMenu();
		}
		
		switch (currentMenu) {
			case TimeMode: {
				Clock_UpdateTimeMenu();
			} break;
			case WeatherMode: {
				Clock_UpdateWeatherMenu();
			} break;
			case TempMode: {
				Clock_UpdateTempMenu();
			} break;
			case CompassMode: {
				Clock_UpdateCompassMenu();
			} break;
			case BlankMode:{
				Clock_UpdateBlankMenu();
			} break;
			case LightMode:{
				Clock_UpdateLightMenu();
			} break;
			default:
				break;
		}
		
  }
} 

