#include <lld_gyroscope.h>


float                   gyro_angle_xyz[3]   = {0, 0, 0};
float                   angular_speed[3]    = {0, 0, 0};
float                   gyro_mean_error[3]  = {0, 0, 0};


/*
 * @brief   Gyroscope thread.
 *          Read gyroscope data and integrate it to get angle value
 */
static THD_WORKING_AREA(gyroThread, 256); // 256 - stack size
static THD_FUNCTION(Gyro, arg)
{
    arg = arg;              // just to avoid warnings
    systime_t time = chVTGetSystemTime();
    while( true )
    {
        uint8_t i;
        msg_t msg = readGyroSpeed(angular_speed);

        if( msg == MSG_OK )
        {
            for(i = 0; i < 3; i++)
            {
                angular_speed[i] -= gyro_mean_error[i];

                if (fabs(angular_speed[i]) < 0.01)
                    angular_speed[i] = 0;

                gyro_angle_xyz[i] += angular_speed[i] * GYRO_INT_PERIOD / 1000;

                if (gyro_angle_xyz[i] > 360)   // Keep angle in range [0 360]
                    gyro_angle_xyz[i] -= 360;
                else
                    {
                    if (gyro_angle_xyz[i] < 0)
                        gyro_angle_xyz[i] += 360;
                    }
            }
        }
        time = chThdSleepUntilWindowed( time, time + TIME_MS2I( GYRO_INT_PERIOD ) );
    }
}


static bool             isInitialized       = false;

/**
 * @brief   Initialize gyroscope
 * @args
 *          priority - NORMALPRIO +/- numbers
 */
void gyroscopeInit( tprio_t priority )
{
    if(isInitialized)
        return;

    i2cStartUp();

    uint8_t initbuf[5];
    initbuf[0] = GYRO_CTRL1_REG;            // Start from ctrl1 register
    initbuf[1] = 0x0F;                      // Normal mode, each axis enable, freq settings ignored
    initbuf[2] = 0x00;                      // High pass filter default configuration
    initbuf[3] = 0x00;                      // Too smart for me
    initbuf[4] = 0x30;                      // 2000dps
    
    i2cSimpleWrite(GYRO_ADDR, initbuf, 5, 1000);

    chThdSleepMilliseconds(GYRO_INIT_TIME);

    calculateGyroError(gyro_mean_error);

    chThdCreateStatic(gyroThread, sizeof(gyroThread), priority, Gyro, NULL);

    isInitialized = true;
}


/**
 * @brief   Get current angle value, [deg]
 *
 * @param
 *          axis        Number of axis to return. 0 through 2 for XYZ
 */
float getGyroAngle(gyroAxis_t axis)
{
    return gyro_angle_xyz[axis];
}


/**
 * @brief   Get current angular speed value, [deg]
 *
 * @param
 *          axis        Number of axis to return. 0 through 2 for XYZ
 */
float getGyroSpeed(gyroAxis_t axis)
{
    if(axis < 3)
        return angular_speed[axis];
    return -1;
}


/**
 * @brief   Read gyroscope axis values in XYZ order
 *
 * @param
 *          *axis_values         Array where to store read data
 */
msg_t readGyroscope(int16_t *axis_values)
{
    uint8_t gyro_temp[6] = {0, 0, 0, 0, 0, 0};
    msg_t msg = i2cRegisterRead(GYRO_ADDR, GYRO_DATA_REG, gyro_temp, 6, 10000);
    
    uint8_t i = 0;
    for(i = 0; i < 3; i++)
    {
        axis_values[i] = (int16_t)((uint16_t)(gyro_temp[i * 2]) | ((uint16_t)(gyro_temp[(i * 2) + 1]) << 8));
    }
    return msg;
}

/**
 * @brief   Read gyroscope and represent values as int16
 *
 * @param
 *          *axis_speed         Array where to store axial rotational speed values
 */
msg_t readGyroSpeed(float *axis_speed)
{
    int16_t gyro_axis_values[3] = {0, 0, 0};
    uint8_t i = 0;
    msg_t msg = readGyroscope(gyro_axis_values);
    if (msg == MSG_OK)
        for(i = 0; i < 3; i++)
        {
            axis_speed[i] = (float)gyro_axis_values[i] * 0.07;
        }
    return msg;
}


/**
 * @brief   Calculate static error
 * @details Calculates average value of each axis data over ten readings
 *
 * @param
 *          *axis_speed         Array where to store static error values
 */
msg_t calculateGyroError(float *buf)
{
    msg_t msg = MSG_OK;
    int16_t temp_buf[10][3] = {{0}};
    int16_t super_temp[3] = {0, 0, 0};
    uint8_t i, j;
    float x_mean = 0, y_mean = 0, z_mean = 0;
    for(i = 0; i < 10; i++)
    {
        msg = readGyroscope(super_temp);
        if (msg == MSG_OK)
            for(j = 0; j < 3; j++)
            {
                temp_buf[i][j] = super_temp[j];
            }
    }

    for(i = 0; i < 10; i++)
    {
        x_mean += temp_buf[i][0];
        y_mean += temp_buf[i][1];
        z_mean += temp_buf[i][2];
    }

    buf[0] = (x_mean / 10) * 0.07;
    buf[1] = (y_mean / 10) * 0.07;
    buf[2] = (z_mean / 10) * 0.07;

    return msg;
}
