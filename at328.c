/*************************************************************
*       at328-0.c - Demonstrate simple I/O functions of ATmega328
*
*       Program loops turning PC0 on and off as fast as possible.
*
* The program should generate code in the loop consisting of
*   LOOP:   SBI  PORTC,0        (2 cycles)
*           CBI  PORTC,0        (2 cycles)
*           RJMP LOOP           (2 cycles)
*
* PC0 will be low for 4 / XTAL freq
* PC0 will be high for 2 / XTAL freq
* A 9.8304MHz clock gives a loop period of about 600 nanoseconds.
*
* Revision History
* Date     Author      Description
* 09/14/12 A. Weber    Initial Release
* 11/18/13 A. Weber    Renamed for ATmega328P
*************************************************************/

#include <avr/io.h>
#include <stdio.h>
#include "i2c.h"
#include "serial_debug.h"
#include "mma8451.h"


int main(void)
{

	serial_init(UBRR);
	i2c_init (BDIV);
	// char outBuf[64];
	// char* str = "hello world";
	// float f = 10.3;

	// unsigned char addr = 0x0D;
	// unsigned char rbuf [2];
	// rbuf[0] = 0;
	// rbuf[1] = 0;
	// i2c_io((0x1D << 1), &addr, 1, NULL, 0, rbuf, 1);


	// char floatBuf[10];
	// float_to_str(f, floatBuf, 10);
	// unsigned char a = 10;

			// mma_init();
	mma_init();


	int x, y, z;
	

	char outBuf[64];

	mma_calibrate(4);


	while(1) {
		serial_in();
		mma_read_calibrated(&x, &y, &z);
		snprintf(outBuf, 64, "x: %d, y: %d, z: %d\n\r", x, y, z);
		serial_out(outBuf, 64);
		
	}



    return 0;   /* never reached */
}
