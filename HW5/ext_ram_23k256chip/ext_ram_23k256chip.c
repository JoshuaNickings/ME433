#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <stdlib.h>
#include "math.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT        spi0
#define PIN_MISO        16 // DI
#define PIN_CS_DAC      17 // DAC Chip Select
#define PIN_SCK         18
#define PIN_MOSI        19 // DO
#define PIN_CS_EXTRAM   20 // 23K256 Chip Select

union FloatInt {
    float f;
    uint32_t i;
};

void spi_ram_init();
void spi_ram_write(uint16_t, float);
float spi_ram_read(uint16_t);
void writeDac(int, float);

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

int main()
{   
    stdio_init_all();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS_DAC,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_CS_EXTRAM,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS_DAC, GPIO_OUT);
    gpio_put(PIN_CS_DAC, 1);

    gpio_set_dir(PIN_CS_EXTRAM, GPIO_OUT);
    gpio_put(PIN_CS_EXTRAM, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    spi_ram_init();

    for (int i = 0; i < 1000; i++) {
        float voltage = (3.3/2) * sin(i * M_PI / 500) + (3.3/2);
        spi_ram_write(i, voltage);
        float check = spi_ram_read(i);
        //printf("Wrote %f, Read %f\n", voltage, check);
    }

    while (true) {
        for (uint16_t t = 0; t < 1000; t++) {
            float val = spi_ram_read(t);
            writeDac(0, val);
            sleep_ms(1);
        }
    }
}

void spi_ram_init() {
    uint8_t op_mode[2];
    op_mode[0] = 0b00000001;
    op_mode[1] = 0b01000000;
    cs_select(PIN_CS_EXTRAM);
    spi_write_blocking(SPI_PORT, op_mode, 2); // where data is a uint8_t array with length len
    cs_deselect(PIN_CS_EXTRAM);
}

void spi_ram_write(uint16_t addr, float v) {
    
    uint8_t info[7];
    info[0] = 0b10;

    info[1] = (addr >> 8) & 0xFF;
    info[2] = (addr) & 0xFF;

    union FloatInt num;
    num.f = v;

    info[3] = (num.i >> 24) & 0xFF;
    info[4] = (num.i >> 16) & 0xFF;
    info[5] = (num.i >> 8) & 0xFF;
    info[6] = (num.i) & 0xFF;

    cs_select(PIN_CS_EXTRAM);
    spi_write_blocking(SPI_PORT, info, 7);
    cs_deselect(PIN_CS_EXTRAM);
}

float spi_ram_read(uint16_t addr) {
    uint8_t write[7], read[7];
    write[0] = 0b11;

    write[1] = (addr >> 8) & 0xFF;
    write[2] = (addr) & 0xFF;

    write[3] = 0;
    write[4] = 0;
    write[5] = 0;
    write[6] = 0;

    cs_select(PIN_CS_EXTRAM);
    spi_write_read_blocking(SPI_PORT, write, read, 7);
    cs_deselect(PIN_CS_EXTRAM);

    read[0] = 0;

    read[1] = 0;
    read[2] = 0;

    union FloatInt num;
    num.i = num.i | (read[3] << 24);
    num.i = num.i | (read[4] << 16);
    num.i = num.i | (read[5] << 8);
    num.i = num.i | read[6];

    //printf("%f \t", num.f);

    return num.f;
}

void writeDac(int channel, float voltage) {
    uint8_t data[2];
    int len = 2;
    
    int conv_volt = ((voltage / 3.3) * 1023.0);
    int data_placeholder = (conv_volt & 0x3FF) << 2;
    data_placeholder = (channel << 15) | data_placeholder;
    data_placeholder = (0b111 << 12) | data_placeholder; // simpler method used when debugging writeDac section with AI
    // asked chatgpt to identify potential causes of error when accessing float values

    data[0] = (data_placeholder >> 8) & 0xFF;
    data[1] = data_placeholder & 0xFF;

    cs_select(PIN_CS_DAC);
    spi_write_blocking(SPI_PORT, data, len); // where data is a uint8_t array with length len
    cs_deselect(PIN_CS_DAC);
}