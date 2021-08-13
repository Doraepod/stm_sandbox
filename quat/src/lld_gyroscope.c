#include <lld_gyroscope.h>

float gyro_mean_error[3]  = {0, 0, 0};

void gyroInit(void)
{

    uint8_t initbuf[5];
    initbuf[0] = GYRO_CTRL1_REG;            // Start from ctrl1 register
    initbuf[1] = 0x0F;                      // Normal mode, each axis enable, freq settings ignored
    initbuf[2] = 0x00;                      // High pass filter default configuration
    initbuf[3] = 0x00;                      // Too smart for me
    initbuf[4] = 0x30;                      // 2000dps

    i2cSimpleWrite(GYRO_ADDR, initbuf, 5, 1000);

    chThdSleepMilliseconds(GYRO_INIT_TIME);

    gyroCalculateError();
}

/**
 * @brief   Read gyroscope axis values in XYZ order
 *
 * @param
 *          *axis_values         Array where to store read data
 */
msg_t gyroRead(int16_t *axis_values)
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
msg_t gyroReadSpeed(float *axis_speed)
{
    int16_t gyro_axis_values[3] = {0, 0, 0};
    uint8_t i = 0;
    msg_t msg = gyroRead(gyro_axis_values);
    if (msg == MSG_OK)
        for(i = 0; i < 3; i++)
        {
            axis_speed[i] = (float)gyro_axis_values[i] * 0.07 - gyro_mean_error[i];
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
msg_t gyroCalculateError(void)
{
    msg_t msg = MSG_OK;
    int16_t temp_buf[10][3] = {{0}};
    int16_t super_temp[3] = {0, 0, 0};
    uint8_t i, j;
    float x_mean = 0, y_mean = 0, z_mean = 0;
    for(i = 0; i < 10; i++)
    {
        msg = gyroRead(super_temp);
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

    gyro_mean_error[0] = (x_mean / 10) * 0.07;
    gyro_mean_error[1] = (y_mean / 10) * 0.07;
    gyro_mean_error[2] = (z_mean / 10) * 0.07;

    return msg;
}
