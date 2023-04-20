#ifndef GPS_H
#define GPS_H

void get_gpgga(unsigned char *);
unsigned char parse_sentence(unsigned char *, short *);
short atosh(unsigned char *); 

#endif