#include "display.h"

void drawMessage(int x, int y, char * m){
    // x and y should be the top left corner of the letter to draw!
    int i = 0;
    while(m[i]!=0){
        drawLetter(x+i*5, y, m[i]);
        i++;
    }
}

void drawLetter(int x, int y, char c){
    int row, col;
    row = c-0x20;
    col = 0;
    for (col = 0; col < 5; col++){
        char byte = ASCII[row][col];

        for (int i = 0; i < 8; i++){
            char on_or_off = (byte >> i) & 0b1;

            ssd1306_drawPixel(x + col, y + i, on_or_off);
        }
    }    
    
}

void drawLine(int x, int y, int length_px, int angle) {
    int dx, dy;
    for (int i = 0; i < length_px; i++) {
        float rad = angle *  M_PI / 180;
        if (i * cos(rad) < 0) {
            dx = i * cos(rad) - 0.5;
        } else {
            dx = i * cos(rad) + 0.5;
        }

        if (i * sin(rad) < 0) {
            dy = i * sin(rad) - 0.5;
        } else {
            dy = i * sin(rad) + 0.5;
        }

        ssd1306_drawPixel(x + dx, y + dy, 1);
    }
}