/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "hardware/adc.h"

#define FLAG_VALUE 123
#define PIN_NUM 15

volatile float voltage; 

void core1_entry() {
    // CORE 1: PIN CORE
    // This core will handle all the interactions with the pins and external components

    gpio_init(PIN_NUM); // PIN_NUM without the GP
    gpio_set_dir(PIN_NUM, GPIO_OUT);

    adc_init(); // init the adc module
    adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0

    /*
    multicore_fifo_push_blocking(FLAG_VALUE);

    uint32_t g = multicore_fifo_pop_blocking();

    if (g != FLAG_VALUE)
        printf("Hmm, that's not right on core 1!\n");
    else
        printf("Its all gone well on core 1!");
    */

    while (1) {
        uint32_t g = multicore_fifo_pop_blocking();
        // printf("Core 1 Received %d From Core 0.\n\n", g);

        if (g == 0) {
            uint16_t voltage_int = adc_read();
            voltage = (voltage_int / 4095.0) * 3.3;
            multicore_fifo_push_blocking(0);
        } else if (g == 1) {
            gpio_put(PIN_NUM, 1);
            multicore_fifo_push_blocking(1);
        } else if (g == 2) {
            gpio_put(PIN_NUM, 0);
            multicore_fifo_push_blocking(2);
        } else {
            multicore_fifo_push_blocking(3);
        }
    }

}

int main() {
    // CORE 0: COMMS CORE
    // This core will handle all the printing and scanning for user input
    stdio_init_all();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    
    printf("Hello, multicore!\n");

    /// \tag::setup_multicore[]

    multicore_launch_core1(core1_entry);

    // Wait for it to start up

    while (1) {
        printf("-----MENU-----\nType 0: Analog Voltage Reading\nType 1: Turn On LED%d\nType 2: Turn Off LED%d\n\n", PIN_NUM, PIN_NUM);
        
        uint32_t h;
        scanf("%d", &h);
        printf("\nYou entered: %d\n\n", h);

        multicore_fifo_push_blocking(h);

        uint32_t f = multicore_fifo_pop_blocking();
        // printf("Core 0 Received %d From Core 1.\n\n", f);

        if (f == 0) {
            printf("Analog Voltage Reads %f Volts.", voltage);
        } else if (f == 1) {
            printf("LED%d Turned On.", PIN_NUM);
        } else if (f == 2) {
            printf("LED%d Turned Off.", PIN_NUM);
        } else {
            printf("Invalid Input.");
        }
        printf("\n\n\n");
    }


    // uint32_t g = multicore_fifo_pop_blocking();

    // if (g != FLAG_VALUE)
    //     printf("Hmm, that's not right on core 0!\n");
    // else {
    //     multicore_fifo_push_blocking(FLAG_VALUE);
    //     printf("It's all gone well on core 0!");
    // }

    /// \end::setup_multicore[]
}
