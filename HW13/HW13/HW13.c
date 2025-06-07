#include "hw13_helper.h"

// I2C defines
// This example will use I2C0 on GPIO4 (SDA) and GPIO5 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5



int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);

    gpio_init(PICO2LED); // PIN_NUM without the GP
    gpio_set_dir(PICO2LED, GPIO_OUT);
    gpio_put(PICO2LED, 0);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    init_mpu6050();

    float mpu6050_data[7];

    while (true) {
        // printf("Hello, world!\n");
        // sleep_ms(1000);

        read_mpu6050(mpu6050_data);
        printf("X Acceleration: %f\nY Acceleration: %f\nZ Acceleration: %f\n", mpu6050_data[2], mpu6050_data[1], mpu6050_data[0]); // Due to the orietation of the chip, x and z values must swap
        printf("X Gyro: %f\nY Gyro: %f\nZ Gyro: %f\n", mpu6050_data[4], mpu6050_data[5], mpu6050_data[6]);
        printf("Temp: %f\n\n", mpu6050_data[3]);
        sleep_ms(10);
        
        /* // Debugging code to test chip address starts here

        debug_chipaddr_mpu6050()

        */ // Debugging code to test chip address ends here


    }
}
