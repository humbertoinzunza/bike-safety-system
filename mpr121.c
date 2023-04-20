#include "mpr121.h"
#include "i2c.h"
#include "serial_debug.h"

int calib_e0 = 0;
int calib_e1 = 0;

void mpr_init() {
	unsigned char addr = 0x5D;
	unsigned char rbuf [1];

	//Since there is no device ID or whoami register, 
	//read CDT config register that has constant value 0x24
	//as sanity check that the device is connected properly 
	//and communicates
	i2c_io((0x5A << 1), &addr, 1, NULL, 0, rbuf, 1);

	char outBuf[64];
	if (rbuf[0] != 0x24) {
		char* str = "Failed to init mpr121. Value Read: ";
		snprintf(outBuf, 64, "%s, %X\n\r", str, rbuf[0]);
		serial_out(outBuf, 64);
	}
	else {
		char* str = "mpr init success.";
		snprintf(outBuf, 64, "%s\n\r", str);
		serial_out(outBuf, 64);
	}

	//Write to Electrode Config register to start taking samples
	addr = 0x5E;
	unsigned char wbuf[1] = {0x05};
	i2c_io((0x5A << 1), &addr, 1, wbuf, 1, NULL, 0);
}


void mpr_read_raw (int* e0, int* e1) {

	//Read filtered values for electrodes 0 and 1
	unsigned char addr = 0x04;

	unsigned char rbuf [4];

	i2c_io((0x5A << 1), &addr, 1, NULL, 0, rbuf, 4);

	*e0 = rbuf[1] << 8 | rbuf[0];
	*e1 = rbuf[3] << 8 | rbuf[2];

}

// Couldn't get on-chip calibration working, so we'll calibrate locally
void mpr_calib (int n_samples) {
	
	int e0_sum = 0;
	int e1_sum = 0;


	for (int i = 0; i < n_samples; i++) {
		int e0, e1;
		mpr_read_raw(&e0, &e1);

		e0_sum += e0;
		e1_sum += e1;
	}

	calib_e0 = e0_sum / n_samples;
	calib_e1 = e1_sum / n_samples;

}

// Results are  boolean values per electrode
void mpr_get_touched (_Bool * e0_touched, _Bool * e1_touched) {
	int e0, e1;
	mpr_read_raw(&e0, &e1);

	e0 -= calib_e0;
	e1 -= calib_e1;

	*e0_touched = 0;
	*e1_touched = 0;

	// char outBuf[64];
	// snprintf(outBuf, 64, "E0_raw: %d, E1_raw %d\n\r", e0, e1);
	// serial_out(outBuf, 64);


	if (e0 < -TOUCH_THRESH)
		*e0_touched = 1;

	if (e1 < -TOUCH_THRESH)
		*e1_touched = 1;
}















