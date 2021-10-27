// filename ******** Clock.c ************** 
// Lisong Sun and Kerry Tu
// Created: September 16, 2021
// C file with LCD functions used for the clock display and menus
// Lab 4
// TA: Hassan Iqbal
// Last edited: September 20, 2021

#include "Clock.h" 
#include <math.h>

#define CLOCK_X 64
#define CLOCK_Y 50
#define PI 3.14
#define CLOCK_FLASH_SPEED 50
#define ALARM_FLASH_SPEED 1

static double Mtheta;
static double Htheta;

static uint8_t DayFlag;
static uint8_t LastDayFlag;
static uint8_t Paused;

// Time Config globals
static int8_t CurrentAdjustMode = 0; // 0 for hour, 1 for minute, 2 for second
static int32_t FlashingTextCount = CLOCK_FLASH_SPEED;
static int8_t FlashingTextToggle = 0; // 0 for show, 1 for erase

// Alarm Config globals
static int8_t AlarmHour[10];
static int8_t AlarmMinute[10];
static int8_t AlarmStatus[10]; // 0 inactive, 1 active, 2 go off
static int8_t AlarmIndex;
static int8_t AlarmAdjustMode; // 0 for hour, 1 for minute
static int8_t AlarmAdjustStatus; // 0 to scroll, 1 to edit, 2 to enable

// time conversion functions
uint32_t toHour(uint32_t t) {
	return t / 10000;
}
uint32_t toMinute(uint32_t t) {
	return (t%10000) / 100;
}
uint32_t toSecond(uint32_t t) {
	return t%100;
}

// Initialize the main display
void Clock_Init(void) {
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(ST7735_BLACK);
	Timer1_Init();
	hour = 00;
	minute = 00;
	second = 00;
	time = 000000;
	DayFlag = 1;
	LastDayFlag = 1;
	currentMenu = Clock;
	Clock_ShowClock();
}

// Update the clock
void Clock_UpdateClock(void) {
	uint32_t currTime = time;
	//set day/night mode
	if (currTime > 200000 || currTime < 70000) {
		DayFlag = 0;
	} else {
		DayFlag = 1;
	}
	if (DayFlag != LastDayFlag) {
		Clock_ShowClock();
	}
	uint16_t color;
	uint16_t colorInv;
	
	if (!DayFlag) {
		color = ST7735_WHITE;
		colorInv = ST7735_BLACK;
	} else {
		color = ST7735_BLACK;
		colorInv = ST7735_WHITE;
	}
	ST7735_SetTextColor(color);
	ST7735_SetBGColor(colorInv);
	// erase old clock hands
	ST7735_Line(CLOCK_X, CLOCK_Y, CLOCK_X+(int)35*sin(Mtheta), CLOCK_Y-(int)35*cos(Mtheta), colorInv); // minute hand
	ST7735_Line(CLOCK_X, CLOCK_Y, CLOCK_X+(int)20*sin(Htheta), CLOCK_Y-(int)20*cos(Htheta), colorInv); // hour hand
	// draw clock hands
	Mtheta = 2*PI*toMinute(currTime)/60;
	Htheta = 2*PI*(toHour(currTime)%12)/12;
	ST7735_Line(CLOCK_X, CLOCK_Y, CLOCK_X+(int)35*sin(Mtheta), CLOCK_Y-(int)35*cos(Mtheta), color); // minute hand
	ST7735_Line(CLOCK_X, CLOCK_Y, CLOCK_X+(int)20*sin(Htheta), CLOCK_Y-(int)20*cos(Htheta), color); // hour hand
	// write out time
	ST7735_SetCursor(7, 12);
	ST7735_OutUDec2(toHour(currTime));
	ST7735_OutChar(':');
	ST7735_OutUDec2(toMinute(currTime));
	ST7735_OutChar(':');
	ST7735_OutUDec2(toSecond(currTime));
}

