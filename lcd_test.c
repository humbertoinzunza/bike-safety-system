#include "i2c.h"
#include "lcd.h"
#include "gps.h"
#include "serial.h"
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define FOSC 7372970                        // Clock frequency = Oscillator freq.
#define BAUD 9600              // Baud rate used by the LCD
#define MYUBRR FOSC/16/BAUD-1   // Value for UBRR0 register
#define BDIV (FOSC / 50000 - 16) / 2 + 1 // Puts I2C rate just below 50kHz
// Circumference of a 700c bike wheel in miles
#define MILES_PER_REV 0.001366466620877824f

void absolute(short *, short *, short *);
volatile unsigned char rps = 0; // Revolutions per second
volatile unsigned char seconds = 0;
volatile float distance = 0.0f;
volatile float speed = 0.0f;
// LCD struct
struct LCD lcd;

void setup()
{
  // Disable global interrupts
  cli();
  // Initialize I2C bus, LCD, and serial interface
  i2c_init(BDIV);
  lcd_init(&lcd);
  
  // Set PORTD bit 2 for input
  DDRD &= ~_BV(DDD2);
	// Pin Change Interrupt enable on PCINT2 (Enables PCMSK2 scan)
	PCICR |= _BV(PCIE2);
  // Enable scan for PCINT18 (PD2)
  PCMSK2 |= _BV(PCINT18);
  
  //set timer1 interrupt at 1Hz
  TCCR1A = 0; // Set entire TCCR1A register to 0
  TCCR1B = 0; // Same for TCCR1B
  TCNT1  = 0; // Initialize counter value to 0
  // Set compare match register for 1Hz increments
  OCR1A = 7199; // = (7372970) / (1*1024) - 1 (must be <65536)
  // Turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // Enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  // Enable global interrupts
  sei();
}

int main(void) {
  // Buffer for printing on screen
  char buffer[25];
  setup();
  while (1) {
    set_cursor(&lcd, 0, 0);
    sprintf(buffer, "%.2f mph", speed);
    print_string(&lcd, buffer);
    set_cursor(&lcd, 1, 0);
    sprintf(buffer, "Traveled %.2f mi", distance);
    print_string(&lcd, buffer);
  }
    return 0;   /* never reached */
}

void absolute(short *ret_val, short *a, short *b) {
  *ret_val = *a - *b;
  if (*ret_val < 0) *ret_val = -(*ret_val);
}

ISR(PCINT2_vect) {
  if (PIND & (1 << PIND2))
    rps++;
}

ISR(TIMER1_COMPA_vect) {
  cli();
  speed = rps * MILES_PER_REV; // Miles traveled in the second passed
  distance += speed;
  speed *= 3600; // To get miles per hour
  rps = 0;
  sei();
}

