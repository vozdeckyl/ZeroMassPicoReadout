#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/lock_core.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"


int upThreshold = 500;
int downThreshold = 450;

bool wasUp[32] = {0};
int counter[32] = {0};
static mutex_t mutex;

void send_data()
{
    int data[32] = {0};
    uint64_t timeOfLastReading;

    while(true) {
	char input;
	// wait until \n (new line) is sent
	while((input = getchar()) != 0x0D)
	{
	    sleep_ms(1);
	}
	
	mutex_enter_blocking(&mutex);
	for(int n = 0; n<32; n++) {
	    data[n] = counter[n];
	    counter[n] = 0;
	}
	mutex_exit(&mutex);
	int measurementDuration_us = to_us_since_boot(get_absolute_time()) - timeOfLastReading;
	timeOfLastReading = to_us_since_boot(get_absolute_time());
	
	for (int n = 0; n<32; n++) {
	    printf("%f ", 1e6*((float) data[n])/((float) measurementDuration_us));
	}
	printf("\n");
    }
}

int main() {
    stdio_init_all();
    adc_init();
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
    // Select ADC input 0 (GPIO26)
    adc_select_input(0);

    gpio_init(2);
    gpio_init(3);
    gpio_init(4);
    
    gpio_init(6);
    gpio_init(7);
    gpio_init(8);
    gpio_init(9);

    // channel select
    gpio_set_dir(2, true);
    gpio_set_dir(3, true);
    gpio_set_dir(4, true);

    // chip select
    gpio_set_dir(6, true);
    gpio_set_dir(7, true);
    gpio_set_dir(8, true);
    gpio_set_dir(9, true);
    
    mutex_init(&mutex);
    multicore_launch_core1(send_data);
    
    uint16_t signal;
    
    while(true) {

	for(int chipSelect = 0; chipSelect<4; chipSelect++) {
	    int chipSelectOutput = (0b1111 << 6) & ~(0b01 << (chipSelect+6));
	    for(int channel = 0; channel < 8; channel++) {
		// bit-shift by two to output the channel number on pins 2, 3, 4
		gpio_put_all((channel << 2) | chipSelectOutput);

		// wait 1us for the channel to switch
	        sleep_us(1);
		
		signal = adc_read();
		
		if(wasUp[chipSelect*8 + channel]) {
		    // the channel was up last time
		    // have to check if it came back down or not
		    if (signal<downThreshold) {
			wasUp[chipSelect*8 + channel] = false;
		    }
		} else {
		    if (signal>upThreshold) {
			mutex_enter_blocking(&mutex);
			counter[chipSelect*8 + channel]++;
			mutex_exit(&mutex);
			wasUp[chipSelect*8 + channel] = true;
		    }
		}
	    }
	}
    }
        
    return 0;
}
