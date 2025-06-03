/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

/**
 * NOTE:
 *  Take into consideration if your WS2812 is a RGB or RGBW variant.
 *
 *  If it is RGBW, you need to set IS_RGBW to true and provide 4 bytes per 
 *  pixel (Red, Green, Blue, White) and use urgbw_u32().
 *
 *  If it is RGB, set IS_RGBW to false and provide 3 bytes per pixel (Red,
 *  Green, Blue) and use urgb_u32().
 *
 *  When RGBW is used with urgb_u32(), the White channel will be ignored (off).
 *
 */

#define IS_RGBW false
#define NUM_PIXELS 4

#ifdef PICO_DEFAULT_WS2812_PIN
#define WS2812_PIN PICO_DEFAULT_WS2812_PIN
#else
// default to pin 15 if the board doesn't have a default WS2812 pin defined
#define WS2812_PIN 15
#endif

#define RCServoPin 16

// Check the pin is compatible with the platform
#if WS2812_PIN >= NUM_BANK0_GPIOS
#error Attempting to use a pin>=32 on a platform that does not support it
#endif

static inline void put_pixel(PIO pio, uint sm, uint32_t pixel_grb) {
    pio_sm_put_blocking(pio, sm, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 16) |
            ((uint32_t) (g) << 8) |
            (uint32_t) (b);
}

static inline uint32_t urgbw_u32(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            ((uint32_t) (w) << 24) |
            (uint32_t) (b);
}

void pattern_snakes(PIO pio, uint sm, uint len, uint t) {
    for (uint i = 0; i < len; ++i) {
        uint x = (i + (t >> 1)) % 64;
        if (x < 10)
            put_pixel(pio, sm, urgb_u32(0xff, 0, 0));
        else if (x >= 15 && x < 25)
            put_pixel(pio, sm, urgb_u32(0, 0xff, 0));
        else if (x >= 30 && x < 40)
            put_pixel(pio, sm, urgb_u32(0, 0, 0xff));
        else
            put_pixel(pio, sm, 0);
    }
}

void pattern_random(PIO pio, uint sm, uint len, uint t) {
    if (t % 8)
        return;
    for (uint i = 0; i < len; ++i)
        put_pixel(pio, sm, rand());
}

void pattern_sparkle(PIO pio, uint sm, uint len, uint t) {
    if (t % 8)
        return;
    for (uint i = 0; i < len; ++i)
        put_pixel(pio, sm, rand() % 16 ? 0 : 0xffffffff);
}

void pattern_greys(PIO pio, uint sm, uint len, uint t) {
    uint max = 100; // let's not draw too much current!
    t %= max;
    for (uint i = 0; i < len; ++i) {
        put_pixel(pio, sm, t * 0x10101);
        if (++t >= max) t = 0;
    }
}

typedef void (*pattern)(PIO pio, uint sm, uint len, uint t);
const struct {
    pattern pat;
    const char *name;
} pattern_table[] = {
        {pattern_snakes,  "Snakes!"},
        {pattern_random,  "Random data"},
        {pattern_sparkle, "Sparkles"},
        {pattern_greys,   "Greys"},
};


typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} wsColor; 

void setLEDs(PIO, uint, wsColor *);

wsColor HSBtoRGB(float, float, float);

void init_pwm(int, float, uint16_t);