// Display the clock menu
void Clock_ShowClock(void) {
	uint32_t currTime = time;
	//set day/night mode
	if (currTime > 200000 || currTime < 70000) {
		DayFlag = 0;
		LastDayFlag = 0;
	} else {
		DayFlag = 1;
		LastDayFlag = 1;
	}
	Clock_Continue();
	uint16_t color;
	uint16_t colorInv;
	if (!DayFlag) {
		color = ST7735_WHITE;
		colorInv = ST7735_BLACK;
	} else {
		color = ST7735_BLACK;
		colorInv = ST7735_WHITE;
	}
	ST7735_FillScreen(colorInv);
	ST7735_SetBGColor(colorInv);
	ST7735_SetTextColor(color);
	// draw the clock template
	for (int i = 0; i < 400; i++) { // draw a circle
		double theta = 2*PI*i/400;
		ST7735_DrawPixel(CLOCK_X+(int)50*sin(theta),CLOCK_Y-(int)50*cos(theta),ST7735_CYAN);
	}
	ST7735_SetCursor(10,0); ST7735_OutUDec(12);
	ST7735_SetCursor(10,9); ST7735_OutUDec(6);
	ST7735_SetCursor(3,5);  ST7735_OutUDec(9);
	ST7735_SetCursor(17,5); ST7735_OutUDec(3);
	ST7735_SetCursor(3,3); ST7735_OutUDec(10);
	ST7735_SetCursor(6,1); ST7735_OutUDec(11);
	ST7735_SetCursor(17,3); ST7735_OutUDec(2);
	ST7735_SetCursor(14,1); ST7735_OutUDec(1);
	ST7735_SetCursor(4,7); ST7735_OutUDec(8);
	ST7735_SetCursor(6,8); ST7735_OutUDec(7);
	ST7735_SetCursor(16,7); ST7735_OutUDec(4);
	ST7735_SetCursor(14,8); ST7735_OutUDec(5);
	// draw clock hands
	Mtheta = 2*PI*toMinute(currTime)/60;
	Htheta = 2*PI*(toHour(currTime)%12)/12;
	ST7735_Line(CLOCK_X, CLOCK_Y, CLOCK_X+(int)35*sin(Mtheta), CLOCK_Y-(int)35*cos(Mtheta), color); // minute hand
	ST7735_Line(CLOCK_X, CLOCK_Y, CLOCK_X+(int)20*sin(Htheta), CLOCK_Y-(int)20*cos(Htheta), color); // hour hand
	// write out time
	ST7735_SetCursor(7, 12);
	ST7735_OutUDec2(toHour(currTime));
	ST7735_OutChar(':');
	ST7735_OutUDec2(toMinute(currTime));
	ST7735_OutChar(':');
	ST7735_OutUDec2(toSecond(currTime));
}

// Update the time config menu
void Clock_UpdateMenu1(void) {
	ST7735_SetCursor(7, 8);
	uint16_t color;
	uint16_t colorInv;
	if (!DayFlag) {
		color = ST7735_WHITE;
		colorInv = ST7735_BLACK;
	} else {
		color = ST7735_BLACK;
		colorInv = ST7735_WHITE;
	}
	ST7735_SetTextColor(color);
	ST7735_SetBGColor(colorInv);
	if(CurrentAdjustMode == 0){ //adjusting hours
		FlashingTextCount--;
		if(FlashingTextCount == 0){
			FlashingTextCount = CLOCK_FLASH_SPEED;
			FlashingTextToggle ^= 1;
		}
		if(FlashingTextToggle == 0){
			ST7735_OutUDec2(hour);
		}
		else{
			ST7735_OutString("  ");
		}
	}
	else{
		ST7735_OutUDec2(hour);
	}
	
	ST7735_OutChar(':');
	
	if(CurrentAdjustMode == 1){ //adjusting minutes
		FlashingTextCount--;
		if(FlashingTextCount == 0){
			FlashingTextCount = CLOCK_FLASH_SPEED;
			FlashingTextToggle ^= 1;
		}
		if(FlashingTextToggle == 0){
			ST7735_OutUDec2(minute);
		}
		else{
			ST7735_OutString("  ");
		}
	}
	else{
		ST7735_OutUDec2(minute);
	}
	
	ST7735_OutChar(':');
	
	if(CurrentAdjustMode == 2){ //adjusting seconds
		FlashingTextCount--;
		if(FlashingTextCount == 0){
			FlashingTextCount = CLOCK_FLASH_SPEED;
			FlashingTextToggle ^= 1;
		}
		if(FlashingTextToggle == 0){
			ST7735_OutUDec2(second);
		}
		else{
			ST7735_OutString("  ");
		}
	}
	else{
		ST7735_OutUDec2(second);
	}
	
	
	// Process button inputs
	uint8_t direction = directionalPress_flag;
	directionalPress_flag = 0;
	if(direction == 1){ //Up
		if(CurrentAdjustMode==0){
			hour++;
			if(hour > 23){
				hour = 0;
			}
		}
		else if(CurrentAdjustMode==1){
			minute++;
			if(minute > 59){
				minute = 0;
			}
		} else {
			second++;
			if (second > 59) {
				second = 0;
			}
		}
	}
	else if(direction == 2){ //Right
		if(CurrentAdjustMode == 2){
			CurrentAdjustMode = 0;
		}
		else{
			CurrentAdjustMode++;
		}
	}
	else if(direction == 3){ //Down
		if(CurrentAdjustMode==0){
			if(hour == 0){
				hour = 23;
			}
			else{
				hour--;
			}
		}
		else if(CurrentAdjustMode==1){
			if(minute == 0){
				minute = 59;
			}
			else{
				minute--;
			}
		} else {
			if (second == 0) {
				second = 59;
			}
			else {
				second--;
			}
		}
	}
	else if(direction == 4){ //Left
		if(CurrentAdjustMode == 0){
			CurrentAdjustMode = 2;
		}
		else{
			CurrentAdjustMode--;
		}
	}
	time = second + 100*minute + 10000*hour; // process potential edits from time config
}

