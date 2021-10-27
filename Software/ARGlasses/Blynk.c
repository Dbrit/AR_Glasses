// filename ******** Blynk.c ************** 
// Lisong Sun and Kerry Tu
// Created: September 20, 2021
// C file with the Blynk functions
// Lab 4
// TA: Hassan Iqbal
// Last edited: September 25, 2021

// TM4C123       ESP8266-ESP01 (2 by 4 header)
// PE5 (U5TX) to Pin 1 (Rx)
// PE4 (U5RX) to Pin 5 (TX)
// PE3 output debugging
// PE2 nc
// PE1 output    Pin 7 Reset
// PE0 input     Pin 3 Rdy IO2
//               Pin 2 IO0, 10k pullup to 3.3V  
//               Pin 8 Vcc, 3.3V (separate supply from LaunchPad 
// Gnd           Pin 4 Gnd  
// Place a 4.7uF tantalum and 0.1 ceramic next to ESP8266 3.3V power pin
// Use LM2937-3.3 and two 4.7 uF capacitors to convert USB +5V to 3.3V for the ESP8266
// http://www.ti.com/lit/ds/symlink/lm2937-3.3.pdf
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "PLL.h"
#include "Timer2.h"
#include "Timer3.h"
#include "UART.h"
//#include "PortF.h"
#include "esp8266.h"
#include "Lab3.h"

void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

uint32_t LastH = 99;    // VP74
uint32_t LastM = 99;    // VP75
uint32_t LastS = 99;    // VP76
// These 6 variables contain the most recent Blynk to TM4C123 message
// Blynk to TM4C123 uses VP0 to VP15
char serial_buf[64];
char Pin_Number[2]   = "99";       // Initialize to invalid pin number
char Pin_Integer[8]  = "0000";     //
char Pin_Float[8]    = "0.0000";   //
uint32_t pin_num; 
uint32_t pin_int;
 
// globals from Lab 3
// globals for time
volatile uint32_t hour;
volatile uint32_t minute;
volatile uint32_t second;
volatile uint32_t time;

// semaphores for button inputs
volatile uint8_t menuPress_flag; 
volatile uint8_t backPress_flag;
volatile uint8_t directionalPress_flag;
volatile uint8_t press_flag; //menu, back, dir

// current menu global
enum MenuMode currentMenu;

// timeout global
volatile int32_t timeout;
volatile uint8_t timeout_flag;
 
