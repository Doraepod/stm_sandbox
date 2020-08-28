#ifndef INCLUDE_I2C_H_
#define INCLUDE_I2C_H_

static I2CDriver    *i2cDriver  = &I2CD1;

static const I2CConfig i2c1conf = {
         STM32_TIMINGR_PRESC(15U)  |
         STM32_TIMINGR_SCLDEL(9U)  | STM32_TIMINGR_SDADEL(9U) |
         STM32_TIMINGR_SCLH(21U)   | STM32_TIMINGR_SCLL(24U),
         0,                         // CR1
         0                          // CR2
};

#define SCL_LINE    PAL_LINE( GPIOB, 8 )
#define DATA_LINE   PAL_LINE( GPIOB, 9 )

void i2c_start(void);

#endif /* INCLUDE_I2C_H_ */
