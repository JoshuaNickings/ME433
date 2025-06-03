#include <stdio.h>
#include "pico/stdlib.h"
#include "heartbeat.h"

void pico_led_init() {
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    printf("LED Initialization done! \n");
}

void pico_led_blink() {
    gpio_put(PICO_DEFAULT_LED_PIN, 1);
    sleep_ms(100);
    gpio_put(PICO_DEFAULT_LED_PIN, 0);
    // printf("LED Blink! \n");
}
