#ifndef SIM_H
#define SIM_H

#define SIM_UBRR 3

void sim_init ();

void sim_send (char* msg, int len);

#endif