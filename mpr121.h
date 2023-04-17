#ifndef MPR121_H
#define MPR121_H

#define TOUCH_THRESH 100

void mpr_init();
void mpr_read_raw (int* e0, int* e1);
void mpr_calib (int n_samples);
void mpr_get_touched (_Bool * e0_touched, _Bool * e1_touched);

#endif