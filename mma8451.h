#ifndef MMA8451_H
#define MMA8451_H

int calib_x;
int calib_y;
int calib_z;

void mma_init ();
void mma_read_raw(int * x, int * y, int * z);
void mma_calibrate (char n);
void mma_read_calibrated(int * x, int * y, int * z);


#endif