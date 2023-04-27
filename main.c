#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "i2c.h"
#include "serial.h"
#include "mma8451.h"
#include "mpr121.h"
#include "SIM.h"
#include "states.h"
#include "lcd.h"
#include "gps.h"

#define FOSC 7372800                        // Clock frequency = Oscillator freq.
#define BAUD 9600              // Baud rate used by the LCD
#define MYUBRR FOSC/16/BAUD-1   // Value for UBRR0 register
#define SIM_UBRR 3 // Baud rate of 115200 by default
#define BDIV (FOSC / 50000 - 16) / 2 + 1   // Puts I2C rate just below 50kHz
// Circumference of a 700c bike wheel in miles
#define MILES_PER_REV 0.001366466620877824f
#define WEIGHT_LB 160


#define FALL_THRESH 1000
#define SEV_FALL_THRESH 2000

struct LCD lcd;
float speed = 0.0f;
float distance = 0.0f;

volatile unsigned char rps = 0; // Revolutions per second
volatile unsigned char prev_rps = 0; // Revolutions per second recorded in the previous interval
volatile unsigned char calculate_distance;

void print_speed_distance();

void setup()
{
	// Disable global interrupts
	cli();
	// Initialize I2C bus, LCD, and serial interface
	i2c_init(BDIV);
	lcd_init(&lcd);
	set_contrast(&lcd, 50);

	// Initialize the sim module at 115200 bps
	serial_init(3);
	set_cursor(&lcd, 1, 0);
	print_string(&lcd, "   Initializing   ");
	set_cursor(&lcd, 2, 0);
	print_string(&lcd, "    device....    ");
	_delay_ms(10000);
	// Change the sim module baud rate to 9600 bps
	sim_init();
	// Change the system's baud rate to 9600 bps
	serial_init(MYUBRR);
	// Initialize the sensors
	mma_init();
	mpr_init();
	mma_calibrate(8);
	mpr_calib(16);
	// Set the LED port as output
	DDRC |= 1 << DDC0;
	// Set LED to off
	PORTC &= ~_BV(PC0);
	
	// Set PORTD bit 2 for input
	DDRD &= ~_BV(DDD2);
	// Pin Change Interrupt enable on PCINT2 (Enables PCMSK2 scan)
	PCICR |= _BV(PCIE2);
	// Enable scan for PCINT18 (PD2)
	PCMSK2 |= _BV(PCINT18);
	
	//set timer1 interrupt at 1Hz
	TCCR1A = 0; // Set entire TCCR1A register to 0
	TCCR1B = 0; // Same for TCCR1B
	TCNT1  = 0; // Initialize counter value to 0
	// Set compare match register for 1Hz increments
	OCR1A = 7199; // = (7372970) / (1*1024) - 1 (must be <65536)
	// Turn on CTC mode
	TCCR1B |= (1 << WGM12);
	// Set CS10 and CS12 bits for 1024 prescaler
	TCCR1B |= (1 << CS12) | (1 << CS10);  
	// Enable timer compare interrupt
	TIMSK1 |= (1 << OCIE1A);
	clear_screen(&lcd);
	// Enable global interrupts
	sei();
}

void print_speed_distance() {
	char buffer[21];
	// If the distance is ready to be calculated
	// This occurs regardless of the state
	if (calculate_distance) {
		speed = prev_rps * MILES_PER_REV;
		distance += speed;
		speed *= 3600;
		calculate_distance = 0;
		//clear_screen(&lcd);
		sprintf(buffer, "Speed: %.2f mph", speed);
		set_cursor(&lcd, 0, 0);
		print_string(&lcd, buffer);
		sprintf(buffer, "Traveled: %.2f mi", distance);
		set_cursor(&lcd, 1, 0);
		print_string(&lcd, buffer);
		/*
		hours = seconds / 3600;
		// The part in parentheses is a formula found by doing linear regression done on data found in
		// https://sites.google.com/site/compendiumofphysicalactivities/Activity-Categories/bicycling
		// to calculate METs and use the formula found at
		// https://runbundle.com/tools/cycling/cycling-calorie-calculator
		// (weight_kg) * (METs) * (duration in hours = 5/3600 = 1/720)
		float mets = (distance / hours) * 0.8439 - 2.056;
		if (mets > 0.0f)
			calories += weight_kg *  mets * hours;
		sprintf(buffer, "Cals burned: %.2f", calories);
		set_cursor(&lcd, 2, 0);
		print_string(&lcd, buffer);
		seconds++;
		*/
	}
}

