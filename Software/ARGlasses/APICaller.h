// filename ******** APICaller.h ************** 
// Lisong Sun, Kerry Tu, Dakota Britton, Casey Kim
// Created: October 26, 2021
// H file with functions to make API calls with the ESP8266.
// Lab 11
// TA: Matthew Yu
// Last edited: November 25, 2021

#include <stdlib.h>

//Initializes the API caller
void APICaller_Init(void);

//returns a char* with the weather status
//possible weather: Clear, Clouds, Snow, Rain, Drizzle, Thunderstorm
char* API_Get_WeatherStatus(void);
	
//returns a char* with the temperature
char* API_Get_Temperature(void);

//returns a char* with the humidity
char* API_Get_Humidity(void);

//returns a char* with the date
char* API_Get_Date(void);

//returns the day of the week
char* API_Get_DoW(void);

//returns a char* with the time
char* API_Get_Time(void);
