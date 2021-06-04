#include "include_all.h"

void accelerometerInit(void)
{
    uint8_t accel_id[1] = {0};
    uint8_t initbuf[5];
    initbuf[0] = ACCEL_CTRL1_REG;            // Start from cntl1 register
    initbuf[1] = 0x27;                      // Normal mode, each axis enable, 10 Hz output
    initbuf[2] = 0x00;                      // High pass filter default configuration
    initbuf[3] = 0x00;                      // Too smart for me
    initbuf[4] = 0x00;                      // +-2g

    msg_t msg = i2c_simple_write(ACCEL_ADDR, initbuf, 5, 1000);

    msg = i2c_register_read(ACCEL_ADDR, WHO_AM_I, accel_id, 1, 1000);
    if (accel_id[0] == ACCEL_ID)
    {
        palSetLine(LINE_LED3);              // Check if gyroscope present and working
    }
}

msg_t readAccelerometer(int16_t *axis_values)
{
    uint8_t accel_temp[6] = {0, 0, 0, 0, 0, 0};
    msg_t msg = i2c_register_read(ACCEL_ADDR, ACCEL_DATA_REG, accel_temp, 6, 1000);

    uint8_t i = 0;
    for(i = 0; i < 3; i++)
    {
        axis_values[i] = (int16_t)((uint16_t)(accel_temp[i * 2]) | ((uint16_t)(accel_temp[(i * 2) + 1]) << 8));
    }
    return msg;
}
