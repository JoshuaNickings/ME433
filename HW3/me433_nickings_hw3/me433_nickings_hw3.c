#include <stdio.h>
#include "pico/stdlib.h"


int main()
{
    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Start!\n");
 
    while (1) {
        char message[100];
        scanf("%s", message);
        printf("message: %s\r\n",message);
        sleep_ms(50);
    }
}
