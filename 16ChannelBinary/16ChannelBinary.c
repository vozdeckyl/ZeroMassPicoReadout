#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/lock_core.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

int counter[16] = {0};
static mutex_t mutex;

void send_data()
{
    int data[16] = {0};
    uint64_t timeOfLastReading;

    while(true) {
		
		char input;
		// wait until \r (return) is sent
		while((input = getchar()) != 0x0D)
		{
			sleep_ms(1);
		}

		int measurementDuration_us = to_us_since_boot(get_absolute_time()) - timeOfLastReading;
		mutex_enter_blocking(&mutex);
		for(int i=0; i<16; i++)
		{
			data[i] = counter[i];
			counter[i] = 0;
		}
		mutex_exit(&mutex);
		timeOfLastReading = to_us_since_boot(get_absolute_time());
		
		for(int i=0;i<16;i++)
		{
			printf("%f ", 1e6*((float) data[i])/((float) measurementDuration_us));
		}
		printf("\n");
    }
}

void capture()
{
	uint32_t oldReadings = 0;
	uint32_t newReadings = 0;
	uint32_t risingEdges = 0;
	
	while(true)
	{
		oldReadings = newReadings;
	    newReadings = 0b1111111111111111 & (gpio_get_all() >> 2);

		// this bitwise operation detects rising edges
		// it's 1 if and only if the oldReading was 0 and newReading is 1 
		risingEdges = (~oldReadings) & newReadings;
		
		if(risingEdges != 0)
		{
			mutex_enter_blocking(&mutex);
			for(int i=0; i<16; i++)
			{
				if(((risingEdges>>i) & 0b1) == 1)
				{
					counter[i]++;
				}
			}
			mutex_exit(&mutex);
		}
    }
}

int main() {
    stdio_init_all();

	// 16 input pins (2,3...17)
	for(int inputPin = 2; inputPin<18; inputPin++)
	{
		gpio_init(inputPin);
		gpio_set_input_enabled(inputPin, true);
    }
	
    mutex_init(&mutex);
    multicore_launch_core1(capture);

	send_data();
	
    return 0;
}
