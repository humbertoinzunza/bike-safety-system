#include "mpr121.h"
#include "i2c.h"

unsigned short calib_e0 = 0;
unsigned short calib_e1 = 0;

void mpr_init() {
	//Write to Electrode Config register to start taking samples
	unsigned char addr = 0x5E;
	unsigned char wbuf[1] = {0x05};
	i2c_io((0x5A << 1), &addr, 1, wbuf, 1, NULL, 0);
}


void mpr_read_raw (unsigned short *e0, unsigned short *e1) {
	//Read filtered values for electrodes 0 and 1
	unsigned char addr = 0x04;
	unsigned char rbuf [4];
	i2c_io((0x5A << 1), &addr, 1, NULL, 0, rbuf, 4);
	unsigned char *ptr = (unsigned char *)e0;
	*ptr++ = rbuf[0];
	*ptr = rbuf[1];
	ptr = (unsigned char *)e1;
	*ptr++ = rbuf[2];
	*ptr = rbuf[3];
}

// Couldn't get on-chip calibration working, so we'll calibrate locally.
// Up to 64 samples, because the maximum value retrieved is 1024
// and 2^16/1024 = 64.
void mpr_calib (char n_samples) {
	
	unsigned short e0_sum = 0;
	unsigned short e1_sum = 0;


	for (; n_samples > 0; n_samples--) {
		unsigned short e0, e1;
		mpr_read_raw(&e0, &e1);

		e0_sum += e0;
		e1_sum += e1;
	}

	calib_e0 = e0_sum / n_samples;
	calib_e1 = e1_sum / n_samples;

}

// Results are  boolean values per electrode
void mpr_get_touched (unsigned char *e0_touched, unsigned char *e1_touched, unsigned short *v0, unsigned short *v1) {

	unsigned short e0, e1;
	mpr_read_raw(&e0, &e1);
	*e0_touched = e0 < TOUCH_THRESH;	
	*e1_touched = e1 < TOUCH_THRESH;
}
