#include "i2c.h"
#include "lcd.h"
#include <util/delay.h>

#define FOSC 7372970                        // Clock frequency = Oscillator freq.
#define BDIV (FOSC / 50000 - 16) / 2 + 1   // Puts I2C rate just below 50kHz

int main(void)
{
    struct LCD lcd;
    i2c_init(BDIV);
    _delay_ms(2000);/*
    unsigned char buf[2] = { 0xFE, 0x4B };
    i2c_io(0x50, buf, 2, NULL, 0, NULL, 0);
    buf[0] = 'A';
    while(1) {
        _delay_ms(500);
        i2c_io(0x50, buf, 1, NULL, 0, NULL, 0);\
    }*/
   lcd_init(&lcd);
   unsigned char string[] = "Hello World!";
   while(1) {
        _delay_ms(500);
        print_string(&lcd, string);
   }
}

