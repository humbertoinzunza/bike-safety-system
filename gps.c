#include "serial.h"
#include "gps.h"

void read_sentence(unsigned char *buffer, unsigned char max_length) {
    unsigned char last_chars[2] = { 0, 0 };
    unsigned char bytes_read = 0;

    while (last_chars[0] != '\r' && last_chars[1] != '\n' && max_length > 0) {
        buffer[bytes_read] = serial_in();
        last_chars[0] = last_chars[1];
        last_chars[1] = buffer[bytes_read];
        bytes_read++;
        max_length--;
    }
}