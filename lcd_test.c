#include "i2c.h"
#include "lcd.h"
#include "gps.h"
#include "serial.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#define FOSC 7372970                        // Clock frequency = Oscillator freq.
#define BAUD 9600              // Baud rate used by the LCD
#define MYUBRR FOSC/16/BAUD-1   // Value for UBRR0 register
#define BDIV (FOSC / 50000 - 16) / 2 + 1   // Puts I2C rate just below 50kHz

int main(void)
{
  struct LCD lcd;
  i2c_init(BDIV);
  _delay_ms(1000);
  lcd_init(&lcd);
  sei();
  
  while(1) {
  }

}

