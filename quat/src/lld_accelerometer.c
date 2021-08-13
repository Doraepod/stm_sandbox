#include "lld_accelerometer.h"

void accelInit(void)
{
    uint8_t initbuf[5];
    initbuf[0] = ACCEL_CTRL1_REG;            // Start from cntl1 register
    initbuf[1] = 0x27;                      // Normal mode, each axis enable, 10 Hz output
    initbuf[2] = 0x00;                      // High pass filter default configuration
    initbuf[3] = 0x00;                      // Too smart for me
    initbuf[4] = 0x00;                      // +-2g

    i2cSimpleWrite(ACCEL_ADDR, initbuf, 5, 1000);
}

msg_t accelRead(int16_t *axis_values)
{
    uint8_t accel_temp[6] = {0, 0, 0, 0, 0, 0};
    msg_t msg = i2cRegisterRead(ACCEL_ADDR, ACCEL_DATA_REG, accel_temp, 6, 1000);

    uint8_t i = 0;
    for(i = 0; i < 3; i++)
    {
        axis_values[i] = (int16_t)((uint16_t)(accel_temp[i * 2]) | ((uint16_t)(accel_temp[(i * 2) + 1]) << 8));
    }
    return msg;
}
