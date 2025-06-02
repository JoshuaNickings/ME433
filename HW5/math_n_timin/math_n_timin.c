#include <stdio.h>
#include "pico/stdlib.h"


int main()
{
    stdio_init_all();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Start!\n");

    volatile float f1, f2;
    printf("Enter two floats to use:");
    scanf("%f %f", &f1, &f2);
    volatile float f_add, f_sub, f_mult, f_div;
    
    absolute_time_t t1 = get_absolute_time();
    for (int a = 0; a < 1000; a++) {
        f_add = f1+f2;
    }
    uint64_t t01 = to_us_since_boot(t1);
    t1 = get_absolute_time();
    t01 = to_us_since_boot(t1) - t01;
    t01 = t01 * 150 / 1000;

    absolute_time_t t2 = get_absolute_time();
    for (int b = 0; b < 1000; b++) {
        f_sub = f1-f2;
    }
    uint64_t t02 = to_us_since_boot(t2);
    t2 = get_absolute_time();
    t02 = to_us_since_boot(t2) - t02;
    t02 = t02 * 150 / 1000;

    absolute_time_t t3 = get_absolute_time();
    for (int c = 0; c < 1000; c++) {
        f_mult = f1*f2;
    }
    uint64_t t03 = to_us_since_boot(t3);
    t3 = get_absolute_time();
    t03 = to_us_since_boot(t3) - t03;
    t03 = t03 * 150 / 1000;

    absolute_time_t t4 = get_absolute_time();
    for (int d = 0; d < 1000; d++) {
        f_div = f1/f2;
    }
    uint64_t t04 = to_us_since_boot(t4);
    t4 = get_absolute_time();
    t04 = to_us_since_boot(t4) - t04;
    t04 = t04 * 150 / 1000;
    
    printf("\nResults: \n%f+%f=%f \n%f-%f=%f \n%f*%f=%f \n%f/%f=%f\n", f1,f2,f_add, f1,f2,f_sub, f1,f2,f_mult, f1,f2,f_div);
    printf("\nTime Results:\n\nAddition:\nClock Cycles: %llu\n", t01);
    printf("\nSubtraction:\nClock Cycles: %llu\n", t02);
    printf("\nMultiplication:\nClock Cycles: %llu\n", t03);
    printf("\nDivision:\nClock Cycles: %llu\n", t04);
}
