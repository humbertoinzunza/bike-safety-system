#include "SIM.h"
#include "serial.h"
#include <util/delay.h>
#include <stdio.h>
#define PHONE_NUMBER "+123456789"

void sim_init() {
	char * config_msg = "AT+IPR=9600\r\n";
	serial_out_array(config_msg, 13);
	_delay_ms(500);
}
void sim_send (char* msg, unsigned short len) {

	char * config_msg = "AT+CMGF=1\r\n";
	serial_out_array(config_msg, 11);
	_delay_ms(500);

	sprintf(config_msg, "AT+CMGS=\"%s\"\r\n", PHONE_NUMBER);
	serial_out_array(config_msg, 24);
	_delay_ms(1000);
	serial_out_array(msg, len);
	_delay_ms(500);
}