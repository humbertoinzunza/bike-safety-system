#include "i2c.h"
#include "lcd.h"
//#include "gps.h"
//#include "serial.h"
#include <util/delay.h>
#include <avr/io.h>
#include <math.h>

#define FOSC 7372970                        // Clock frequency = Oscillator freq.
#define BAUD 9600              // Baud rate used by the LCD
#define MYUBRR FOSC/16/BAUD-1   // Value for UBRR0 register
#define BDIV (FOSC / 50000 - 16) / 2 + 1   // Puts I2C rate just below 50kHz

void abs(short *, short *, short *);

int main(void)
{
  short prev_location[6];
  short last_location[6];
  short temp;
  float temp_y, temp_x;
  short *pl_ptr, *ll_ptr;
  float total_distance = 0.0f;
  float last_distance = 0.0f;
  unsigned char i;
  struct LCD lcd;
  i2c_init(BDIV);
  _delay_ms(1000);
  lcd_init(&lcd);
  serial_init(MYUBRR);
  unsigned char buffer[100];
  unsigned char speed[25];
  unsigned char distance[25];
  unsigned char deg_symbol = 0xDF;
  while(1) {
    get_gpgga(buffer);
    // Get the miles travelled through latitude
    if(parse_sentence(buffer, last_location)) {
      abs(&temp, &last_location[2], &prev_location[2]);
      if (temp > 300) {
        temp = 10000 - temp;
      }
      temp_y = 0.000115f * temp;
      // Get the miles traveled through longitude
      abs(&temp, &last_location[5], &prev_location[5]);
      if (temp > 300) {
        temp = 10000 - temp;
      }
      temp_x = 0.000091f * temp;
      last_distance = sqrt(temp_x * temp_x + temp_y * temp_y);
      total_distance += last_distance;
      // Calculate speed (reuse variable temp_x)
      temp_x *= 3600;
      pl_ptr = prev_location;
      ll_ptr = last_location;
      // Set previous location to the last location
      for (i = 0; i < 6; i++) *(pl_ptr++) = *(ll_ptr++);
    }
    sprintf(speed, "Speed: %.4f mph", temp_x);
    sprintf(distance, "%.4f miles traveled");
    set_cursor(&lcd, 0, 0);
    print_string(&lcd, speed);
    set_cursor(&lcd, 1, 0);
    print_string(&lcd, distance);
  }
}

void abs(short *ret_val, short *a, short *b) {
  *ret_val = *a - *b;
  if (*ret_val < 0) *ret_val = -(*ret_val);
}

