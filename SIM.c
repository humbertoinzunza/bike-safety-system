#include "SIM.h"
#include "serial_debug.h"
#include <util/delay.h>


void sim_init () {
	serial_init(SIM_UBRR);
}

void sim_send (char* msg, int len) {

	char * config_msg = "AT+CMGF=1\r\n";
	serial_out (config_msg, 11);
	// serial_out_char (0x30);
	_delay_ms(500);

	config_msg = "AT+CMGS=\"+16509467854\"\r\n";
	serial_out (config_msg, 24);
	// serial_out_char (0x30);
	_delay_ms(1000);
	serial_out (msg, len);
	_delay_ms(500);
}