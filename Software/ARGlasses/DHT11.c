#include "DHT11.h"


static uint8_t data[5]; // 40 bit data transmission; RH integral, RH decimal, T integral, T decimal, Check; check = sum(data[0:3])
uint32_t temperature;
uint32_t humidity;

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
}

// return temperature
void DHT11_Read(void) {
	if (readSensor() != DHT11_READ_SUCCESS) { // if read was failure
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
// return 0 if timeout (timeout or too little time since last reading)
// return 1 if too little time since last reading
// return 2 if checksum failure
uint8_t readSensor(void) {
	//check if two seconds since last reading
	
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

