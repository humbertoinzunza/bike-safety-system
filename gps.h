#ifndef GPS_H
#define GPS_H

unsigned char read_sentence(unsigned char *, unsigned char);
unsigned char parse_sentence(unsigned char *, short *);

#endif