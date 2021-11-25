// filename ******** APICaller.c ************** 
// Lisong Sun, Kerry Tu, Dakota Britton, Casey Kim
// Created: October 26, 2021
// C file with functions to make API calls with the ESP8266.
// Lab 11
// TA: Matthew Yu
// Last edited: November 25, 2021

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "APICaller.h"
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "../inc/PLL.h"
#include "Timer3.h"
#include "UART.h"
#include "esp8266.h"


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

// Strings for desired data fields and other variables
const char weatherStatusField[9] = "\"main\":\"";
const char tempField[8] = "\"temp\":";
const char humField[13] = "\"humidity\":";

const char dtField[13] = "\"datetime\":\"";
const char doWField[16] = "\"day_of_week\":";

#define dateLength		 10	//length of YYYY-MM-DD
#define timeLength		 5	//length of HH:MM
#define timeStart			 11 //starting point of time in data response

// These variables contain the most recent ESP8266 to TM4C123 message
char serial_buf[2048];

char weatherStatus[10] = "---";		//slightly larger to account for value changes that increase length
char temperature[10] = "--.--";			//
char humidity[10] = "--";				//

char datetime[36] = "---";				//
char date[11];										//
char doW[2] = "-";
char time[6];											//

// ----------------------------------- ParseForField ------------------------------
// Send data to the ESP
void Parse_For_Field(char* response, const char* field, char* valueHold)
{
	char* fieldInResponse = strstr(response, field);
	if(fieldInResponse)
	{
		fieldInResponse += strlen(field);
		int i = 0;
		char data;
		while(1)
		{
			data = fieldInResponse[i];
			if(data == ',' || data == '}' || data == '\"')
			{
				valueHold[i] = '\0';
				break;
			}
			valueHold[i] = data;
			i++;
		}
	}
}


 
// -------------------------   ESP8266_to_TM4C  -----------------------------------
// This routine receives the  ESP8266 data and parses the
// data and feeds the commands to the TM4C.
void ESP8266_to_TM4C(void){int j; char data;
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
		if(serial_buf[0] - 48 == 0)
		{
			Parse_For_Field(serial_buf, tempField, temperature);
			Parse_For_Field(serial_buf, humField, humidity);
			Parse_For_Field(serial_buf, weatherStatusField, weatherStatus);
		}
		else if(serial_buf[0] - 48 == 1)
		{
			Parse_For_Field(serial_buf, dtField, datetime);
			Parse_For_Field(serial_buf, doWField, doW);
		}
#ifdef DEBUG1
		UART_OutString("DebugStart\n\r");
    UART_OutString(API_Get_WeatherStatus());
    UART_OutString("\n\r");
		UART_OutString(API_Get_Temperature());
    UART_OutString("\n\r");
		UART_OutString(API_Get_Humidity());
    UART_OutString("\n\r");
		UART_OutString(datetime);
    UART_OutString("\n\r");
		UART_OutString(API_Get_Date());
    UART_OutString("\n\r");
		UART_OutString(API_Get_Time());
    UART_OutString("\n\r");
#endif
  }  
}

//returns a string that is the weather status (clear, cloudy, etc.)
char* API_Get_WeatherStatus(void)
{
	return weatherStatus;
}

//returns temperature in degree F
char* API_Get_Temperature(void)
{
	return temperature;
}

//returns humidity in %
char* API_Get_Humidity(void)
{
	return humidity;
}

//returns the date in YYYY-MM-DD
char* API_Get_Date(void)
{
	if(datetime[0] == '-')
	{
		return "---";
	}
	
	char data;
	int i;
	for(i = 0; i < dateLength; i++)
	{
		data = datetime[i];
		date[i] = data;
	}
	
	date[dateLength] = '\0';
	
	return date;
}

//returns the day of the week
char* API_Get_DoW(void) {
	int intdoW = atoi(doW);
	switch (intdoW) {
		case 0: {
			return "Sun";
		} break;
		case 1: {
			return "Mon";
		} break;
		case 2: {
			return "Tue";
		} break;
		case 3: {
			return "Wed";
		} break;
		case 4: {
			return "Thu";
		} break;
		case 5: {
			return "Fri";
		} break;
		case 6: {
			return "Sat";
		} break;
		default: {
			return "---";
		} break;
	}
}

//returns the time in HH:MM
char* API_Get_Time(void)
{
	if(datetime[0] == '-')
	{
		return "---";
	}
	char data;
	int i;
	for(i = 0; i < timeLength; i++)
	{
		data = datetime[timeStart + i];
		time[i] = data;
	}
	
	time[timeLength] = '\0';
	
	return time;
}

void APICaller_Init(void) {
	ESP8266_Init();       // Enable ESP8266 Serial Port
  ESP8266_Reset();      // Reset the WiFi module
  ESP8266_SetupWiFi();
  Timer3_Init(&ESP8266_to_TM4C,80000000); 
  // check for receive data from Blynk App every 1s
}
	