int main(void)
{
	// Setup
	setup();
	// Init state machine
	enum states curr_state = IDLE;
	/*
	const float weight_kg = WEIGHT_LB * 0.453592;
	float calories = 0.0f;
	float hours = 0.0f;
	unsigned short seconds = 1;
	*/

	while (1) {

		if (curr_state == IDLE) {
			print_speed_distance();
			//Turn off LED when idling
			PORTC &= ~_BV(PC0);

			//Wait until handlebars are touched
			unsigned char e0_touched, e1_touched;
			unsigned short v0, v1;

			mpr_get_touched(&e0_touched, &e1_touched, &v0, &v1);

			if (e0_touched && e1_touched) {
				curr_state = MONITORING;
				//char buf[21];
				//set_cursor(&lcd, 3, 0);
				//sprintf(buf, "%hu %hu", v0, v1);
				//print_string(&lcd, buf);
			}

		}

		else if (curr_state == MONITORING) {
			print_speed_distance();
			// Turn on LED when monitoring
			PORTC |= _BV(PC0);

			int x, y, z;
			mma_read_calibrated(&x, &y, &z);

			unsigned char e0_touched, e1_touched;
			unsigned short v0, v1;

			mpr_get_touched(&e0_touched, &e1_touched, &v0, &v1);

			unsigned char fall_detected = 0;
			unsigned char sev_fall_detected = 0;

			if (abs(x) > FALL_THRESH || abs(y) > FALL_THRESH || abs(z - 4096) > FALL_THRESH)
				fall_detected = 1;

			if (abs(x) > SEV_FALL_THRESH || abs(y) > SEV_FALL_THRESH || abs(z - 4096) > SEV_FALL_THRESH)
				sev_fall_detected = 1;

			// If either handlebar is not touched
			if (!e0_touched || !e1_touched) {
				if (sev_fall_detected)
					curr_state = SEND_SEVERE_FALL;
				else if (fall_detected)
					curr_state = SEND_FALL;
			}
		}

		else if (curr_state == SEND_FALL) {
			char buffer[100];
			char coordinates[30];
			get_sentence(buffer, "GGA");
			clear_screen(&lcd);
			set_cursor(&lcd, 0, 0);
			print_string(&lcd, "Fall detected");
			while(!get_coordinates(buffer, coordinates)) {
				
				set_cursor(&lcd, 2, 0);
				print_string(&lcd, "Attempting to get GPS data...");
				get_sentence(buffer, "GGA");
			}
			char msg[45];
			sprintf(msg, "Fall detected at %s\x1A", coordinates);
			sim_send(msg, 44);
			curr_state = FALL;
			clear_screen(&lcd);
			set_cursor(&lcd, 0, 0);
			print_string(&lcd, "Fall detected");
			set_cursor(&lcd, 1, 0);
			print_string(&lcd, "Message sent");
		}

		else if (curr_state == SEND_SEVERE_FALL) {
			char buffer[100];
			char coordinates[30];
			get_sentence(buffer, "GGA");
			clear_screen(&lcd);
			set_cursor(&lcd, 0, 0);
			print_string(&lcd, "Severe fall detected");
			while(!get_coordinates(buffer, coordinates)) {
				set_cursor(&lcd, 2, 0);
				print_string(&lcd, "Attempting to get GPS data...");
				get_sentence(buffer, "GGA");
			}
			char msg[52];
			sprintf(msg, "Severe fall detected at %s\x1A", coordinates);
			sim_send(msg, 51);
			curr_state = SEVERE_FALL;
			clear_screen(&lcd);
			set_cursor(&lcd, 0, 0);
			print_string(&lcd, "Severe fall detected");
			set_cursor(&lcd, 1, 0);
			print_string(&lcd, "Message sent");
		}

		else if (curr_state == FALL) {
			PORTC |= _BV(PC0);
			_delay_ms(500);
			PORTC &= ~_BV(PC0);
			_delay_ms(400);

		}

		else if (curr_state == SEVERE_FALL) {
			//Blink at 2 HZ for a severe fall
			PORTC |= _BV(PC0);
			_delay_ms(250);
			PORTC &= ~_BV(PC0);
			_delay_ms(150);
		}
	}
	// Never reached
    return 0;
}

ISR(PCINT2_vect) {
  if (PIND & _BV(PIND2))
    rps++;
}

ISR(TIMER1_COMPA_vect) {
  cli();
  prev_rps = rps;
  rps = 0;
  calculate_distance = 1;
  sei();
}