// Display the time config menu
void Clock_ShowMenu1(void) {
	uint32_t currTime = time;
	Clock_Pause();
	CurrentAdjustMode = 0; // 0 for hour, 1 for minute
	FlashingTextCount = CLOCK_FLASH_SPEED;
	FlashingTextToggle = 0;
	uint16_t color;
	uint16_t colorInv;
	if (!DayFlag) {
		color = ST7735_WHITE;
		colorInv = ST7735_BLACK;
	} else {
		color = ST7735_BLACK;
		colorInv = ST7735_WHITE;
	}
	ST7735_FillScreen(colorInv);
	ST7735_SetBGColor(colorInv);
	ST7735_SetTextColor(color);
	ST7735_SetCursor(0,0);
	ST7735_OutString("Use the buttons");
	ST7735_SetCursor(0,1);
	ST7735_OutString("to change the");
	ST7735_SetCursor(0,2);
	ST7735_OutString("current time");
	// write out time
	ST7735_SetCursor(7, 8);
	ST7735_OutUDec2(toHour(currTime));
	ST7735_OutChar(':');
	ST7735_OutUDec2(toMinute(currTime));
	ST7735_OutChar(':');
	ST7735_OutUDec2(toSecond(currTime));

}

// Update the alarm config menu
void Clock_UpdateMenu2(void) {
	uint16_t color;
	uint16_t colorInv;
	if (!DayFlag) {
		color = ST7735_WHITE;
		colorInv = ST7735_BLACK;
	} else {
		color = ST7735_BLACK;
		colorInv = ST7735_WHITE;
	}
	ST7735_SetTextColor(color);
	ST7735_SetBGColor(colorInv);
	
	for(int8_t i=0;i<10;i++){
		ST7735_SetCursor(0, i+4); // title
		
		if(AlarmAdjustStatus == 0 && i == AlarmIndex){ // scrolling through alarms
			FlashingTextCount--;
			if(FlashingTextCount == 0){
				FlashingTextCount = ALARM_FLASH_SPEED;
				FlashingTextToggle ^= 1;
			}
			if(FlashingTextToggle == 0){
				ST7735_OutString("Alarm ");
				ST7735_OutUDec(i);
				ST7735_OutString(":");
			}
			else{
				ST7735_OutString("        ");
			}
		}
		else{
			ST7735_OutString("Alarm ");
			ST7735_OutUDec(i);
			ST7735_OutString(":");
		}
		
		ST7735_SetCursor(10, i+4); // time
		if(AlarmAdjustStatus == 1 && AlarmAdjustMode == 0 && i == AlarmIndex){ // editing hours
			FlashingTextCount--;
			if(FlashingTextCount == 0){
				FlashingTextCount = ALARM_FLASH_SPEED;
				FlashingTextToggle ^= 1;
			}
			if(FlashingTextToggle == 0){
				ST7735_OutUDec2(AlarmHour[i]);
			}
			else{
				ST7735_OutString("  ");
			}
		}
		else{
			ST7735_OutUDec2(AlarmHour[i]);
		}
		ST7735_OutChar(':');
		if(AlarmAdjustStatus == 1 && AlarmAdjustMode == 1 && i == AlarmIndex){ // editing min
			FlashingTextCount--;
			if(FlashingTextCount == 0){
				FlashingTextCount = ALARM_FLASH_SPEED;
				FlashingTextToggle ^= 1;
			}
			if(FlashingTextToggle == 0){
				ST7735_OutUDec2(AlarmMinute[i]);
			}
			else{
				ST7735_OutString("  ");
			}
		}
		else{
			ST7735_OutUDec2(AlarmMinute[i]);
		}
		
		ST7735_SetCursor(17, i+4); // status
		if(AlarmAdjustStatus == 2 && i == AlarmIndex){ // enabling alarms
			FlashingTextCount--;
			if(FlashingTextCount == 0){
				FlashingTextCount = ALARM_FLASH_SPEED;
				FlashingTextToggle ^= 1;
			}
			if(FlashingTextToggle == 0){
				if (!AlarmStatus[i]) {
					ST7735_OutString("OFF");
				} else {
					ST7735_OutString("ON");
				}
			}
			else{
				ST7735_OutString("   ");
			}
		}
		else{
			if (!AlarmStatus[i]) {
				ST7735_OutString("OFF");
			} else {
				ST7735_OutString("ON");
			}
		}
	}
	
	// Process inputs
	uint8_t direction = directionalPress_flag;
	directionalPress_flag = 0;
	if(direction==1){ //Up
		if(AlarmAdjustStatus==0){
			if(AlarmIndex == 0){
				AlarmIndex = 9;
			}
			else{
				AlarmIndex--;
			}
		}
		else if(AlarmAdjustStatus==1){
			if(AlarmAdjustMode==0){
				if(AlarmHour[AlarmIndex]==23){
					AlarmHour[AlarmIndex]=0;
				}
				else{
					AlarmHour[AlarmIndex]++;
				}
			}
			else if(AlarmAdjustMode==1){
				if(AlarmMinute[AlarmIndex]==59){
					AlarmMinute[AlarmIndex]=0;
				}
				else{
					AlarmMinute[AlarmIndex]++;
				}
			}
		}
		else {
			if (AlarmStatus[AlarmIndex] == 0) {
				AlarmStatus[AlarmIndex] = 1;
			} else {
				AlarmStatus[AlarmIndex] = 0;
			}
		}
	}
	else if(direction==2){ //Right
		if(AlarmAdjustStatus==0){
			AlarmAdjustStatus = 1;
			AlarmAdjustMode = 0;
		}
		else if(AlarmAdjustStatus==1){
			if(AlarmAdjustMode==0){
				AlarmAdjustMode = 1;
			}
			else if(AlarmAdjustMode==1){
				AlarmAdjustStatus = 2;
			}
		}
		else {
			AlarmAdjustStatus = 0;
		}
	}
	else if(direction==3){ //Down
		if(AlarmAdjustStatus==0){
			if(AlarmIndex == 9){
				AlarmIndex = 0;
			}
			else{
				AlarmIndex++;
			}
		}
		else if(AlarmAdjustStatus==1){
			if(AlarmAdjustMode==0){
				if(AlarmHour[AlarmIndex]==0){
					AlarmHour[AlarmIndex]=23;
				}
				else{
					AlarmHour[AlarmIndex]--;
				}
			}
			else if(AlarmAdjustMode==1){
				if(AlarmMinute[AlarmIndex]==0){
					AlarmMinute[AlarmIndex]=59;
				}
				else{
					AlarmMinute[AlarmIndex]--;
				}
			}
		}
		else {
			if (AlarmStatus[AlarmIndex] == 0) {
				AlarmStatus[AlarmIndex] = 1;
			} else {
				AlarmStatus[AlarmIndex] = 0;
			}
		}
	}
	else if(direction==4){ //Left
		if(AlarmAdjustStatus==0){
			AlarmAdjustStatus = 2;
		}
		else if(AlarmAdjustStatus==1){
			if(AlarmAdjustMode==1){
				AlarmAdjustMode = 0;
			}
			else if(AlarmAdjustMode==0){
				AlarmAdjustStatus = 0;
			}
		}
		else {
			AlarmAdjustStatus = 1;
			AlarmAdjustMode = 1;
		}
	}
}

