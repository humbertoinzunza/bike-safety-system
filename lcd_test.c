#include "i2c.h"
#include "lcd.h"
#include "serial.h"
#include <util/delay.h>

#define FOSC 7372970                        // Clock frequency = Oscillator freq.
#define BAUD 9600              // Baud rate used by the LCD
#define MYUBRR FOSC/16/BAUD-1   // Value for UBRR0 register
#define BDIV (FOSC / 50000 - 16) / 2 + 1   // Puts I2C rate just below 50kHz

int main(void)
{
    struct LCD lcd;
    i2c_init(BDIV);
    serial_init(MYUBRR);
    _delay_ms(1000);
    lcd_init(&lcd);
    unsigned char ch;
   while(1) {
        ch = serial_in();
        _delay_ms(10);
        print_character(&lcd, ch);
   }
}

