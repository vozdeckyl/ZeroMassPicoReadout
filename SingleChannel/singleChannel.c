#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/lock_core.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"


int upThreshold = 500;
int downThreshold = 450;


int counter = 0;
int counter2 = 0;
static mutex_t mutex;

void send_data()
{
    int data = 0;
    uint64_t timeOfLastReading;

    while(true) {

	char input;
	// wait until \n (new line) is sent
	while((input = getchar()) != 0x0D)
	{
	    sleep_ms(1);
	}
	
	mutex_enter_blocking(&mutex);
	data = counter;
	counter = 0;
	mutex_exit(&mutex);
	int measurementDuration_us = to_us_since_boot(get_absolute_time()) - timeOfLastReading; 
	printf("%d %d %f \n", data, measurementDuration_us, 1e6*((float) data)/((float) measurementDuration_us));
	timeOfLastReading = to_us_since_boot(get_absolute_time());
    }
}

int main() {
    stdio_init_all();
    adc_init();
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
    // Select ADC input 0 (GPIO26)
    adc_select_input(0);
    
    mutex_init(&mutex);
    multicore_launch_core1(send_data);

    uint16_t signal;
    
    while (true) {

        signal = adc_read();

	if (signal>upThreshold) {

	    mutex_enter_blocking(&mutex);
	    counter++;
	    mutex_exit(&mutex);
	    
	    while (signal>downThreshold) {
		// wait for the signal to come down
		signal = adc_read();
	    }
	}
	
    }
    return 0;
}
