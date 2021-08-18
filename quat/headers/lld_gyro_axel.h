#ifndef HEADERS_LLD_GYRO_AXEL_H_
#define HEADERS_LLD_GYRO_AXEL_H_

#include "i2c.h"
#define GYRO_AXEL_ADR       0x6A    // I2c address of gyroscope
#define GYRO_AXEL_CTRL1_REG  0x10    // Address of control register 1
#define GYRO_DATA_REG   0x22    // Address of OUTX_L_G register
#define AXEL_DATA_REG   0x28    // Address of OUTX_L_G register

void gaInit(void);
msg_t accelRead(int16_t *axis_values);
msg_t gyroRead(int16_t *axis_values);
msg_t gyroReadSpeed(float *axis_speed);
msg_t gyroCalculateError(void);


#endif /* HEADERS_LLD_GYRO_AXEL_H_ */
