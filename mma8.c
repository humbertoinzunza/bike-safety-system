#include "mma8451.h"
#include "i2c.h"
#include "serial_debug.h"

void mma_init (){

	unsigned char addr = 0x0D;
	unsigned char rbuf [1];
	rbuf[0] = 0;
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

}