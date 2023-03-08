#define CLOCKRATE   7372970

/*
    serial_init - Initialize the USART port at a given baud rate.
*/
void serial_init(unsigned short);

/*
    serial_out - Output a byte to the USART0 port.
*/
void serial_out(char);

/*
    serial_in - Read a byte from the USART0 and return it
*/
char serial_in();