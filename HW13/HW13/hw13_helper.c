#include "hw13_helper.h"

void debug_chipaddr_mpu6050() {
    uint8_t address = CHIPADDR; // Address of the chip
    uint8_t regstr = WHO_AM_I; // Address of the WHO_AM_I register

    uint8_t test_num; // Expecting 0x68 (104 in decimal)

    i2c_write_blocking(i2c_default, address, &regstr, 1, true);  // true to keep master control of bus
    i2c_read_blocking(i2c_default, address, &test_num, 1, false);  // false - finished with bus

    if (test_num == 0x68) {
        printf("Looking good! Test successful!\n");
    } else {
        gpio_put(PICO2LED, 1);
        while (1);
            sleep_ms(1000);
    }
}

void init_mpu6050() {
    uint8_t address = CHIPADDR; // Address of the chip

    uint8_t pwr_mgmt_1_reg = PWR_MGMT_1; 
    uint8_t accel_config_reg = ACCEL_CONFIG;
    uint8_t gyro_config_reg = GYRO_CONFIG;

    uint8_t pwr_mgmt_1_buf[2];
    uint8_t accel_config_buf[2];
    uint8_t gyro_config_buf[2];

    // PWR_MGMT_1 CONFIG
    pwr_mgmt_1_buf[0] = PWR_MGMT_1;
    pwr_mgmt_1_buf[1] = 0x00; // value to turn the chip on
    // END OF PWR_MGMT_1 CONFIG


    // ACCEL_CONFIG CONFIG
    accel_config_buf[0] = ACCEL_CONFIG;
    uint8_t accel_config_buf_interim;

    i2c_write_blocking(i2c_default, address, &accel_config_reg, 1, true);  // true to keep master control of bus
    i2c_read_blocking(i2c_default, address, &accel_config_buf_interim, 1, false);  // false - finished with bus

    accel_config_buf_interim = accel_config_buf_interim & 0b11100111;
    // 00 sets it to +/- 2g, the range doubling each incrememnt of the corresponding decimal value
    // (01: +/- 4g, 10: +/- 8g, 11: +/- 16g)
    // the & statement prevents the existing values in the other bits of the register from being changed

    accel_config_buf[1] = accel_config_buf_interim; // value to turn the chip on
    // END OF ACCEL_CONFIG CONFIG


    // GYRO_CONFIG CONFIG
    gyro_config_buf[0] = GYRO_CONFIG;
    uint8_t gyro_config_buf_interim;

    i2c_write_blocking(i2c_default, address, &gyro_config_reg, 1, true);  // true to keep master control of bus
    i2c_read_blocking(i2c_default, address, &gyro_config_buf_interim, 1, false);  // false - finished with bus

    gyro_config_buf_interim = gyro_config_buf_interim | 0b11 << 3;
    // 11 sets it to +/- 2000 deg/s, the range halving each decrease by 1 of the corresponding decimal value
    // (00: +/- 250 deg/s, 01: +/- 500 deg/s, 10: +/- 1000 deg/s)
    // the | statement prevents the existing values in the other bits of the register from being changed

    gyro_config_buf[1] = gyro_config_buf_interim; // value to turn the chip on
    // END OF GYRO_CONFIG CONFIG


    uint8_t test_num; // Expecting 0x68 (104 in decimal)

    i2c_write_blocking(i2c_default, address, pwr_mgmt_1_buf, 2, false);
    i2c_write_blocking(i2c_default, address, accel_config_buf, 2, false);
    i2c_write_blocking(i2c_default, address, gyro_config_buf, 2, false);
}

void read_mpu6050(float * result) {
    uint8_t address = CHIPADDR; // Address of the chip
    uint8_t starting_reg = ACCEL_XOUT_H; //Address of ACCEL_XOUT_H

    uint8_t num_of_expected_data = 14;
    uint8_t output_data[num_of_expected_data];
    
    i2c_write_blocking(i2c_default, address, &starting_reg, 1, true);  // true to keep master control of bus
    i2c_read_blocking(i2c_default, address, output_data, num_of_expected_data, false);  // false - finished with bus

    result[0] = byte_merge(output_data[0], output_data[1]) * 0.000061; //ACCEL_XOUT
    result[1] = byte_merge(output_data[2], output_data[3]) * 0.000061; //ACCEL_YOUT
    result[2] = byte_merge(output_data[4], output_data[5]) * 0.000061; //ACCEL_ZOUT
    result[3] = (byte_merge(output_data[6], output_data[7]) / 340.0) + 36.53; //TEMP_OUT
    result[4] = byte_merge(output_data[8], output_data[9]) * 0.007630; //GYRO_XOUT
    result[5] = byte_merge(output_data[10], output_data[11]) * 0.007630; //GYRO_YOUT
    result[6] = byte_merge(output_data[12], output_data[13]) * 0.007630; //GYRO_ZOUT
}

uint16_t byte_merge(uint8_t high_bits, uint8_t low_bits) {
    return (high_bits << 8) | low_bits;
}
