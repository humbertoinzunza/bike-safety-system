#include <avr/io.h>
#include <stdio.h>
#include "i2c.h"
#include "serial_debug.h"
#include "mma8451.h"
#include "mpr121.h"
#include <util/delay.h>
#include "SIM.h"
#include "states.h"
#include <stdlib.h>

#define FALL_THRESH 1000
#define SEV_FALL_THRESH 2000


int main(void)
{

	sim_init();

	mma_init();
	mpr_init();
	mma_calibrate(8);
	mpr_calib(16);
	DDRC |= 1 << DDC0;

	


	// Init state machine
	enum states curr_state = IDLE;


	while (1) {

		if (curr_state == IDLE) {
			//Turn off LED when idling
			PORTC &= ~(1 << PC0);

			//Wait until handlebars are touched
			_Bool e0_touched, e1_touched;

			mpr_get_touched(&e0_touched, &e1_touched);

			if (e0_touched && e1_touched) {
				curr_state = MONITORING;
			}

		}

		else if (curr_state == MONITORING) {

			// Turn on LED when monitoring
			PORTC |= 1 << PC0;

			int x, y, z;
			mma_read_calibrated(&x, &y, &z);

			_Bool e0_touched, e1_touched;

			mpr_get_touched(&e0_touched, &e1_touched);

			_Bool fall_detected = 0;
			_Bool sev_fall_detected = 0;

			if (abs(x) > FALL_THRESH || abs(y) > FALL_THRESH || abs(z - 4096) > FALL_THRESH) {
				fall_detected = 1;
			}

			if (abs(x) > SEV_FALL_THRESH || abs(y) > SEV_FALL_THRESH || abs(z - 4096) > SEV_FALL_THRESH) {
				sev_fall_detected = 1;
			}

			// If either handlebar is not touched
			if (!e0_touched || !e1_touched) {
				if (sev_fall_detected) {
					curr_state = SEND_SEVERE_FALL;

				} else if (fall_detected) {
					curr_state = SEND_FALL;
				}
			}

			char * msg = "No Fall Detected!\x1A";
			sim_send(msg, 18);

		}

		else if (curr_state == SEND_FALL) {
			char * msg = "Fall Detected!\x1A";
			sim_send(msg, 15);
			curr_state = FALL;

		}

		else if (curr_state == SEND_SEVERE_FALL) {
			char * msg = "Severe Fall Detected!\x1A";
			sim_send(msg, 22);
			curr_state = SEVERE_FALL;
		}

		else if (curr_state == FALL) {
			//Blink at 1 HZ for a regular fall
			PORTC |= 1 << PC0;
			_delay_ms(500);
			PORTC &= ~(1 << PC0);
			_delay_ms(400);

		}

		else if (curr_state == SEVERE_FALL) {
			//Blink at 2 HZ for a severe fall
			PORTC |= 1 << PC0;
			_delay_ms(250);
			PORTC &= ~(1 << PC0);
			_delay_ms(150);
		}


		_delay_ms(100);
	}






    return 0;
}
