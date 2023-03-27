#ifndef I2C_H
#define I2C_H

#include <stdio.h>

void i2c_init(uint8_t);

uint8_t i2c_io(uint8_t, uint8_t *, uint16_t, uint8_t *, uint16_t, uint8_t *, uint16_t);

#endif