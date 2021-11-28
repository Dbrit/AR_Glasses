// filename ******** DHT11.c ************** 
// Lisong Sun, Kerry Tu, Dakota Britton, Casey Kim
// Created: October 26, 2021
// C file with functions to communicate with the DHT11 temperature sensor.
// Lab 11
// TA: Matthew Yu
// Last edited: November 25, 2021

#include "DHT11.h"


static uint8_t data[5]; // 40 bit data transmission; RH integral, RH decimal, T integral, T decimal, Check; check = sum(data[0:3])
static uint32_t temperature;
static uint32_t humidity;
static uint32_t LastReadTime;

void Timer1_Init(void);  // timer for tracking time between reads
uint8_t readSensor(void);

// Initialize DHT11 sensor on PC4
void DHT11_Init(void) {
	SYSCTL_RCGCGPIO_R     |=  0x04;         // Activate clock for Port C
  while((SYSCTL_PRGPIO_R & 0x04) != 0x04){};  // Allow time for clock to start
   
  GPIO_PORTC_PCTL_R     &= ~0x000F0000;   // regular GPIO
  GPIO_PORTC_AMSEL_R    &= ~0x10;         // disable analog function 
  GPIO_PORTC_DIR_R      &= ~0x10;         // inputs on PC4
  GPIO_PORTC_AFSEL_R    &= ~0x10;         // regular port function
  GPIO_PORTC_PUR_R      |=  0x10;         // enable pull-up on PC4	
  GPIO_PORTC_DEN_R      |=  0x10;         // enable digital port 
		
	SysTick_Init();
	Timer1_Init();
	LastReadTime = TIMER1_TAR_R;
	temperature = 9999;
	humidity = 9999;
}

// return temp
uint32_t DHT11_GetTemp(void) {
	DHT11_Read();
	return ((temperature*9)/5) + 3200;
}

// return humidity
uint32_t DHT11_GetHumidity(void) {
	DHT11_Read();
	return humidity;
}

// return temperature
void DHT11_Read(void) {
	uint8_t status = readSensor();
	if (status == DHT11_READ_WAIT) { // too little time since last read
		return;
	} else if (status != DHT11_READ_SUCCESS) { // if read was failure
		temperature = 0;
		humidity = 0;
	} else {
		//read temp
		temperature = data[2] * 100;
		if (data[3] & 0x80) {
      temperature -= (data[3] & 0x0f)*10;
		} else {
			temperature += (data[3] & 0x0f)*10;
		}
		
		humidity = data[0] * 100;
		if (data[1] & 0x80) {
      humidity -= (data[1] & 0x0f)*10;
		} else {
			humidity += (data[1] & 0x0f)*10;
		}
	}
}


// fill data[] with info
// can only be called every two seconds
// return 3 if success
// return 0 if timeout
// return 1 if too little time since last reading
// return 2 if checksum failure
uint8_t readSensor(void) {
	//check if two seconds since last reading
	uint32_t currentTime = TIMER1_TAR_R;
	uint32_t timeStat = currentTime < LastReadTime? (LastReadTime - currentTime):(LastReadTime+(0xFFFFFFFF-currentTime));
//	ST7735_SetCursor(7,8);
//	ST7735_OutUDec(timeStat);
	if (timeStat < 160000000) { //less than 2 sec
		return DHT11_READ_WAIT;
	}
	LastReadTime = currentTime;
	// clear buffer
	data[0] = data[1] = data[2] = data[3] = data[4] = 0;
	// init vars for data collection
	uint8_t mask = 0x80;
  uint8_t idx = 0;
	
	//set PC4 output
	GPIO_PORTC_DIR_R |= 0x10;
	
	//send start (low 18ms, high 40us)
	PC4 = 0x00;
	SysTick80_Wait10ms(2); // data sheet says at least 18ms, 20ms just to be safe
	PC4 = 0x10;
	GPIO_PORTC_DIR_R &= ~0x10; // set PC4 input and pull up
	NVIC_ST_RELOAD_R = NVIC_ST_RELOAD_M; // set Systick max reload value for stopwatch
	
	DisableInterrupts(); // begin time critical section
	//read acknowledge
	uint32_t start = NVIC_ST_CURRENT_R;
	while ((GPIO_PORTC_DATA_R & 0x10) == 0)  // T-rel
	{
			if (start - NVIC_ST_CURRENT_R > DHT11_TIMEOUT) return DHT11_READ_TIMEOUT;
	}

	start = NVIC_ST_CURRENT_R;
	while ((GPIO_PORTC_DATA_R & 0x10) != 0)  // T-reh
	{
			if (start - NVIC_ST_CURRENT_R > DHT11_TIMEOUT) return DHT11_READ_TIMEOUT;
	}
	
	// ST7735_OutString("Acknowledged!");
	
	// read pre data bit
	start = NVIC_ST_CURRENT_R;
	while ((GPIO_PORTC_DATA_R & 0x10) == 0)  // T-rel
	{
			if (start - NVIC_ST_CURRENT_R > DHT11_TIMEOUT) return DHT11_READ_TIMEOUT;
	}

	start = NVIC_ST_CURRENT_R;
	while ((GPIO_PORTC_DATA_R & 0x10) != 0)  // T-reh
	{
			if (start - NVIC_ST_CURRENT_R > DHT11_TIMEOUT) return DHT11_READ_TIMEOUT;
	}
	
	//read buffer
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
	
	for (uint8_t i = 0; i < 40; i++) {
		start = NVIC_ST_CURRENT_R;
		while ((GPIO_PORTC_DATA_R & 0x10) == 0)
		{
			if (start - NVIC_ST_CURRENT_R > DHT11_TIMEOUT) return DHT11_READ_TIMEOUT;
		}
		
		uint32_t t = NVIC_ST_CURRENT_R;

		start = NVIC_ST_CURRENT_R;
		while ((GPIO_PORTC_DATA_R & 0x10) != 0)
		{
			if (start - NVIC_ST_CURRENT_R > DHT11_TIMEOUT) return DHT11_READ_TIMEOUT;
		}

		if ((t - NVIC_ST_CURRENT_R) > 3200) // data bit is 1
		{ 
				data[idx] |= mask;
		}
		mask >>= 1;
		if (mask == 0)   // next byte?
		{
				mask = 0x80;
				idx++;
		}
	}
	
	EnableInterrupts();
	//set PC4 output, high
	GPIO_PORTC_DIR_R |= 0x10;
	GPIO_PORTC_DATA_R |= 0x10;
	
	//checkSum
	if (data[4] != ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
		return DHT11_READ_CHECKFAIL;
	}
	
	return DHT11_READ_SUCCESS;
}

void Timer1_Init(void){ volatile uint32_t delay;
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
  delay = SYSCTL_RCGCTIMER_R;   // allow time to finish activating
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, down-count 
  TIMER1_TAILR_R = 0xFFFFFFFF;  // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A
}

