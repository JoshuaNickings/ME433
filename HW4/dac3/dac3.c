#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <stdlib.h>
#include "math.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16 //SPI DI Pin for the Pico
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19 //SPI DO Pin for the Pico

static inline void cs_select(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop"); // FIXME
}

static inline void cs_deselect(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop"); // FIXME
}

void writeDac(int, double);

int main()
{
    stdio_init_all();

    // SPI initialisation.
    spi_init(SPI_PORT, 1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    while (true) {
        for (int t = 0; t < 50; t++) { // When making the time period 100, the math doubled the period of each graph. Unsure why! When halved to 50, it got to the desired outcome.
            sleep_ms(10);
            double volt_a = (3.3/2) * sin(t * M_PI / 12.5) + (3.3/2);
            printf("sinvolt %f \n", volt_a);
            writeDac(0, volt_a);

            double volt_b = abs((25 - t)) * 3.3/25;
            printf("trivolt %f \n", volt_b);
            writeDac(1, volt_b);
        }
    }
}

void writeDac(int channel, double voltage) {
    uint8_t data[2];
    int len = 2;
    
    int conv_volt = (voltage / 3.3) * 1024;
    printf("convolt %d \n", conv_volt);
    int conv_volt0 = conv_volt >> 6;
    printf("convolt0 %d \n", conv_volt0);
    int conv_volt1 = (conv_volt & 0b0000111111) << 2;
    printf("convolt1 %d \n\n", conv_volt1);

    data[0] = (channel << 7) | (0b01110000 | conv_volt0); 
    data[1] = conv_volt1;
    cs_select(PIN_CS);
    spi_write_blocking(SPI_PORT, data, len); // where data is a uint8_t array with length len
    cs_deselect(PIN_CS);
}
