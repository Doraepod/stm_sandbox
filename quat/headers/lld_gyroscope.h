#ifndef GYROSCOPE_H_
#define GYROSCOPE_H_

#include "i2c.h"
#include <math.h>

#define GYRO_ADDR       0x6B    // I2c address of gyroscope
#define GYRO_CTRL1_REG  0xA0    // Address of control register with autoincrement bit set
#define GYRO_DATA_REG   0xA8    // Address of OUT_X_L register with autoincrement bit set
#define GYRO_INT_PERIOD 5       // Gyroscope integration step [ms]
#define GYRO_INIT_TIME  100     // Time needed to initialise gyroscope [ms]

typedef enum {
    GYRO_AXIS_X,
    GYRO_AXIS_Y,
    GYRO_AXIS_Z
} gyroAxis_t;


/**
 * @brief   Read gyroscope axis values in XYZ order
 * @param
 *          *axis_values         Array where to store read data
 */
msg_t readGyroscope(int16_t *axis_values);


/**
 * @brief   Read gyroscope and represent values as int16
 * @param
 *          *axis_speed         Array where to store axial rotational speed values
 */
msg_t readGyroSpeed(float *axis_speed);


/**
 * @brief   Calculate static error
 * @details Calculates average value of each axis data over ten readings
 * @param
 *          *axis_speed         Array where to store static error values
 */
msg_t calculateGyroError(float *buf);


/**
 * @brief   Get current angle value, [deg]
 * @param
 *          axis        Number of axis to return. 0 through 2 for XYZ
 */
float getGyroAngle(gyroAxis_t axis);


/**
 * @brief   Get current angular speed value, [deg/s]
 * @param
 *          axis        Number of axis to return. 0 through 2 for XYZ
 */
float getGyroSpeed(gyroAxis_t axis);


/**
 * @brief   Initialize gyroscope
 * @args
 *          priopity - NORMALPRIO +/- numbers
 */
void gyroscopeInit( tprio_t priority );


#endif /* GYROSCOPE_H_ */
