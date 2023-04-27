#include "serial.h"
#include "gps.h"
#include <avr/io.h>

void get_sentence(char *buffer, char *type)
{
    char sentence_type[6] = {'$', 'G', 'P', type[0], type[1], type[2] };
    unsigned char bytes_read;
    
    for (bytes_read = 0; bytes_read < 6; bytes_read++)
        buffer[bytes_read] = 0;
    bytes_read = 0;    
    
    while (buffer[0] != sentence_type[0] || buffer[1] != sentence_type[1] || buffer[2] != sentence_type[2]
        || buffer[3] != sentence_type[3] || buffer[4] != sentence_type[4] || buffer[5] != sentence_type[5]) {
        
        for (bytes_read = 0; bytes_read < 5; bytes_read++)
            buffer[bytes_read] = buffer[bytes_read + 1];
        buffer[bytes_read++] = serial_in();
    }

    while (buffer[bytes_read - 1] != '\r' && buffer[bytes_read] != '\n') {
        buffer[bytes_read++] = serial_in();
    }
    buffer[bytes_read - 2] = '\0';
}

unsigned char parse_vtg(char *buffer, unsigned short *knots, unsigned short *kmh) {
    unsigned char commas_found = 0;
    char *ptr = buffer;
    // Read up to the speed in knots
    while (commas_found < 5) {
        if (*ptr++ == ',') commas_found++;
    }
    char temp_str[4] = { 0, 0, 0, 0 };
    char *temp_ptr = temp_str;
    unsigned char bytes_read = 0;
    // Read the integer part of the speed in knots
    while (*ptr != '.') {
        *temp_ptr++ = *ptr++;
        bytes_read++;
    }
    if (bytes_read == 0) return 0;
    *temp_ptr = '\0';
    // Turn the integer part of the speed to uint16
    atoui16(temp_str, knots);
    // Discard the '.'
    ptr++;
    // Read the decimal part of the speed in knots
    temp_ptr = temp_str;
    bytes_read = 0;
    while(*ptr != ',') {
        *temp_ptr++ = *ptr++;
        bytes_read++;
    }
    if (!bytes_read) return 0;
    *temp_ptr = '\0';
    // Turn the decimal part of the speed to uint16
    atoui16(temp_str, &knots[1]);
    // Discard the ',N,'
    ptr += 3;
    
    temp_ptr = temp_str;
    // Read the integer part of the speed in km/h
    bytes_read = 0;
    while (*ptr != '.') {
        *temp_ptr++ = *ptr++;
        bytes_read++;
    }
    if (!bytes_read) return 0;
    *temp_ptr = '\0';
    // Turn the integer part of the speed to uint16
    atoui16(temp_str, kmh);
    // Discard the '.'
    ptr++;
    // Read the decimal part of the speed in km/h
    temp_ptr = temp_str;
    bytes_read = 0;
    while(*ptr != ',') {
        *temp_ptr++ = *ptr++;
        bytes_read++;
    }
    if (!bytes_read) return 0;
    *temp_ptr = '\0';
    // Turn the decimal part of the speed to uint16
    atoui16(temp_str, &kmh[1]);
    return 1;
}

