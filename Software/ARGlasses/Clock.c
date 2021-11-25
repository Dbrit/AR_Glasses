// filename ******** Clock.c ************** 
// Lisong Sun, Kerry Tu, Dakota Britton, Casey Kim
// Created: October 26, 2021
// C file with LCD functions for each menu.
// Lab 11
// TA: Matthew Yu
// Last edited: November 25, 2021

#include "Clock.h" 
#include "DHT11.h"
#include "ICM20948.h"
#include "APICaller.h"
#include <math.h>
#include "Images.h"

static uint32_t prevDHT11_Temp;
static uint32_t prevDHT11_Humidity;
static uint32_t prevDirection;
static uint32_t prevBlynk_Temp;
static uint32_t prevBlynk_Humidity;

static char strnDate[3] = {0};
static char strYear[5] = {0};
static char strMonth[3] = {0};

#define COMPASS_SIZE 80
const char label[COMPASS_SIZE] = "N         NE        E         SE        S         SW        W         NW        ";
const char marks[COMPASS_SIZE] = "|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-";

// Initialize the main display
void Clock_Init(void) {
	ST7735_SetTextColor(ST7735_WHITE);
	ST7735_SetBGColor(ST7735_BLACK);
	currentMenu = TimeMode;
	//Clock_ShowTimeMenu();
}

void Clock_ShowModeIndicator(void){
	uint16_t color;
	uint16_t colorInv;
	color = ST7735_WHITE;
	colorInv = ST7735_RED;

	if(currentMenu == TimeMode){
		ST7735_SetTextColor(colorInv);
	}
	else{
		ST7735_SetTextColor(color);
	}
	ST7735_SetCursor(8,2);
	ST7735_OutString("*");
	
	if(currentMenu == WeatherMode){
		ST7735_SetTextColor(colorInv);
	}
	else{
		ST7735_SetTextColor(color);
	}
	ST7735_SetCursor(11,2);
	ST7735_OutString("*");
	
	if(currentMenu == TempMode){
		ST7735_SetTextColor(colorInv);
	}
	else{
		ST7735_SetTextColor(color);
	}
	ST7735_SetCursor(14,2);
	ST7735_OutString("*");
	
	if(currentMenu == CompassMode){
		ST7735_SetTextColor(colorInv);
	}
	else{
		ST7735_SetTextColor(color);
	}
	ST7735_SetCursor(17,2);
	ST7735_OutString("*");
}

void Clock_ShowTimeMenu(void){
	ST7735_FillScreen(ST7735_BLACK);
	Clock_UpdateTimeMenu();
}

void Clock_ShowWeatherMenu(void){
	ST7735_FillScreen(ST7735_BLACK);
	//Clock_ShowModeIndicator();
	ST7735_SetCursor(6, 4);
	ST7735_OutString("Austin, TX");
	ST7735_DrawBitmap(84,86,humBMP,5,7);
	Clock_UpdateWeatherMenu();
}

void Clock_ShowTempMenu(void){
	ST7735_FillScreen(ST7735_BLACK);
	//Clock_ShowModeIndicator();
	ST7735_SetCursor(4, 4);
	ST7735_OutString("Temp:");
	ST7735_SetCursor(10, 4);
	uint32_t temp = DHT11_GetTemp();
	ST7735_sDecOut2(temp);
	ST7735_SetCursor(18, 4);
	ST7735_OutString("F");
	ST7735_SetCursor(4, 6);
	ST7735_OutString("Rel Hum:");
	ST7735_SetCursor(11, 6);
	uint32_t hum = DHT11_GetHumidity();
	ST7735_sDecOut2(hum);
	ST7735_SetCursor(18, 6);
	ST7735_OutString("%");
}

void Clock_ShowCompassMenu(void){
	ST7735_FillScreen(ST7735_BLACK);
	//Clock_ShowModeIndicator();

	
	ST7735_SetCursor(10,4);
	ST7735_OutString("^");
	
	uint32_t currentDirection = ICM20948_GetDirection();
	ST7735_SetCursor(10,5);
	ST7735_OutUDec(currentDirection);
	
	uint32_t lowerBound = (currentDirection * COMPASS_SIZE) / 360;
	
	uint32_t i = lowerBound - 7;
	for(uint8_t offset = 0; offset < 15; offset++){
		ST7735_SetCursor(offset+3, 2);
		ST7735_OutChar3D(label[i]);
		ST7735_SetCursor(offset+3, 3);
		ST7735_OutChar3D(marks[i]);
		i = (i+1)%COMPASS_SIZE;
	}
}

void Clock_ShowBlankMenu(void){
	PC6 &= ~0x40;
	ST7735_FillScreen(ST7735_BLACK);
	WaitForInterrupt();
}

