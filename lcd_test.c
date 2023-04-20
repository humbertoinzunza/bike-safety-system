#include "i2c.h"
#include "lcd.h"
//#include "gps.h"
//#include "serial.h"
#include <util/delay.h>
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
  serial_init(MYUBRR);
  unsigned char buffer[100];
  short gps_data[6];
  unsigned char lat[20];
  unsigned char lon[20];
  unsigned char deg_symbol = 0xDF;
  while(1) {
    get_gpgga(buffer);
    if(parse_sentence(buffer, gps_data)) {
      sprintf(lat, "Lat: %hi%c %hi.%hi\"", gps_data[0], deg_symbol, gps_data[1], gps_data[2]);
      sprintf(lon, "Lon: %hi%c %hi.%hi\"", gps_data[3], deg_symbol, gps_data[4], gps_data[5]);
      set_cursor(&lcd, 0, 0);
      print_string(&lcd, lat);
      set_cursor(&lcd, 1, 0);
      print_string(&lcd, lon);
    }
    else
    {
      set_cursor(&lcd, 0, 0);
      print_string(&lcd, "No data");
    }
  }
}

