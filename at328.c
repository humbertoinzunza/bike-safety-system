/*************************************************************
*       at328-0.c - Demonstrate simple I/O functions of ATmega328
*
*       Program loops turning PC0 on and off as fast as possible.
*
* The program should generate code in the loop consisting of
*   LOOP:   SBI  PORTC,0        (2 cycles)
*           CBI  PORTC,0        (2 cycles)
*           RJMP LOOP           (2 cycles)
*
* PC0 will be low for 4 / XTAL freq
* PC0 will be high for 2 / XTAL freq
* A 9.8304MHz clock gives a loop period of about 600 nanoseconds.
*
* Revision History
* Date     Author      Description
* 09/14/12 A. Weber    Initial Release
* 11/18/13 A. Weber    Renamed for ATmega328P
*************************************************************/

#include <avr/io.h>
#include <stdio.h>
#include "i2c.h"

#define UBRR 47

# define FOSC 7372800 // Clock frequency = Oscillator freq .
# define BDIV ( FOSC / 100000 - 16) / 2 + 1

void serial_init ( unsigned short ubrr ) {
	UBRR0 = ubrr ; // Set baud rate
	UCSR0B |= (1 << TXEN0 ); // Turn on transmitter
	UCSR0B |= (1 << RXEN0 ); // Turn on receiver
	UCSR0C = (3 << UCSZ00 ); // Set for async . operation , no parity ,
	// one stop bit , 8 data bits
}

/*
serial_out - Output a byte to the USART0 port
*/
void serial_out_char ( char ch ) {
	while (( UCSR0A & (1 << UDRE0 )) == 0);
	UDR0 = ch ;
}

void serial_out (char * word, int len) {
	for (int i = 0; i < len; i++) {
		serial_out_char (word[i]);
	}
}

void float_to_str(float f, char* str) {
	char *tmpSign = (f < 0) ? "-" : "";
	float tmpVal = (f < 0) ? -f : f;

	int tmpInt1 = tmpVal;                  // Get the integer (678).
	float tmpFrac = tmpVal - tmpInt1;      // Get fraction (0.0123).
	int tmpInt2 = tmpFrac * 10000;  // Turn into integer (123).

	// Print as parts, note that you need 0-padding for fractional bit.
	sprintf (str, "%s%d.%04d", tmpSign, tmpInt1, tmpInt2);
}

/*
serial_in - Read a byte from the USART0 and return it
*/
char serial_in () {
	while ( !( UCSR0A & (1 << RXC0 )) );
	return UDR0 ;
}

int main(void)
{

	serial_init(UBRR);
	i2c_init (BDIV);
	char outBuf[64];
	char* str = "hello world";
	float f = 10.5;

	unsigned char addr = 0x0D;
	unsigned char rbuf [1];
	i2c_io(0x1D, &addr, 1, NULL, 0, rbuf, 2);


	char floatBuf[6];
	float_to_str(f, floatBuf);


	snprintf(outBuf, 30, "%s, %X, %s\n\r", str, rbuf[0], floatBuf);

	while(1) {
		serial_in();
		serial_out(outBuf, 30);
		
	}



    return 0;   /* never reached */
}