// Display the alarm config menu
void Clock_ShowMenu2(void) {
	uint16_t color;
	uint16_t colorInv;
	if (!DayFlag) {
		color = ST7735_WHITE;
		colorInv = ST7735_BLACK;
	} else {
		color = ST7735_BLACK;
		colorInv = ST7735_WHITE;
	}
	AlarmAdjustStatus = 0;
	AlarmAdjustMode = 0; // 0 for hour, 1 for minute
	AlarmIndex = 0;
	FlashingTextCount = ALARM_FLASH_SPEED;
	FlashingTextToggle = 0;
	ST7735_FillScreen(colorInv);
	ST7735_SetBGColor(colorInv);
	ST7735_SetTextColor(color);
	ST7735_SetCursor(0,0);
	ST7735_OutString("Use the buttons");
	ST7735_SetCursor(0,1);
	ST7735_OutString("to change your");
	ST7735_SetCursor(0,2);
	ST7735_OutString("alarm");
	for(int8_t i=0;i<10;i++){
		ST7735_SetCursor(0, i+4); // title
		ST7735_OutString("Alarm ");
		ST7735_OutUDec(i);
		ST7735_OutString(":");
		ST7735_SetCursor(10, i+4); // time
		ST7735_OutUDec2(AlarmHour[i]);
		ST7735_OutChar(':');
		ST7735_OutUDec2(AlarmMinute[i]);
		ST7735_SetCursor(17, i+4); // status
		if (!AlarmStatus[i]) {
			ST7735_OutString("OFF");
		} else {
			ST7735_OutString("ON");
		}
	}
}

