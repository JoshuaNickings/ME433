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
// MCLK on GP10, set 50% 10MHz PWM
#define MCLK 10
// PCLK on GP11
#define PCLK 11
// RST to GP12
#define RST 12
// PWDN to float (or GND?)

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


void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == VS){
        //printf("v\n");
        if (saveImage==1){
            //printf("v\n");
            rawIndex = 0;
            hsCount = 0;
            vsCount = 0;
            startImage = 1;
            startCollect = 0;
        }
    }
    if (gpio == HS){
        //printf("h");
        if(saveImage){
            if (startImage){
                startCollect = 1;
                hsCount++;
                if (hsCount == IMAGESIZEY){
                    //printf("%d",hsCount);
                    saveImage = 0;
                    startImage = 0;
                    startCollect = 0;
                    hsCount = 0;
                }
            }
        }
    }
    if (gpio == PCLK){
        if(saveImage){
            if(startImage){
                if(startCollect){
                    vsCount++;
                    // read the raw data
                    uint32_t d = gpio_get_all();
                    cameraData[rawIndex] = d & 0xFF;
                    rawIndex++;
                    if (rawIndex == IMAGESIZEX*IMAGESIZEY*2){
                        saveImage = 0;
                        startImage = 0;
                        startCollect = 0;
                    }
                    if (vsCount == IMAGESIZEX*2){
                        startCollect = 0;
                        vsCount = 0;
                    }
                }
            }
        }
    }
}

int main()
{
    stdio_init_all();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Hello, camera!\n");

    init_camera_pins();
 
    while (true) {
        //sleep_ms(1000);
        char m[10];
        scanf("%s",m);
        setSaveImage(1);
        while(getSaveImage()==1){}
        //printf("HS count = %d PixelCount = %d\n",getHSCount(), getPixelCount());
        convertImage();
        printImage();
    }
}

