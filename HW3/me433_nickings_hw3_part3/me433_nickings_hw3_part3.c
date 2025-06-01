#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"


int main()
{
    stdio_init_all();

    gpio_init(16);
    gpio_init(17); // initialize both pins

    gpio_set_dir(16, GPIO_IN); //button input goes into gp16
    gpio_set_dir(17, GPIO_OUT); //gp17 becomes the output

    adc_init(); // init the adc module
    adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0

    while (true) {
        if(gpio_get(16) == 1) {
            uint16_t result = adc_read();
            printf("%d\n", result);
            sleep_ms(250);
        }
    }
}
