#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "ssd1306.h"
#include "font.h"
#include "heartbeat.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5

void drawMessage(int, int, char *);
void drawLetter(int, int, char);

int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    pico_led_init();

    adc_init(); // init the adc module
    adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0
    
    ssd1306_setup();
    ssd1306_clear();
    ssd1306_update();

    while (true) {
        pico_led_blink();

        uint16_t result = adc_read();
        float result_volts = result * 3.3 / 4095;
        
        absolute_time_t t1 = get_absolute_time();
        unsigned int t01 = to_us_since_boot(t1);

        char adc_disp[50];
        sprintf(adc_disp, "The voltage reads %f", result_volts);
        drawMessage(4, 4, adc_disp);
        ssd1306_update();

        absolute_time_t t2 = get_absolute_time();
        unsigned int t02 = to_us_since_boot(t2);

        unsigned int t_diff = t2 - t1; // in units of microseconds !!
        float fps = (float) t_diff / 4.0; //[UNIT CONVERSION NICKINGS CMON!!!!];

        char fps_disp[50];
        sprintf(fps_disp, "Frame Rate: %f",fps);
        drawMessage(9, 20, fps_disp);
        ssd1306_update();
        sleep_ms(1000);
    }
}

void drawMessage(int x, int y, char * m){
    // x and y should be the top left corner of the letter to draw!
    int i = 0;
    while(m[i]!=0){
        drawLetter(x+i*5, y, m[i]);
        i++;
    }
}

void drawLetter(int x, int y, char c){
    int row, col;
    row = c-0x20;
    col = 0;
for (col = 0; col < 5; col++){
    char byte = ASCII[row][col];

    for (int i = 0; i < 8; i++){
        char on_or_off = (byte >> i) & 0b1;

        ssd1306_drawPixel(x + col, y + i, on_or_off);
    }
}    
    
}