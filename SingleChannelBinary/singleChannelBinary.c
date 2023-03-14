#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/lock_core.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

int counter = 0;
static mutex_t mutex;
uint inputPin = 16;

void send_data()
{
    int data = 0;
    uint64_t timeOfLastReading;

    while(true) {

	char input;
	// wait until \r (return) is sent
	while((input = getchar()) != 0x0D)
	{
	    sleep_ms(1);
	}
	
	mutex_enter_blocking(&mutex);
	data = counter;
	counter = 0;
	mutex_exit(&mutex);
	int measurementDuration_us = to_us_since_boot(get_absolute_time()) - timeOfLastReading; 
	printf("%f \n",1e6*((float) data)/((float) measurementDuration_us));
	timeOfLastReading = to_us_since_boot(get_absolute_time());
    }
}

void capture()
{
	while (true) {
	
		if(gpio_get(inputPin)) {
			
			mutex_enter_blocking(&mutex);
			counter++;
			mutex_exit(&mutex);
			
			/*
			int leaveCounter;
			while(true) {
				
				// wait until the signal comes back down
				// three readings low cout as signal being back down
				
				if(gpio_get(inputPin))
				{
					leaveCounter=0;
				}
				else
				{
					leaveCounter++;
				}
				
				if(leaveCounter>3)
				{
					break;
				}
			}
			*/
			
			while(gpio_get(inputPin)){}
		}
    }
}

int main() {
    stdio_init_all();
    
    gpio_init(inputPin);
    gpio_set_input_enabled(inputPin, true);
    
    mutex_init(&mutex);
    multicore_launch_core1(capture);

	send_data();
	
    return 0;
}
