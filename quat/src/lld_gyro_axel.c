#include "lld_gyro_axel.h"

float gyro_mean_error[3]  = {0, 0, 0};

void gaInit(void)
{
    uint8_t ctrl_regs[11];
    ctrl_regs[0] = GYRO_AXEL_CTRL1_REG; //addres of first ctrl register
    ctrl_regs[1] = 0b01010000;  //208Hz, +-2g, no LPF2
    ctrl_regs[2] = 0b01011100;  //208Hz, +-2000dgs
    ctrl_regs[3] = 0b00000100;  //continious update, autoincrement on
    ctrl_regs[4] = 0b00000000;  //no sleep mode, i2c enable, no LPF1 for gyro
    ctrl_regs[5] = 0b00000000;  //no rounding read, no self-test
    ctrl_regs[6] = 0b00000000;  //DEN trigger ignored, axel high-performance, axel user offset (try to get into this, might be usefull), no need to set LPF1
    ctrl_regs[7] = 0b00000000;  //gyro high-performance, no HPF, no axel user offset, OIS disabled(?)
    ctrl_regs[8] = 0b00000000;  //accel filtering ignored
    ctrl_regs[9] = 0b00000010;  //don't store den value, turn off mipi i3c
    ctrl_regs[10] = 0b00000000;  //no timestamp counter

    i2cSimpleWrite(GYRO_AXEL_ADR, ctrl_regs, 11, TIME_MS2I(1000));

    chThdSleepMilliseconds(110);

    gyroCalculateError();
}

msg_t accelRead(int16_t *axis_values)
{
    uint8_t accel_temp[6] = {0, 0, 0, 0, 0, 0};
    msg_t msg = i2cRegisterRead(GYRO_AXEL_ADR, AXEL_DATA_REG, accel_temp, 6, 1000);

    uint8_t i = 0;
    for(i = 0; i < 3; i++)
    {
        axis_values[i] = (int16_t)((uint16_t)(accel_temp[i * 2]) | ((uint16_t)(accel_temp[(i * 2) + 1]) << 8));
    }
    return msg;
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
    msg_t msg = i2cRegisterRead(GYRO_AXEL_ADR, GYRO_DATA_REG, gyro_temp, 6, 10000);

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
