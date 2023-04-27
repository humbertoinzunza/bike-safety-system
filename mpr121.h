#ifndef MPR121_H
#define MPR121_H

#define TOUCH_THRESH 100

void mpr_init();
void mpr_read_raw (unsigned short*, unsigned short*);
void mpr_calib (char);
void mpr_get_touched(unsigned char *, unsigned char *, unsigned short *, unsigned short *);

#endif