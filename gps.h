#ifndef GPS_H
#define GPS_H

void get_sentence(char *, char *);
unsigned char parse_gga(char *);
unsigned char parse_vtg(char *, unsigned short *, unsigned short *);
unsigned char get_coordinates(char *, char *);
void atoui16(char *, unsigned short *);

#endif