int main() {
    //set_sys_clock_48();
    stdio_init_all();
    printf("WS2812 Smoke Test, using pin %d\n", WS2812_PIN);
    
    float div_val = 50;
    uint16_t wrap_val = 60000;

    init_pwm(RCServoPin, div_val, wrap_val);

    // todo get free sm
    PIO pio;
    uint sm;
    uint offset;

    // This will find a free pio and state machine for our program and load it for us
    // We use pio_claim_free_sm_and_add_program_for_gpio_range (for_gpio_range variant)
    // so we will get a PIO instance suitable for addressing gpios >= 32 if needed and supported by the hardware
    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(&ws2812_program, &pio, &sm, &offset, WS2812_PIN, 1, true);
    hard_assert(success);

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    int a = 0;
    int b = 90;
    int c = 180;
    int d = 270;

    wsColor led_color_cycle[1440];

    for (int i = 0; i < 360; i++) {
        a = a + 1;
        b = b + 1;
        c = c + 1;
        d = d + 1;
        if (a >= 360){
            a = 0;
        }
        if (b >= 360){
            b = 0;
        }
        if (c >= 360){
            c = 0;
        }
        if (d >= 360){
            d = 0;
        }
        led_color_cycle[i*4] = HSBtoRGB(a, 1, 0.2);
        led_color_cycle[i*4+1] = HSBtoRGB(b, 1, 0.2);
        led_color_cycle[i*4+2] = HSBtoRGB(c, 1, 0.2);
        led_color_cycle[i*4+3] = HSBtoRGB(d, 1, 0.2);
    }

    wsColor led_color_stage[4];


    uint16_t rcservo_pmw_traj[360];

    for (int j = 0; j < 360; j++) {
        rcservo_pmw_traj[j] = (wrap_val * 0.1 / 180) + (wrap_val * 0.025);
        if (j > 180) {
             rcservo_pmw_traj[j] = (wrap_val * -0.1 / 180) + (wrap_val * 0.225);
        }
    }

    int t = 0;
    while (1) {
        for (int t = 0; t < 360; t++){
            
            led_color_stage[0] = led_color_cycle[t*4];
            led_color_stage[1] = led_color_cycle[t*4+1];
            led_color_stage[2] = led_color_cycle[t*4+2];
            led_color_stage[3] = led_color_cycle[t*4+3];
            setLEDs(pio, sm, led_color_stage);

            pwm_set_gpio_level(RCServoPin, rcservo_pmw_traj[t]);

            sleep_us(5000000/360);

        }
        
        /*
        int pat = rand() % count_of(pattern_table);
        int dir = (rand() >> 30) & 1 ? 1 : -1;
        puts(pattern_table[pat].name);
        puts(dir == 1 ? "(forward)" : "(backward)");
        for (int i = 0; i < 1000; ++i) {
            pattern_table[pat].pat(pio, sm, NUM_PIXELS, t);
            sleep_ms(10);
            t += dir;
        }
        */
    }

    // This will free resources and unload our program
    pio_remove_program_and_unclaim_sm(&ws2812_program, pio, sm, offset);
}

void setLEDs(PIO p, uint s, wsColor * colors){
    int i;
    for(i=0;i<NUM_PIXELS;i++){
        put_pixel(p, s, urgb_u32(colors[i].r, colors[i].g, colors[i].b)); // assuming you've made arrays of colors to send
    }
    sleep_ms(1); // wait at least the reset time
}

// adapted from https://forum.arduino.cc/index.php?topic=8498.0
// hue is a number from 0 to 360 that describes a color on the color wheel
// sat is the saturation level, from 0 to 1, where 1 is full color and 0 is gray
// brightness sets the maximum brightness, from 0 to 1
wsColor HSBtoRGB(float hue, float sat, float brightness) {
    float red = 0.0;
    float green = 0.0;
    float blue = 0.0;

    if (sat == 0.0) {
        red = brightness;
        green = brightness;
        blue = brightness;
    } else {
        if (hue == 360.0) {
            hue = 0;
        }

        int slice = hue / 60.0;
        float hue_frac = (hue / 60.0) - slice;

        float aa = brightness * (1.0 - sat);
        float bb = brightness * (1.0 - sat * hue_frac);
        float cc = brightness * (1.0 - sat * (1.0 - hue_frac));

        switch (slice) {
            case 0:
                red = brightness;
                green = cc;
                blue = aa;
                break;
            case 1:
                red = bb;
                green = brightness;
                blue = aa;
                break;
            case 2:
                red = aa;
                green = brightness;
                blue = cc;
                break;
            case 3:
                red = aa;
                green = bb;
                blue = brightness;
                break;
            case 4:
                red = cc;
                green = aa;
                blue = brightness;
                break;
            case 5:
                red = brightness;
                green = aa;
                blue = bb;
                break;
            default:
                red = 0.0;
                green = 0.0;
                blue = 0.0;
                break;
        }
    }

    unsigned char ired = red * 255.0;
    unsigned char igreen = green * 255.0;
    unsigned char iblue = blue * 255.0;

    wsColor c;
    c.r = ired;
    c.g = igreen;
    c.b = iblue;
    return c;
}

void init_pwm(int pin, float d, uint16_t w){
    gpio_set_function(pin, GPIO_FUNC_PWM); // Set the LED Pin to be PWM
    uint slice_num = pwm_gpio_to_slice_num(pin); // Get PWM slice number
    float div = d; // must be between 1-255
    pwm_set_clkdiv(slice_num, div); // divider
    uint16_t wrap = w; // when to rollover, must be less than 65535
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true); // turn on the PWM
}