/*
unsigned char parse_gga(char *buffer)
{
    short lat_degs = 0;
    short lat_mins_int = 0;
    short lat_mins_dec = 0;
    short lon_degs = 0;
    short lon_mins_int = 0;
    short lon_mins_dec = 0;

    unsigned char commas_found = 0;
    unsigned char chars_processed = 0;

    while (commas_found < 2) {
        // Count the commas found
        commas_found += buffer[chars_processed] == ',';
        chars_processed++;
    }
    
    char temp[6];
    unsigned char temp_read = 0;
    // Get the ddmm part in the NMEA sentence
    while (buffer[chars_processed] != '.') {
        // If we find an unexpected comma, return 0, indicating that we don't have data,
        // probably because we didn't get a fix
        if (buffer[chars_processed] == ',')
            return 0;
        temp[temp_read++] = buffer[chars_processed++];
    }
    // Discard period
    chars_processed++;

    // Add a termination character needed for atosh
    temp[temp_read] = '\0';

    // Get the degrees of the latitude and the integer part of the minutes of the latitude
    atoui16(temp, &lat_degs);
    lat_mins_int = lat_degs % 100;
    lat_degs /= 100;

    // Now get the decimal part of the minutes of the latitude
    temp_read = 0;
    
    while (buffer[chars_processed] != ',') {
        temp[temp_read++] = buffer[chars_processed++];
    }
    // Discard comma
    chars_processed++;

    // Add a termination character needed for atosh
    temp[temp_read] = '\0';

    // Get the decimal part of the minutes of the latitude as a short
    atoui16(temp, &lat_mins_dec);

    // If its latitude South, make the latitude negative
    if (buffer[chars_processed++] == 'S') lat_degs *= -1;

    // Ignore the next comma
    chars_processed++;

    temp_read = 0;
    // Get the dddmm part in the NMEA sentence for the longitude
    while (buffer[chars_processed] != '.') {
        // If we find an unexpected comma, return 0, indicating that we don't have data,
        // probably because we didn't get a fix
        if (buffer[chars_processed] == ',')
            return 0;
        temp[temp_read++] = buffer[chars_processed++];
    }

    // Discard period
    chars_processed++;

    // Add a termination character needed for atosh
    temp[temp_read] = '\0';

    // Get the degrees of the longitude and the integer part of the minutes of the longitude
    atoui16(temp, &lon_degs);
    lon_mins_int = lon_degs % 100;
    lon_degs /= 100;

    // Now get the decimal part of the minutes of the longitude
    temp_read = 0;
    
    while (buffer[chars_processed] != ',') {
        temp[temp_read++] = buffer[chars_processed++];
    }
    // Discard comma
    chars_processed++;

    // Add a termination character needed for atosh
    temp[temp_read] = '\0';

    // Get the decimal part of the minutes of the longitude as a short
    atoui16(temp, &lon_mins_dec);

    // If its latitude South, make the latitude negative
    if (buffer[chars_processed++] == 'W') lon_degs *= -1;

    return 1;
}
*/

unsigned char get_coordinates(char *sentence, char *coordinates) {
    unsigned char n_commas = 0;
    char *ptr = sentence;
    // Discard characters until two commas have been found
    while (n_commas < 2) {
        if (*ptr++ == ',') n_commas++;
    }
    char latitude[9];
    char *temp_ptr = latitude;
    unsigned char bytes_read = 0;
    // Get the 9 characters of the latitude
    while (*ptr != ',') {
        *temp_ptr++ = *ptr++;
        bytes_read++;
    }
    if (bytes_read == 0) return 0;
    char ns = 0;
    // Discard the comma
    ptr++;
    if (*ptr == ',') return 0;
    // Get N or S
    ns = *ptr++;
    // Discard the comma
    ptr++;
    char longitude[10];
    temp_ptr = longitude;
    bytes_read = 0;
    // Get the 10 characters of the latitude
    while (*ptr != ',') {
        *temp_ptr++ = *ptr++;
        bytes_read++;
    }
    if (bytes_read == 0) return 0;
    char ew = 0;
    // Discard the comma
    ptr++;
    if (*ptr == ',') return 0;
    // Get E or W
    ew = *ptr;
    // Create the coordinates sentence in the coordinates buffer
    ptr = coordinates;
    temp_ptr = latitude;
    char *ptr_end = latitude + 9;
    // Copy the degrees of the latitude to the buffer
    *ptr++ = *temp_ptr++;
    *ptr++ = *temp_ptr++;
    // Add degree symbol
    *ptr++ = ' ';
    // Add the minutes of the latitude to the buffer
    while (temp_ptr < ptr_end)
        *ptr++ = *temp_ptr++;
    *ptr++ = '\'';
    // Add N or S
    *ptr++ = ns;
    *ptr++ = ' ';
    
    temp_ptr = longitude;
    ptr_end = longitude + 10;
    // Copy the degrees of the longitude to the buffer
    *ptr++ = *temp_ptr++;
    *ptr++ = *temp_ptr++;
    *ptr++ = *temp_ptr++;
    // Add degree symbol
    *ptr++ = ' ';
    // Add the minutes of the latitude to the buffer
    while (temp_ptr < ptr_end)
        *ptr++ = *temp_ptr++;
    *ptr++ = '\'';
    // Add E or W
    *ptr++ = ew;

    // Ending character
    *ptr = '\0';

    return 1;
}

void atoui16(char *string, unsigned short *value) {
    char *i;
    for(i = string; *i != '\0'; i++) {
        *value = *value * 10 + (*i - '0');
    }
}