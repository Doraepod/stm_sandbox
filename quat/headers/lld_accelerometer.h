#ifndef HEADERS_LLD_ACCELEROMETER_H_
#define HEADERS_LLD_ACCELEROMETER_H_

#include "i2c.h"

#define ACCEL_ADDR      0x18    // i2c address of accelerometer
#define ACCEL_CTRL1_REG  0xA0    // address of control register with autoincrement bit set
#define ACCEL_DATA_REG   0xA8    // address of OUT_X_L register with autoincrement bit set

void accelInit(void);
msg_t accelRead(int16_t *axis_values);

#endif /* HEADERS_LLD_ACCELEROMETER_H_ */
