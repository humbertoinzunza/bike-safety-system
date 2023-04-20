#include "serial.h"
#include "gps.h"
#include <avr/io.h>

void get_gpgga(unsigned char *buffer)
{
    unsigned char sentence_type[6] = "$GPGGA";
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

unsigned char parse_sentence(unsigned char *buffer, short *response)
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
    
    unsigned char temp[6];
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
    lat_degs = atosh(temp);
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
    lat_mins_dec = atosh(temp);

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
    lon_degs = atosh(temp);
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
    lon_mins_dec = atosh(temp);

    // If its latitude South, make the latitude negative
    if (buffer[chars_processed++] == 'W') lon_degs *= -1;

    // Store the response
    response[0] = lat_degs;
    response[1] = lat_mins_int;
    response[2] = lat_mins_dec;
    response[3] = lon_degs;
    response[4] = lon_mins_int;
    response[5] = lon_mins_dec;

    return 1;
}

short atosh(unsigned char *string)
{
    short res = 0;
    unsigned char i = 0;
    for(i = 0; string[i] != '\0'; i++) {
        res = res * 10 + string[i] - '0';
    }   

    return res;
}