// Display the alarm activation menu
void Clock_ShowMenu3(void) {
	uint16_t color;
	uint16_t colorInv;
	if (!DayFlag) {
		color = ST7735_WHITE;
		colorInv = ST7735_BLACK;
	} else {
		color = ST7735_BLACK;
		colorInv = ST7735_WHITE;
	}
	ST7735_FillScreen(colorInv);
	ST7735_SetBGColor(colorInv);
	ST7735_SetTextColor(color);
	ST7735_SetCursor(4,7);
	ST7735_OutString("GOOD MORNING!");
	ST7735_SetTextColor(ST7735_RED);
	ST7735_SetCursor(0, 13);
	ST7735_OutString("SNOOZE (BACK)");
	ST7735_SetCursor(0, 14);
	ST7735_OutString("OFF (MODE)");
}

// Stop clock timer
void Clock_Pause(void) {
	//NVIC_DIS0_R = 1<<21;              // disable interrupt 19 in NVIC
	Paused = 1;
}

// enable clock timer
void Clock_Continue(void) {
	//NVIC_EN0_R = 1<<21;              // enable interrupt 19 in NVIC
	Paused = 0;
}

void Clock_AlarmShutoff(void){
	for(int8_t i=0;i<10;i++){
		if(AlarmStatus[i]==2){
			AlarmStatus[i] = 0;
		}
	}
}

void Clock_AlarmSnooze(void){
	for(int8_t i=0;i<10;i++){
		if(AlarmStatus[i]==2){
			AlarmStatus[i] = 1;
			AlarmMinute[i] = minute;
			AlarmHour[i] = hour;
			for(int8_t x=0;x<10;x++){
				if (++AlarmMinute[i] == 60) {
					AlarmMinute[i] = 0;
					if (++AlarmHour[i] == 24) {
						AlarmHour[i] = 0;
					}
				}
			}
		}
		
	}
}

// Initialize the clock timer to 1Hz
void Timer1_Init(void) {
	volatile uint32_t delay;
  DisableInterrupts();
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x02;      // activate timer1
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER1_CTL_R &= ~TIMER_CTL_TAEN; // disable timer1A during setup
  TIMER1_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer1A initialization ****
                                   // configure for periodic mode
  TIMER1_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  TIMER1_TAILR_R = 79999999;         // start value for 1Hz interrupts
  TIMER1_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// clear timer1A timeout flag
  TIMER1_CTL_R |= TIMER_CTL_TAEN;  // enable timer1A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer0A=priority 1
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF0FFF)|0x00002000; // bits 15-13
  NVIC_EN0_R = 1<<21;              // disable interrupt 21 in NVIC
}

// Tick up the current time
// TODO: auto shutoff
void Timer1A_Handler(void) {
	TIMER1_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer1A timeout
	PF2 ^= 0x04; // Heartbeat
	
	if (!Paused) {
		if (++second == 60) {
			second = 0;
			if (++minute == 60) {
				minute = 0;
				if (++hour == 24) {
					hour = 0;
				}
			}
			for(int8_t i=0;i<10;i++){
				if(AlarmStatus[i]==1&&AlarmHour[i]==hour&&AlarmMinute[i]==minute){
					currentMenu = AlarmActivate;
					AlarmStatus[i] = 2;
				}
			}
		}
	}
	
	if (timeout_flag && (currentMenu == TimeConfig || currentMenu == AlarmConfig)) {
		timeout--;
	}
	
	
	time = second + 100*minute + 10000*hour; 

}
