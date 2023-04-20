#include <avr/io.h>
#include <serial.h>

/*
    serial_init - Initialize the USART port at a given baud rate.
*/
void serial_init(unsigned short baud_rate)
{
    unsigned short ubrr = (CLOCKRATE/16)/115200 - 1;
    UBRR0 = ubrr;           // Set baud rate
    UCSR0B |= (1 << TXEN0); // Turn on transmitter
    UCSR0B |= (1 << RXEN0); // Turn on receiver
    UCSR0C = (3 << UCSZ00); // Set for asynchronous operation, no parity, one stop bit, 8 data bits
}

/*
    serial_out - Output a byte to the USART0 port.
*/
void serial_out(char character) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = character;
}

/*
serial_in - Read a byte from the USART0 and return it
*/
char serial_in()
{
    while (!( UCSR0A & (1 << RXC0)));
    return UDR0;
}