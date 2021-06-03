#include <ch.h>
#include <hal.h>
#include <chprintf.h>
#include <stdint.h>
#include <stdlib.h>
#include "debug.c"

#define SCL_LINE    PAL_LINE( GPIOB, 8 )
#define DATA_LINE   PAL_LINE( GPIOB, 9 )

static I2CDriver    *i2cDriver  = &I2CD1;

static const I2CConfig i2c1conf = {
         STM32_TIMINGR_PRESC(15U)  |
         STM32_TIMINGR_SCLDEL(9U)  | STM32_TIMINGR_SDADEL(9U) |
         STM32_TIMINGR_SCLH(21U)   | STM32_TIMINGR_SCLL(24U),
         0,                         // CR1
         0                          // CR2
};

int main(void)
{
    chSysInit();
    halInit();
    debug_stream_init();

    palSetLineMode( SCL_LINE,  PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN  );
    palSetLineMode( DATA_LINE, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN );
    i2cStart(i2cDriver, &i2c1conf);
    uint8_t buf[6] = {0, 0, 0, 0, 0, 0};
    uint8_t addr[1] = {0xA8};
    uint8_t i;
    int16_t meas[3] = {0, 0, 0};
    uint8_t who_am_i[1] = {0x0F};

    i2cMasterTransmitTimeout( i2cDriver, 0b0011001, who_am_i, 1, buf, 1, 1000 );
    dbgprintf("I am %d\n\r", buf[0]);
    uint8_t init[2] = {0x20, 0b10010111};
    i2cMasterTransmitTimeout( i2cDriver, 0b0011001, init, 2, NULL, 0, 1000 );
    chThdSleepMilliseconds(1000);




    while (true)
    {
        i2cMasterTransmitTimeout( i2cDriver, 0b0011001, addr, 1, buf, 6, 1000 );
        for( i = 0; i < 3; i++)
        {
            meas[i] = (int16_t)((uint16_t)(buf[i * 2]) | ((uint16_t)(buf[(i * 2) + 1]) << 8));
        }
        dbgprintf("x %d y %d z %d\n\r", meas[0], meas[1], meas[2]);
        chThdSleepMilliseconds(100);

    }
}
