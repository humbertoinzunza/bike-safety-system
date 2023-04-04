#include "serial.h"
#include "gps.h"
#include <stdlib.h>

unsigned char read_sentence(unsigned char *buffer, unsigned char max_length) {
    unsigned char last_chars[2] = { 0, 0 };
    unsigned short bytes_read = 0;

    while (last_chars[0] != '\r' && last_chars[1] != '\n' && max_length > 0) {
        buffer[bytes_read] = serial_in();
        last_chars[0] = last_chars[1];
        last_chars[1] = buffer[bytes_read];
        bytes_read++;
        max_length--;
    }
    return bytes_read;
}

unsigned char parse_sentence(unsigned char *buffer, short *response)
{
    unsigned char index = 0;
    unsigned char state = 0;
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