#ifndef CAM_h
#define CAM_h

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "ov7670.h"

// I2C defines
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15

// pins
// D0-D7 on GP0-GP7
#define D0 0
#define D1 1
#define D2 2
#define D3 3
#define D4 4
#define D5 5
#define D6 6
#define D7 7
// VS on GP8
#define VS 8
// HS on GP9
#define HS 9
// MCLK on GP10, set 50% 25MHz PWM
#define MCLK 10
// PCLK on GP11
#define PCLK 11
// RST to GP12
#define RST 12
// PWDN to GP13
#define PWDN 13

// RGB565 example:
// https://blog.usedbytes.com/2022/02/pico-pio-camera/

void init_camera_pins();
void init_camera();
void setSaveImage(uint32_t);
uint32_t getSaveImage();
uint32_t getHSCount();
uint32_t getPixelCount();
void convertImage();
void printImage();
int findLine(int row);
void setPixel(int row, int col, uint8_t r, uint8_t g, uint8_t b);

static volatile uint8_t saveImage = 0; // user requests image
static volatile uint8_t startImage = 0; // got a start of frame
static volatile uint8_t startCollect = 0; // got a start of row
static volatile uint32_t rawIndex = 0;
static volatile uint32_t hsCount = 0;
static volatile uint32_t vsCount = 0;
#define IMAGESIZEX 80
#define IMAGESIZEY 60
static volatile uint8_t cameraData[IMAGESIZEX*IMAGESIZEY*2];

typedef struct cameraImage{
    uint32_t index;
    uint8_t r[IMAGESIZEX*IMAGESIZEY];
    uint8_t g[IMAGESIZEX*IMAGESIZEY];
    uint8_t b[IMAGESIZEX*IMAGESIZEY];
} cameraImage_t;
static volatile struct cameraImage picture;
// I2C functions
void OV7670_write_register(uint8_t reg, uint8_t value);
uint8_t OV7670_read_register(uint8_t reg);
void OV7670_test_pattern(OV7670_pattern pattern);

#endif