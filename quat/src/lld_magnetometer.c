#include "lld_magnetometer.h"

void magInit(void)
{
    uint8_t ctrl_regs[4];
    ctrl_regs[0] = MAG_CONFIG_REG_ADR;  //Config register A with autoincrement
    ctrl_regs[1] = 0b10001100;  //no reset, continious mode, 100Hz, temperature compensation
    ctrl_regs[2] = 0b00000000;  //no offset cancelation, no lpf
    ctrl_regs[3] = 0b00010000;  //no self-test, no bits inversion, use i2c, avoid incorrect data

    i2cSimpleWrite(MAG_ADR, ctrl_regs, 4, TIME_MS2I(1000));
}

msg_t magRead(int16_t* axis_values)
{
    uint8_t mag_temp[6] = {0, 0, 0, 0, 0, 0};
    msg_t msg = i2cRegisterRead(MAG_ADR, MAG_DATA_REG, mag_temp, 6, TIME_MS2I(1000));
    uint8_t i = 0;
    for(i = 0; i < 3; i++)
    {
        axis_values[i] = (int16_t)((uint16_t)(mag_temp[i * 2]) | ((uint16_t)(mag_temp[(i * 2) + 1]) << 8));
    }
    return msg;
}
