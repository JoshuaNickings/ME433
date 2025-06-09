#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include <math.h>

// Using DRV8833 Chip

#define IN1GP   16  // PMW Control 1
#define IN2GP   17  // Direction Control 1
#define IN3GP   18  // PMW Control 2
#define IN4GP   19  // Direction Control 2
#define OUT1GP  12  // Output 1+
#define OUT2GP  13  // Output 1-
#define OUT3GP  14  // Output 2+
#define OUT4GP  15  // Output 2-

float motor1_pwm_div = 3;
float motor2_pwm_div = 3;

uint16_t motor1_pwm_wrap = 50000;
uint16_t motor2_pwm_wrap = 50000;

int motor1_duty_percent = 0;
int motor2_duty_percent = 0;


void init_pwm(int, float, uint16_t, float);

void drive_motor(int, float);

float motor1_duty();

float motor2_duty();

int main()
{
    stdio_init_all();

    // initialize input PWM pins

    init_pwm(IN1GP, motor1_pwm_div, motor1_pwm_wrap, 0);
    init_pwm(IN3GP, motor2_pwm_div, motor2_pwm_wrap, 0);

    // initialize the direction control pins as input/outputs
    gpio_init(IN2GP);
    gpio_set_dir(IN2GP, GPIO_OUT);
    drive_motor(0, motor1_duty()); // drive motor 1 forward at initialized speed

    gpio_init(IN4GP);
    gpio_set_dir(IN4GP, GPIO_OUT);
    drive_motor(1, motor2_duty()); // drive motor 2 forward at initialized speed

    // initialize the output pins to read the output
    gpio_init(OUT1GP);
    gpio_set_dir(OUT1GP, GPIO_IN);

    gpio_init(OUT2GP);
    gpio_set_dir(OUT2GP, GPIO_IN);

    gpio_init(OUT3GP);
    gpio_set_dir(OUT3GP, GPIO_IN);

    gpio_init(OUT4GP);
    gpio_set_dir(OUT4GP, GPIO_IN);

    while (true) {
        printf("Type + or - to increase or decrease the duty cycle by 1%% respectively.\n");
        char input[1];
        scanf("%c", input);
        if (input[0] == 43) {
            motor1_duty_percent = motor1_duty_percent + 10;
            motor2_duty_percent = motor2_duty_percent + 10;
        } else if (input[0] == 45) {
            motor1_duty_percent = motor1_duty_percent - 10;
            motor2_duty_percent = motor2_duty_percent - 10;
        } else {
            continue;
        }

        drive_motor(0, motor1_duty());
        drive_motor(1, motor2_duty());
        printf("Programmed Motor 1 Duty: %d%%\nProgrammed Motor 2 Duty: %d%%\n\n", motor1_duty_percent, motor2_duty_percent);

        /*
        int trials1 = ((150000000 / motor1_pwm_div) / motor1_pwm_wrap);
        unsigned long long sum1 = 0;
        unsigned long long sum2 = 0;

        for (int i = 0; i < trials1; i++) {
            sum1 = sum1 + gpio_get(OUT1GP);
            sum2 = sum2 + gpio_get(OUT2GP);
            sleep_us(1000000/trials1);
        }

        int trials2 = ((150000000 / motor2_pwm_div) / motor2_pwm_wrap);
        unsigned long long sum3 = 0;
        unsigned long long sum4 = 0;

        for (int i = 0; i < trials2; i++) {
            sum3 = sum3 + gpio_get(OUT3GP);
            sum4 = sum4 + gpio_get(OUT4GP);
            sleep_us(1000000/trials2);
        }
        // printf("%d %d %d %d", gpio_get(OUT1GP), gpio_get(OUT2GP), gpio_get(OUT3GP), gpio_get(OUT4GP));
        printf("Measured Output 1 Duty: %f%%\nMeasured Output 2 Duty: %f%%\nMeasured Output 3 Duty: %f%%\nMeasured Output 4 Duty: %f%%\n\n", ((sum1 * 1.0) / trials1 * 100.0), ((sum2 * 1.0) / trials1 * 100.0), ((sum3 * 1.0) / trials2 * 100.0), ((sum4 * 1.0) / trials2 * 100.0));
        */
    }
}

void init_pwm(int pin, float d, uint16_t w, float p) {

    gpio_set_function(pin, GPIO_FUNC_PWM); // Set the LED Pin to be PWM
    uint slice_num = pwm_gpio_to_slice_num(pin); // Get PWM slice number
    float div = d; // must be between 1-255
    pwm_set_clkdiv(slice_num, div); // divider
    uint16_t wrap = w; // when to rollover, must be less than 65535
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true); // turn on the PWM

    pwm_set_gpio_level(pin, ((int) (wrap * p))); // set the duty cycle
}

void drive_motor(int motor, float duty) {
    if (motor > 0) {
        motor = 1;
    } else {
        motor = 0;
    }
    // protecting against potentially false numerical inputs

    if (motor == 0) {
        motor = IN2GP;
    } else {
        motor = IN4GP;
    }
    // low drives motor 1, high drives motor 2

    if (duty > 1.0) {
        duty = 1.0;
    } else if (duty < -1.0) {
        duty = -1.0;
    }
    // protecting against potentially false numerical inputs
    if (motor == IN2GP){
        motor1_duty_percent = (int) (duty * 100);
    } else {
        motor2_duty_percent = (int) (duty * 100);
    }    
    
    int dir = 0;
    if (duty < 0) {
        duty = -1 - duty;
        dir = 1;
    }
    // duty cycle needs to be inverted for reverse drive

    gpio_put(motor, dir);
    if (motor == IN2GP) {
        pwm_set_gpio_level(IN1GP, fabs(((int) (motor1_pwm_wrap * duty)))); 
    } else {
        pwm_set_gpio_level(IN3GP, fabs(((int) (motor2_pwm_wrap * duty)))); 
    }
}

float motor1_duty() {
    return motor1_duty_percent/100.0;
}

float motor2_duty() {
    return motor2_duty_percent/100.0;
}