// ----------------------------------- TM4C_to_Blynk ------------------------------
// Send data to the Blynk App
// It uses Virtual Pin numbers between 70 and 99
// so that the ESP8266 knows to forward the data to the Blynk App
void TM4C_to_Blynk(uint32_t pin,uint32_t value){
  if((pin < 70)||(pin > 99)){
    return; // ignore illegal requests
  }
// your account will be temporarily halted 
  // if you send too much data
  ESP8266_OutUDec(pin);       // Send the Virtual Pin #
  ESP8266_OutChar(',');
  ESP8266_OutUDec(value);      // Send the current value
  ESP8266_OutChar(',');
  ESP8266_OutString("0.0\n");  // Null value not used in this example
}
 
 
// -------------------------   Blynk_to_TM4C  -----------------------------------
// This routine receives the Blynk Virtual Pin data via the ESP8266 and parses the
// data and feeds the commands to the TM4C.
void Blynk_to_TM4C(void){int j; char data;
// Check to see if a there is data in the RXD buffer
  if(ESP8266_GetMessage(serial_buf)){  // returns false if no message
    // Read the data from the UART5
#ifdef DEBUG1
    j = 0;
    do{
      data = serial_buf[j];
      UART_OutChar(data);        // Debug only
      j++;
    }while(data != '\n');
    UART_OutChar('\r');        
#endif
           
// Rip the 3 fields out of the CSV data. The sequence of data from the 8266 is:
// Pin #, Integer Value, Float Value.
    strcpy(Pin_Number, strtok(serial_buf, ","));
    strcpy(Pin_Integer, strtok(NULL, ","));       // Integer value that is determined by the Blynk App
    strcpy(Pin_Float, strtok(NULL, ","));         // Not used
    pin_num = atoi(Pin_Number);     // Need to convert ASCII to integer
    pin_int = atoi(Pin_Integer);  
  // ---------------------------- MODE VP #1 ----------------------------------------
    if(pin_num == 0x01)  {  
      menuPress_flag = 1;
			press_flag = menuPress_flag*100 + backPress_flag*10 + directionalPress_flag;
#ifdef DEBUG3
      Output_Color(ST7735_CYAN);
      ST7735_OutString("Rcv VP1 data=");
      ST7735_OutUDec(LED);
      ST7735_OutChar('\n');
#endif
    }          
  // ---------------------------- BACK VP #2 ----------------------------------------		
		if (pin_num == 2) {
			backPress_flag = 1;
			press_flag = menuPress_flag*100 + backPress_flag*10 + directionalPress_flag;
		}
		// ---------------------------- UP VP #3 ----------------------------------------		
		if (pin_num == 3) {
			directionalPress_flag = 1;
			press_flag = menuPress_flag*100 + backPress_flag*10 + directionalPress_flag;
		}
		// ---------------------------- DOWN VP #4 ----------------------------------------		
		if (pin_num == 4) {
			directionalPress_flag = 3;
			press_flag = menuPress_flag*100 + backPress_flag*10 + directionalPress_flag;
		}
		// ---------------------------- LEFT VP #5 ----------------------------------------		
		if (pin_num == 5) {
			directionalPress_flag = 4;
			press_flag = menuPress_flag*100 + backPress_flag*10 + directionalPress_flag;
		}
		// ---------------------------- RIGHT VP #6 ----------------------------------------		
		if (pin_num == 6) {
			directionalPress_flag = 2;
			press_flag = menuPress_flag*100 + backPress_flag*10 + directionalPress_flag;
		}
		// ---------------------------- HOUR+ VP #7 ----------------------------------------		
		if (pin_num == 7) {
			hour++;
			if(hour > 23){
				hour = 0;
			}
			time = second + 100*minute + 10000*hour; 
		}
		// ---------------------------- HOUR- VP #8 ----------------------------------------		
		if (pin_num == 8) {
			if(hour == 0){
				hour = 23;
			}
			else{
				hour--;
			}
			time = second + 100*minute + 10000*hour; 
		}
		// ---------------------------- MIN+ VP #9 ----------------------------------------		
		if (pin_num == 9) {
			minute++;
			if(minute > 59){
				minute = 0;
				if(++hour > 24){
					hour = 0;
				}
			}
			time = second + 100*minute + 10000*hour; 
		}
		// ---------------------------- MIN- VP #10 ----------------------------------------		
		if (pin_num/10 == 10) {
			if(minute == 0){
				minute = 59;
				if(hour == 0){
					hour = 23;
				}
				else{
					hour--;
				}
			}
			else{
				minute--;
			}
			time = second + 100*minute + 10000*hour; 
		}
		// ---------------------------- SEC+ VP #11 ----------------------------------------		
		if (pin_num/10 == 11) {
			if (++second == 60) {
				second = 0;
				if (++minute == 60) {
					minute = 0;
					if (++hour == 24) {
						hour = 0;
					}
				}
			}
			time = second + 100*minute + 10000*hour; 
		}
		// ---------------------------- SEC- VP #12 ----------------------------------------		
		if (pin_num/10 == 12) {
			if (second == 0) {
				second = 59;
				if(minute == 0){
					minute = 59;
					if(hour == 0){
						hour = 23;
					}
					else{
						hour--;
					}
				}
				else{
					minute--;
				}
			}
			else {
				second--;
			}
			time = second + 100*minute + 10000*hour; 
		}
		
		
// Parse incoming data      
#ifdef DEBUG1
    UART_OutString(" Pin_Number = ");
    UART_OutString(Pin_Number);
    UART_OutString("   Pin_Integer = ");
    UART_OutString(Pin_Integer);
    UART_OutString("   Pin_Float = ");
    UART_OutString(Pin_Float);
    UART_OutString("\n\r");
#endif
  }  
}

void SendInformation(void){
	uint32_t ctime = time;
  uint32_t thisH = ctime / 10000;
	uint32_t thisM = (ctime%10000) / 100;
	uint32_t thisS = ctime%100;
// your account will be temporarily halted if you send too much data
  if(thisH != LastH){
    TM4C_to_Blynk(74, thisH);  // send hours
#ifdef DEBUG3
    Output_Color(ST7735_WHITE);
    ST7735_OutString("Send 74 data=");
    ST7735_OutUDec(thisH);
    ST7735_OutChar('\n');
#endif
  }
	if(thisM != LastM){
    TM4C_to_Blynk(75, thisM);  // send minutes
#ifdef DEBUG3
    Output_Color(ST7735_WHITE);
    ST7735_OutString("Send 75 data=");
    ST7735_OutUDec(thisM);
    ST7735_OutChar('\n');
#endif
  }
	if(thisS != LastS){
    TM4C_to_Blynk(76, thisS);  // send seconds
#ifdef DEBUG3
    Output_Color(ST7735_WHITE);
    ST7735_OutString("Send 76 data=");
    ST7735_OutUDec(thisS);
    ST7735_OutChar('\n');
#endif
  }
  LastH = thisH;
	LastM = thisM;
	LastS = thisS;
}

  
int main(void){       
  PLL_Init(Bus80MHz);   // Bus clock at 80 MHz
  DisableInterrupts();  // Disable interrupts until finished with inits
#ifdef DEBUG3
  Output_Init();        // initialize ST7735
  ST7735_OutString("EE445L Lab 4D\nBlynk example\n");
#endif
#ifdef DEBUG1
  UART_Init(5);         // Enable Debug Serial Port
  UART_OutString("\n\rEE445L Lab 4D\n\rBlynk example");
#endif
  ESP8266_Init();       // Enable ESP8266 Serial Port
  ESP8266_Reset();      // Reset the WiFi module
  ESP8266_SetupWiFi();  // Setup communications to Blynk Server  
  
  Timer2_Init(&Blynk_to_TM4C,800000); 
  // check for receive data from Blynk App every 10ms

  Timer3_Init(&SendInformation,40000000); 
  // Send data back to Blynk App every 1/2 second
  // EnableInterrupts();

  Lab3_main();
}


