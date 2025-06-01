#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

int main()
{
    stdio_init_all();

    gpio_init(16);
    gpio_init(17);

    gpio_set_dir(16, GPIO_IN);
    gpio_set_dir(17, GPIO_OUT);

    adc_init(); // init the adc module
    adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    gpio_put(17, 1);

    while (gpio_get(16) == 0) {
        sleep_ms(100);
    }

    gpio_put(17, 0);

    while(true) {
        int message;
        printf("Enter the number of analog samples to take. (Between 1 and 100)\n");
        scanf("%d", &message);
        if (message < 1) {
            message = 1;
        } else if (message > 100) {
            message = 100;
        }
        printf("%d", message);

        for (int i = 0; i < message; i++) {
            uint16_t result = adc_read();
            float result_volts = result * 3.3 / 4095;
            printf("%f\n", result_volts);
            sleep_ms(10);
        }  
    }

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
