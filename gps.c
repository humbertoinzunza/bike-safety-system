#include "serial.h"
#include "gps.h"
#include <stdlib.h>
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
    unsigned char state = 0;
    unsigned char index = 0;
    unsigned char first_L[] = "GPGGA";
    short latitude_decimal = 0;
    short latitude_minutes = 0;
    short longitude_decimal = 0;
    short longitude_minutes = 0;

    if(state == 0)
    {
        if(buffer[index++] == '$')
        {
            state = 1;
        }
        else
        {
            response[0] = 10;
            return 1;
        }
    }
    if(state == 1)
    {
        unsigned char i;
        for(i=0; i<5; i++)
        {
            if(buffer[index++] != first_L[i])
            {
                response[0] = 11;
                response[1] = buffer[index - 1];
                response[2] = first_L[i];
                response[3] = index - 1;
                return 1;
            }
        }
        if(buffer[index++] == ',')
        {
            state = 2;
        }
        else
        {
            response[0] = 12;
            return 1;
        }  
    }
    if(state == 2)
    {
        index = index + 10;
        if(buffer[index++] == ',')
        {
            state = 3;
        }
        else
        {
            response[0] = 13;
            return 1;
        }
    }
    if(state == 3)
    {
        unsigned char i;
        unsigned char lat[5];
        for(i = 0; i<4; i++)
        {
            lat[i] = buffer[index++];
        }
        lat[4] = '\0';
        latitude_decimal = atoi(lat);
        if(buffer[index++] != '.')
        {
            response[0] = 14;
            return 1;
        }
        for(i = 0; i<4; i++)
        {
            lat[i] = buffer[index++];
        }
        lat[4] = '\0';
        latitude_minutes = atoi(lat);
        latitude_minutes += (latitude_decimal%100)*1000;
        latitude_decimal /= 100;
        if(buffer[index++] == ',')
        {
            state = 4;
        }
        else
        {
            response[0] = 15;
            return 1;
        }
    }
    if(state == 4)
    {
        if(buffer[index] == 'S')
        {
            latitude_decimal = latitude_decimal*(-1);
        }
        else if(buffer[index] != 'S')
        {
            response[0] = 16;
            return 1;
        }
        if(buffer[index++] == ',')
        {
            state = 5;
        }
        else
        {
            response[0] = 17;
            return 1;
        }
    }
    if(state == 5)
    {
        unsigned char lon[5];
        unsigned char i;
        for(i = 0; i<4; i++)
        {
            lon[i] = buffer[index++];
        }
        lon[4] = '\0';
        longitude_decimal = atoi(lon);
        if(buffer[index++] != '.')
        {
            response[0] = 18;
            return 1;
        }
        for(i = 0; i<4; i++)
        {
            lon[i] = buffer[index++];
        }
        lon[4] = '\0';
        longitude_minutes = atoi(lon);
        longitude_minutes += (longitude_decimal%100)*1000;
        longitude_decimal /= 100;
         if(buffer[index++] == ',')
        {
            state = 6;
        }
        else
        {
            response[0] = 19;
            return 1;
        }
    }
    if(state == 6)
    {
        if(buffer[index] == 'W')
        {
            latitude_decimal = latitude_decimal*(-1);
        }
        else if(buffer[index] != 'E')
        {
            response[0] = 20;
            return 1;
        }
    }
    response[0] = latitude_decimal;
    response[1] = latitude_minutes;
    response[2] = longitude_decimal;
    response[3] = longitude_minutes;

    return 0;
}