void Clock_ShowLightMenu(void){
	ST7735_FillScreen(ST7735_BLACK);
	
	ST7735_DrawBitmap(130,30,flashlightBMP,25,25);
	PC6 |= 0x40;
}

void Clock_UpdateTimeMenu(void){
	char* strTime = API_Get_Time();
	char* strDate = API_Get_Date();
	
	int intDate = atoi(strncpy(strnDate,strDate+8,2));
	int intYear = atoi(strncpy(strYear,strDate,4));
	int intMonth = atoi(strncpy(strMonth,strDate+5,2));
	
	ST7735_SetCursor(4,4);					//day of week
	ST7735_OutString(API_Get_DoW());
	ST7735_OutString(",");
	
	
	ST7735_SetCursor(9,4);
	switch (intMonth) {
		case 1: {
			ST7735_OutString("Jan");
		} break;
		case 2: {
			ST7735_OutString("Feb");
		} break;
		case 3: {
			ST7735_OutString("Mar");
		} break;
		case 4: {
			ST7735_OutString("Apr");
		} break;
		case 5: {
			ST7735_OutString("May");
		} break;
		case 6: {
			ST7735_OutString("Jun");
		} break;
		case 7: {
			ST7735_OutString("Jul");
		} break;
		case 8: {
			ST7735_OutString("Aug");
		} break;
		case 9: {
			ST7735_OutString("Sep");
		} break;
		case 10: {
			ST7735_OutString("Oct");
		} break;
		case 11: {
			ST7735_OutString("Nov");
		} break;
		case 12: {
			ST7735_OutString("Dec");
		} break;
		default:
			break;
	}
	
	ST7735_SetCursor(13,4);					//num date
	ST7735_OutUDec2(intDate);
	
	ST7735_SetCursor(16,4);					//num year
	ST7735_OutUDec(intYear);
	
	// write out time
	ST7735_SetCursor(10, 6);
	ST7735_OutString(strTime);
//	ST7735_OutUDec2(toHour(currTime));
//	ST7735_OutString(":");
//	ST7735_OutUDec2(toMinute(currTime));
}

void Clock_UpdateWeatherMenu(void){
	char* weather = API_Get_WeatherStatus();
	char* temp = API_Get_Temperature();
	char* humidity = API_Get_Humidity();
	
	// show pic of weather
	//possible weather: Clear, Clouds, Snow, Rain, Drizzle, Thunderstorm
	if (strncmp(weather, "Clear", 5) == 0) {
		ST7735_DrawBitmap(100,85,sunnyBMP,25,25);
	} else if (strncmp(weather, "Clouds", 5) == 0) {
		ST7735_DrawBitmap(100,85,cloudyBMP,25,25);
	} else if (strncmp(weather, "Snow", 5) == 0) {
		ST7735_DrawBitmap(100,85,snowyBMP,25,25);
	} else if (strncmp(weather, "Rain", 5) == 0 || strncmp(weather, "Drizzle", 5) == 0) {
		ST7735_DrawBitmap(100,85,rainyBMP,25,25);
	} else if (strncmp(weather, "Thunderstorm", 5) == 0) {
		ST7735_DrawBitmap(100,85,thunderBMP,25,25);
	} else {
		ST7735_DrawBitmap(100,85,sunnyBMP,25,25);
	}
	
	ST7735_SetCursor(12, 6);
	ST7735_OutString(temp);
	ST7735_OutString(" F");
	ST7735_SetCursor(13, 8);
	ST7735_OutString(humidity);
	ST7735_OutString(" %");
}
void Clock_UpdateTempMenu(void){
	uint32_t temp = DHT11_GetTemp();
	if(temp != prevDHT11_Temp){
		ST7735_SetCursor(10, 4);
		ST7735_sDecOut2(temp);
		prevDHT11_Temp = temp;
	}
	uint32_t humidity = DHT11_GetHumidity();
	if(humidity != prevDHT11_Humidity){
		ST7735_SetCursor(11, 6);
		ST7735_sDecOut2(humidity);
		prevDHT11_Humidity = humidity;
	}
}
void Clock_UpdateCompassMenu(void){
	uint32_t currentDirection = ICM20948_GetDirection();
	ST7735_SetCursor(10,5);
	ST7735_OutUDec(currentDirection);
	ST7735_OutString("  ");
	
	uint32_t lowerBound = (currentDirection * COMPASS_SIZE) / 360;
	
	uint32_t i = lowerBound - 7;
	for(uint8_t offset = 0; offset < 15; offset++){
		ST7735_SetCursor(offset+3, 2);
		ST7735_OutChar3D(label[i]);
		ST7735_SetCursor(offset+3, 3);
		ST7735_OutChar3D(marks[i]);
		i = (i+1)%COMPASS_SIZE;
	}
}

void Clock_UpdateBlankMenu(void){

}

void Clock_UpdateLightMenu(void){

}

