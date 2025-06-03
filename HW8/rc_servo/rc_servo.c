#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define LEDPin FIXITBOY!!
int main()
{
    stdio_init_all();

    gpio_set_function(LEDPin, GPIO_FUNC_PWM); // Set the LED Pin to be PWM
    uint slice_num = pwm_gpio_to_slice_num(LEDPin); // Get PWM slice number
    float div = 50; // must be between 1-255
    pwm_set_clkdiv(slice_num, div); // divider
    uint16_t wrap = 60000; // when to rollover, must be less than 65535
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true); // turn on the PWM

    while (true) {
        pwm_set_gpio_level(LEDPin, wrap / 2);

        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}

void init_pwm(int pin, float d, uint16_t w){
    gpio_set_function(pin, GPIO_FUNC_PWM); // Set the LED Pin to be PWM
    uint slice_num = pwm_gpio_to_slice_num(pin); // Get PWM slice number
    float div = d; // must be between 1-255
    pwm_set_clkdiv(slice_num, div); // divider
    uint16_t wrap = w; // when to rollover, must be less than 65535
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true); // turn on the PWM
}