// setup the camera pins
void init_camera_pins(){
    // 8 data pins
    gpio_init(D0);
    gpio_set_dir(D0, GPIO_IN);
    gpio_init(D1);
    gpio_set_dir(D1, GPIO_IN);
    gpio_init(D2);
    gpio_set_dir(D2, GPIO_IN);
    gpio_init(D3);
    gpio_set_dir(D3, GPIO_IN);
    gpio_init(D4);
    gpio_set_dir(D4, GPIO_IN);
    gpio_init(D5);
    gpio_set_dir(D5, GPIO_IN);
    gpio_init(D6);
    gpio_set_dir(D6, GPIO_IN);
    gpio_init(D7);
    gpio_set_dir(D7, GPIO_IN);

    gpio_init(RST); // reset pin
    gpio_set_dir(RST, GPIO_OUT);
    gpio_put(RST, 1);

    // set MCLK to 50% 10MHz PWM
    gpio_set_function(MCLK, GPIO_FUNC_PWM); // Set the LED Pin to be PWM
    uint slice_num = pwm_gpio_to_slice_num(MCLK); // Get PWM slice number
    float div = 2; // must be between 1-255
    pwm_set_clkdiv(slice_num, div); // divider
    uint16_t wrap = 3; // when to rollover, must be less than 65535
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true); // turn on the PWM
    pwm_set_gpio_level(MCLK, wrap / 2); // set the duty cycle to 50%

    sleep_ms(1000); // give the camera time to get going

    // I2C Initialisation. Using it at 100Khz.
    i2c_init(I2C_PORT, 100*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    printf("Start init camera\n");
    init_camera();
    printf("End init camera\n");

    // interrupts
    gpio_init(VS); // vertical sync
    gpio_set_dir(VS, GPIO_IN);
    // new image starts on falling VS
    gpio_set_irq_enabled_with_callback(VS, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    gpio_init(HS); // horizontal sync
    gpio_set_dir(HS, GPIO_IN);
    // new row starts on rising HS
    gpio_set_irq_enabled_with_callback(HS, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);

    gpio_init(PCLK); // pixel clock
    gpio_set_dir(PCLK, GPIO_IN);
    // read byte on rising PCLK
    gpio_set_irq_enabled_with_callback(PCLK, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
}

// init the camera with RST and I2C commands
void init_camera(){
    // hardware reset the camera
    gpio_put(RST, 0);
    sleep_ms(1);
    gpio_put(RST, 1);
    sleep_ms(1000);

    OV7670_write_register(0x12, 0x80); // software reset
    sleep_ms(1000);

    // perform all the I2C writes for init
    // 25MHz * PLL / divisor = 24MHz for 30fps
    OV7670_write_register(OV7670_REG_CLKRC, 1); // div 1
    OV7670_write_register(OV7670_REG_DBLV, 0); // no pll

    // set colorspace to RGB565
    int i = 0;
    for(i=0; i<3; i++){
        OV7670_write_register(OV7670_rgb[i][0],OV7670_rgb[i][1]);
    }

    // init regular registers
    for(i=0; i<92; i++){
        OV7670_write_register(OV7670_init[i][0],OV7670_init[i][1]);
    }

    // init image size
    
    // Window settings were tediously determined empirically.
    // I hope there's a formula for this, if a do-over is needed.
    //{vstart,hstart,edge_offset,pclk_delay}
            //{9, 162, 2, 2},  // SIZE_DIV1  640x480 VGA
            //{10, 174, 4, 2}, // SIZE_DIV2  320x240 QVGA
            //{11, 186, 2, 2}, // SIZE_DIV4  160x120 QQVGA
            //{12, 210, 0, 2}, // SIZE_DIV8  80x60   ...
            //{15, 252, 3, 2}, // SIZE_DIV16 40x30

    uint8_t value;
    uint8_t size = OV7670_SIZE_DIV8; // 80x60
    uint16_t vstart = 12;
    uint16_t hstart = 210;
    uint16_t edge_offset = 0;
    uint16_t pclk_delay = 2;

    // Enable downsampling if sub-VGA, and zoom if 1:16 scale
    value = (size > OV7670_SIZE_DIV1) ? OV7670_COM3_DCWEN : 0;
    if (size == OV7670_SIZE_DIV16)
    value |= OV7670_COM3_SCALEEN;
    OV7670_write_register(OV7670_REG_COM3, value);

    // Enable PCLK division if sub-VGA 2,4,8,16 = 0x19,1A,1B,1C
    value = (size > OV7670_SIZE_DIV1) ? (0x18 + size) : 0;
    OV7670_write_register(OV7670_REG_COM14, value);

    // Horiz/vert downsample ratio, 1:8 max (H,V are always equal for now)
    value = (size <= OV7670_SIZE_DIV8) ? size : OV7670_SIZE_DIV8;
    OV7670_write_register(OV7670_REG_SCALING_DCWCTR, value * 0x11);

    // Pixel clock divider if sub-VGA
    value = (size > OV7670_SIZE_DIV1) ? (0xF0 + size) : 0x08;
    OV7670_write_register(OV7670_REG_SCALING_PCLK_DIV, value);

    // Apply 0.5 digital zoom at 1:16 size (others are downsample only)
    value = (size == OV7670_SIZE_DIV16) ? 0x40 : 0x20; // 0.5, 1.0
    // Read current SCALING_XSC and SCALING_YSC register values because
    // test pattern settings are also stored in those registers and we
    // don't want to corrupt anything there.

    uint8_t xsc = OV7670_read_register(OV7670_REG_SCALING_XSC);
    uint8_t ysc = OV7670_read_register(OV7670_REG_SCALING_YSC);

    xsc = (xsc & 0x80) | value; // Modify only scaling bits (not test pattern)
    ysc = (ysc & 0x80) | value;
    // Write modified result back to SCALING_XSC and SCALING_YSC
    OV7670_write_register(OV7670_REG_SCALING_XSC, xsc);
    OV7670_write_register(OV7670_REG_SCALING_YSC, ysc);

    // Window size is scattered across multiple registers.
    // Horiz/vert stops can be automatically calc'd from starts.
    uint16_t vstop = vstart + 480;
    uint16_t hstop = (hstart + 640) % 784;
    OV7670_write_register(OV7670_REG_HSTART, hstart >> 3);
    OV7670_write_register(OV7670_REG_HSTOP, hstop >> 3);
    OV7670_write_register(OV7670_REG_HREF,(edge_offset << 6) | ((hstop & 0b111) << 3) | (hstart & 0b111));
    OV7670_write_register(OV7670_REG_VSTART, vstart >> 2);
    OV7670_write_register(OV7670_REG_VSTOP, vstop >> 2);
    OV7670_write_register(OV7670_REG_VREF, ((vstop & 0b11) << 2) | (vstart & 0b11));
    OV7670_write_register(OV7670_REG_SCALING_PCLK_DELAY, pclk_delay);

    sleep_ms(300); // allow camera to settle with new settings 

    //OV7670_test_pattern(OV7670_TEST_PATTERN_COLOR_BAR);

    sleep_ms(300);

    uint8_t p = OV7670_read_register(OV7670_REG_PID);
    printf("pid = %d\n",p);

    uint8_t v = OV7670_read_register(OV7670_REG_VER);
    printf("pid = %d\n",v);
}

// Selects one of the camera's test patterns (or disable).
// See Adafruit_OV7670.h for notes about minor visual bug here.
void OV7670_test_pattern(OV7670_pattern pattern) {
    // Read current SCALING_XSC and SCALING_YSC register settings,
    // so image scaling settings aren't corrupted.
    uint8_t xsc = OV7670_read_register(OV7670_REG_SCALING_XSC);
    uint8_t ysc = OV7670_read_register(OV7670_REG_SCALING_YSC);
    if (pattern & 1) {
      xsc |= 0x80;
    } else {
      xsc &= ~0x80;
    }
    if (pattern & 2) {
      ysc |= 0x80;
    } else {
      ysc &= ~0x80;
    }
    // Write modified results back to SCALING_XSC and SCALING_YSC registers
    OV7670_write_register(OV7670_REG_SCALING_XSC, xsc);
    OV7670_write_register(OV7670_REG_SCALING_YSC, ysc);
  }

// I2C write to the camera
void OV7670_write_register(uint8_t reg, uint8_t value){
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = value;
    i2c_write_blocking(I2C_PORT, OV7670_ADDR, buf, 2, false);
    sleep_ms(1); // after each
}

// I2C read from the camera
uint8_t OV7670_read_register(uint8_t reg){
    uint8_t buf;
    i2c_write_blocking(I2C_PORT, OV7670_ADDR, &reg, 1, false);  // true to keep master control of bus
    i2c_read_blocking(I2C_PORT, OV7670_ADDR, &buf, 1, false);  // false - finished with bus
    return buf;
}

// save an image
void setSaveImage(uint32_t s){
    saveImage = s;
}

// see if you are supposed to be saving an image
uint32_t getSaveImage(){
    return saveImage;
}

// how many rows were counted, should be IMAGESIZEY
uint32_t getHSCount(){
    return hsCount;
}

// how many pixels were counted times 2, should be 2*IMAGESIZEX*IMAGESIZEY
uint32_t getPixelCount(){
    return rawIndex;
}

// convert the raw image to RGB
// https://blog.usedbytes.com/2022/02/pico-pio-camera/
void convertImage(){
    picture.index = 0;
    int i = 0;
    for(i=0;i<IMAGESIZEX*IMAGESIZEY*2;i=i+2){
        
        picture.r[picture.index] = cameraData[i]>>3;
        picture.g[picture.index] = ((cameraData[i]&0b111)<<3) | cameraData[i+1]>>5;
        picture.b[picture.index] = cameraData[i+1]&0b11111;
        
       /*
        picture.r[picture.index] = cameraData[i];
        picture.g[picture.index] = cameraData[i];
        picture.b[picture.index] = cameraData[i];
        */
        picture.index++;
    }
}

// print out the image to computer
void printImage(){
    int i = 0;
    //printf("a\r\n"); // send a start frame byte
    for(i=0;i<IMAGESIZEX*IMAGESIZEY;i++){
        printf("%d %d %d %d\r\n", i, picture.r[i], picture.g[i], picture.b[i]);
    }
    //printf("b\r\n"); // send a stop frame byte
}
