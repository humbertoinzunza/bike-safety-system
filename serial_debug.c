#include <avr/io.h>
#include <stdio.h>
#include "serial_debug.h"

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

void float_to_str(float f, char* str, int len) {
	char *tmpSign = (f < 0) ? "-" : "";
	float tmpVal = (f < 0) ? -f : f;

	int tmpInt1 = tmpVal;                  // Get the integer (678).
	float tmpFrac = tmpVal - tmpInt1;      // Get fraction (0.0123).
	int tmpInt2 = tmpFrac * 10000;  // Turn into integer (123).

	// Print as parts, note that you need 0-padding for fractional bit.
	snprintf (str,len,"%s%d.%04d", tmpSign, tmpInt1, tmpInt2);
}

/*
serial_in - Read a byte from the USART0 and return it
*/
char serial_in () {
	while ( !( UCSR0A & (1 << RXC0 )) );
	return UDR0 ;
}