#include "include_all.h"

float   angular_speed[3] = {0, 0, 0};
float   gyro_mean_error[3] = {0, 0, 0};
float   angle[3] = {0, 0, 0};

static virtual_timer_t  gyroscope_vt;
float                   gyro_angle_xyz[3]   = {0, 0, 0};

float   gyro_speed[3] = {0, 0, 0};

void myFunc(void)
{
    uint8_t i;
    msg_t msg = get_gyro_speed(angular_speed);
    for(i = 0; i < 3; i++)
    {
        angular_speed[i] -= gyro_mean_error[i];
        if (abs(angular_speed[i]) < 0.01) angular_speed[i] = 0;
        angle[i] += angular_speed[i] * 0.02;
    }
}

static void gyroIntegrationCallback(void *args)
{
    chSysLockFromISR();
    chVTSetI(&gyroscope_vt, MS2ST(GYRO_INT_PERIOD), gyroIntegrationCallback, NULL);
    chSysUnlockFromISR();
    palToggleLine(LINE_LED1);
    lcdClear();
//    lcdSendNumber(0, 0, angle[0]);
//    lcdSendNumber(8, 0, angle[1]);
//    lcdSendNumber(0, 1, angle[2]);
    lcdSendNumber(0, 0, 20);

}


void startGyroPosition(void)
{
    chVTObjectInit(&gyroscope_vt);
    chVTSet( &gyroscope_vt, MS2ST(GYRO_INT_PERIOD), gyroIntegrationCallback, NULL );
}
/**
 * @brief   Initialize gyroscope
 */
void gyroscope_init(void)
{
    uint8_t gyro_id[1] = {0};
    uint8_t initbuf[5];
    initbuf[0] = GYRO_CtRL1_REG;            // Start from cntl1 register
    initbuf[1] = 0x0F;                      // Normal mode, each axis enable, freq settings ignored
    initbuf[2] = 0x00;                      // High pass filter default configuration
    initbuf[3] = 0x00;                      // Too smart for me
    initbuf[4] = 0x30;                      // 2000dps

    msg_t msg = i2c_simple_write(GYRO_ADDR, initbuf, 5, 1000);
    get_gyro_error(gyro_mean_error);
    msg = i2c_register_read(GYRO_ADDR, WHO_AM_I, gyro_id, 1, 1000);
    if (gyro_id[0] == GYRO_ID)
    {
        palSetLine(LINE_LED2);              // Check if gyroscope present and working
    }
}


/**
 * @brief   read gyroscope axis values in XYZ order
 *
 * @param
 *          *axis_values         array where to store read data
 */
msg_t read_gyroscope(int16_t *axis_values)
{
    uint8_t gyro_temp[6] = {0, 0, 0, 0, 0, 0};
    msg_t msg = i2c_register_read(GYRO_ADDR, GYRO_DATA_REG, gyro_temp, 6, 1000);

    uint8_t i = 0;
    for(i = 0; i < 3; i++)
    {
        axis_values[i] = (int16_t)((uint16_t)(gyro_temp[i * 2]) | ((uint16_t)(gyro_temp[(i * 2) + 1]) << 8));
    }
    return msg;
}


msg_t get_gyro_speed(float *axis_speed)
{
    int16_t gyro_axis_values[3] = {0, 0, 0};
    uint8_t i = 0;
    msg_t msg = read_gyroscope(gyro_axis_values);
    for(i = 0; i < 3; i++)
    {
        axis_speed[i] = (float)gyro_axis_values[i] * 0.07;
    }
    return msg;
}

void get_gyro_error(float *buf)
{
    int16_t temp_buf[10][3] = {{0}};

    int16_t super_temp[3] = {0, 0, 0};
    uint8_t i, j;
    float x_mean = 0, y_mean = 0, z_mean = 0;
    for(i = 0; i < 10; i++)
    {
        msg_t msg = read_gyroscope(super_temp);
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
}
