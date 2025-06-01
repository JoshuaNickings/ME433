#include <stdio.h>
#include "pico/stdlib.h"


int main()
{
    stdio_init_all();

    gpio_init(16);
    gpio_init(17); // initialize both pins

    gpio_set_dir(16, GPIO_IN); //button input goes into gp16
    gpio_set_dir(17, GPIO_OUT); //gp17 becomes the output

    while (true) {
        if(gpio_get(16) == 1) {
            gpio_put(17, 1);
        } else {
            gpio_put(17, 0);
        }
    }
}
