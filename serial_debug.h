#ifndef SERIAL_DEBUG_H
#define SERIAL_DEBUG_H

#define UBRR 47

void serial_init ( unsigned short ubrr );
void serial_out_char ( char ch );
void serial_out (char * word, int len);
void float_to_str(float f, char* str, int len);
char serial_in ();


#endif
