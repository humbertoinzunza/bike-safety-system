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
#include <util/delay.h>


int main(void)
{

	serial_init(UBRR);
	i2c_init (BDIV);
	char outBuf[64];
	// char* str = "hello world";
	// float f = 10.3;

	unsigned char addr = 0x5D;
	unsigned char rbuf [2];
	// rbuf[0] = 0;
	// rbuf[1] = 0;
	i2c_io((0x5A << 1), &addr, 1, NULL, 0, rbuf, 1);


	addr = 0x5E;

	unsigned char wbuf[1] = {0x05};

	i2c_io((0x5A << 1), &addr, 1, wbuf, 1, NULL, 0);

	addr = 0x41;

	//touch threshold

	wbuf[0] = 0x00;

	i2c_io((0x5A << 1), &addr, 1, wbuf, 1, NULL, 0);


	//release threshold

	addr = 0x42;

	wbuf[0] = 10;

	i2c_io((0x5A << 1), &addr, 1, wbuf, 1, NULL, 0);


	// Read filtered value
	addr = 0x04;

	i2c_io((0x5A << 1), &addr, 1, NULL, 0, rbuf, 2);

	int filt_val = rbuf[1] << 8 | rbuf[0];

	//Read baseline value

	addr = 0x1E;

	i2c_io((0x5A << 1), &addr, 1, NULL, 0, rbuf, 1);

	int base_val = rbuf[0];

	// Read touch status

	addr = 0x00;

	i2c_io((0x5A << 1), &addr, 1, NULL, 0, rbuf, 1);

	int touch_status = rbuf[0];
	

	snprintf(outBuf, 64, "T: %X, b: %d, f: %d\n\r", touch_status, base_val, filt_val);
	serial_out(outBuf, 64); 


	// char floatBuf[10];
	// float_to_str(f, floatBuf, 10);
	// unsigned char a = 10;

			// mma_init();
	// mma_init();


	// // int x, y, z;
	

	// // char outBuf[64];

	// mma_calibrate(4);


	while(1) {
		_delay_ms(100);
		

		// Read filtered value
		addr = 0x04;

		i2c_io((0x5A << 1), &addr, 1, NULL, 0, rbuf, 2);

		filt_val = rbuf[1] << 8 | rbuf[0];

		//Read baseline value

		addr = 0x1E;

		i2c_io((0x5A << 1), &addr, 1, NULL, 0, rbuf, 1);

		base_val = rbuf[0];

		// Read touch status

		addr = 0x00;

		i2c_io((0x5A << 1), &addr, 1, NULL, 0, rbuf, 1);

		touch_status = rbuf[0];
		

		snprintf(outBuf, 64, "T: %X, b: %d, f: %d\n\r", touch_status, base_val, filt_val);
		serial_out(outBuf, 64); 
	

		snprintf(outBuf, 64, "Capacitive touch data: %X\n\r", rbuf[0]);
		serial_out(outBuf, 64); 
	}



    return 0;   /* never reached */
}
