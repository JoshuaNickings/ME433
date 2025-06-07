#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"
#include <math.h>

void drawMessage(int, int, char *);
void drawLetter(int, int, char);
void drawLine(int, int, int, int);