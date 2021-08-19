#ifndef HEADERS_LLD_MAGNETOMETER_H_
#define HEADERS_LLD_MAGNETOMETER_H_

#include "i2c.h"

#define MAG_ADR 0b0011110
#define MAG_CONFIG_REG_ADR 0b11100000
#define MAG_DATA_REG 0b11101000

void magInit(void);
msg_t magRead(int16_t* axis_values);

#endif /* HEADERS_LLD_MAGNETOMETER_H_ */
