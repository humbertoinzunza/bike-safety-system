#include "i2c.h"
#include "lcd.h"
#include "gps.h"
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
    unsigned char buffer[150];
    short response[4];
    unsigned char code;
    unsigned char lat[15];
    unsigned char lon[15];
   while(1) {
     //set_cursor(&lcd, 0, 0);
     short bytes_read = read_sentence(buffer, 150);
     sprintf(lat, "%i ", bytes_read);
     //print_array(&lcd, buffer, bytes_read);
     print_string(&lcd, lat);
     /*
        code = parse_sentence(buffer, response);
        if(code == 0) {
          sprintf(lat, "Lat: %i.%i", response[0], response[1]);
          sprintf(lon, "Lon: %i.%i", response[2], response[3]);
          set_cursor(&lcd, 0, 0);
          print_string(&lcd, lat);
          set_cursor(&lcd, 1, 0);
          print_string(&lcd, lon);
        }
        else {
          set_cursor(&lcd, 0, 0);
          print_string(&lcd, "Parsing error");
          sprintf(lat, "%i %c %c %i", response[0], response[1], response[2], response[3]);
          set_cursor(&lcd, 1, 0);
          print_string(&lcd, lat);
        }
     */
   }

}

