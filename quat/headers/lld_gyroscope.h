#ifndef GYROSCOPE_H_
#define GYROSCOPE_H_

#include "i2c.h"

#define GYRO_ADDR       0x6B    // I2c address of gyroscope
#define GYRO_CTRL1_REG  0xA0    // Address of control register with autoincrement bit set
#define GYRO_DATA_REG   0xA8    // Address of OUT_X_L register with autoincrement bit set
#define GYRO_INT_PERIOD 5       // Gyroscope integration step [ms]
#define GYRO_INIT_TIME  100     // Time needed to initialise gyroscope [ms]

void gyroInit(void);
msg_t gyroRead(int16_t *axis_values);
msg_t gyroReadSpeed(float *axis_speed);
msg_t gyroCalculateError(void);

#endif /* GYROSCOPE_H_ */
