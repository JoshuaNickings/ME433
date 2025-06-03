#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO4 (SDA) and GPIO5 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5

// Pico LED functions
void pico_led_init();
void pico_led_blink();

// MCP23008 general functions
void mpc23008_init();
void setPin(unsigned char, unsigned char, unsigned char);
unsigned char readPin(unsigned char, unsigned char);

// Program specific functions
void mcp23008_gp7_blink();
void mcp23008_gp0butt_gp7led();

int main()
{
    stdio_init_all();
   
    /*while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Start!\n");
    */

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c
    
    pico_led_init();
    mpc23008_init();

    while (true) {
        pico_led_blink();
        // mcp23008_gp7_blink();
        mcp23008_gp0butt_gp7led();
    }
    
}

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


void mpc23008_init() {
    uint8_t buf[2];

    buf[0] = 0;
    buf[1] = 0b01111111;

    i2c_write_blocking(i2c_default, 0b0100000, buf, 2, false);
    // printf("MPC23008 initialization done!\n");
}

void setPin(unsigned char address, unsigned char reg, unsigned char value) {
    // for I2C Writing
    uint8_t buf[2];

    buf[0] = reg;
    buf[1] = value;
    i2c_write_blocking(i2c_default, address, buf, 2, false);
    printf("Pin set!\n");
}

unsigned char readPin(unsigned char address, unsigned char reg) {
    // for I2C Reading
    unsigned char buf;

    i2c_write_blocking(i2c_default, address, &reg, 1, true);
    i2c_read_blocking(i2c_default, address, &buf, 1, false);
    // printf("Pin read!\n");
    return buf;
}


void mcp23008_gp7_blink() {
    setPin(0b0100000, 10, 0b10000000);
    sleep_ms(250);
    setPin(0b0100000, 10, 0);
    // printf("GP7 shoulda just blinked!\n");
}

void mcp23008_gp0butt_gp7led() {
    //MCP23008 controlled GP0 button determining status of GP7 LED
    
    unsigned char val = readPin(0b0100000, 9) & 1;
    val = !val;
    val = val << 7;

    // printf("%d\n", val);

    setPin(0b0100000, 10, val);
}

// TO SEND DATA
// i2c_write_blocking(i2c_default, ADDR, buf, 2, false);

// TO READ DATA
// i2c_write_blocking(i2c_default, ADDR, &reg, 1, true);  // true to keep master control of bus
// i2c_read_blocking(i2c_default, ADDR, &buf, 1, false);  // false - finished with bus

