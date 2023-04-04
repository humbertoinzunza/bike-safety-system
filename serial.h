/*
    serial_init - Initialize the USART port at a given baud rate.
*/
void serial_init(unsigned short);

/*
    serial_out_byte - Output a byte to the USART0 port.
*/
void serial_out_byte(char);

/*
    serial_out_array - Output an array of bytes to the USART0 port.
*/
void serial_out_array(char *, unsigned short);

/*
    serial_in - Read a byte from the USART0 and return it
*/
char serial_in();