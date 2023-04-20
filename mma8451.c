#include "mma8451.h"
#include "i2c.h"
#include "serial_debug.h"

int calib_x = 0;
int calib_y = 0;
int calib_z = 0;

void mma_init(){

	unsigned char addr = 0x0D;
	unsigned char rbuf [1];
	i2c_io((0x1D << 1), &addr, 1, NULL, 0, rbuf, 1);

	char outBuf[64];
	if (rbuf[0] != 0x1A) {
		char* str = "Failed to init mma8541. ID Read: ";
		snprintf(outBuf, 64, "%s, %X\n\r", str, rbuf[0]);
		serial_out(outBuf, 64);
	}
	else {
		char* str = "mma8541 init success.";
		snprintf(outBuf, 64, "%s\n\r", str);
		serial_out(outBuf, 64);
	}

	addr = 0x2A;
	unsigned char wbuf[1] = {0x19};

	i2c_io((0x1D << 1), &addr, 1, wbuf, 1, NULL, 0);

}

void mma_read_raw(int * x, int * y, int * z){
	unsigned char addr = 0x01;
	unsigned char rbuf [6];
	i2c_io((0x1D << 1), &addr, 1, NULL, 0, rbuf, 6);

	*x = (rbuf[0] << 6) | rbuf[1] >> 2;
	*y = (rbuf[2] << 6) | rbuf[3] >> 2;
	*z = (rbuf[4] << 6) | rbuf[5] >> 2;

	// char outBuf[64];
	// snprintf(outBuf, 64, "raw_x: %X, raw_y: %X, raw_z: %X\n\r", *x, *y, *z);
	// serial_out(outBuf, 64);

	//Wacky stuff to convert from two's complement to signed int
	*x = *x & (1 << 13) ? (0x3 << 14) | *x: *x;
	*y = *y & (1 << 13) ? (0x3 << 14) | *y :*y;
	*z = *z & (1 << 13) ? (0x3 << 14) | *z :*z;

	// char outBuf[64];
	// snprintf(outBuf, 64, "x: %d, y: %d, z: %d\n\r", *x, *y, *z);
	// serial_out(outBuf, 64);

}

void mma_calibrate(int n) {

	int x_sum = 0;
	int y_sum = 0;
	int z_sum = 0;
	for (int i = 0; i < n; i++) {
		int x, y, z;

		mma_read_raw(&x, &y, &z);
		// char outBuf[64];
		// snprintf(outBuf, 64, "raw_x: %X, raw_y: %X, raw_z: %X\n\r", x, y, z);
		// serial_out(outBuf, 64);


		x_sum += x;
		y_sum += y;
		z_sum += z - 4096;

	}

	calib_x = x_sum/n;
	calib_y = y_sum/n;
	calib_z = z_sum/n;

	// char outBuf[64];
	// snprintf(outBuf, 64, "calib_x: %d, calib_y: %d, calib_z: %d\n\r", calib_x, calib_y, calib_z);
	// serial_out(outBuf, 64);

}

void mma_read_calibrated(int * x, int * y, int * z) {
	mma_read_raw(x, y, z);

	*x -= calib_x;
	*y -= calib_y;
	*z -= calib_z;
}