#ifndef INCLUDE_I2C_H_
#define INCLUDE_I2C_H_

static const I2CConfig i2c1conf = {
         STM32_TIMINGR_PRESC(15U)  |
         STM32_TIMINGR_SCLDEL(9U)  | STM32_TIMINGR_SDADEL(9U) |
         STM32_TIMINGR_SCLH(21U)   | STM32_TIMINGR_SCLL(24U),
         0,                         // CR1
         0                          // CR2
};

#define SCL_LINE    PAL_LINE( GPIOB, 8 )
#define DATA_LINE   PAL_LINE( GPIOB, 9 )


/**
 * @brief   Start hardware i2c module as master
 */
void i2c_start(void);


/**
 * @brief   Write multiple bytes of data from slave
 * @param
 *          address         7 bit address of slave device
 *          txbuf           link to a buffer array with data to write
 *          length          number of bytes to write
 *          timeout         timeout value [ms]
 *
 * @return                  The operation status.
 * @retval  MSG_OK          if the function succeeded.
 * @retval  MSG_RESET       if one or more I2C errors occurred, the errors can
 *                          be retrieved using @p i2cGetErrors().
 * @retval  MSG_TIMEOUT     if a timeout occurred before operation end.
 */
msg_t i2c_simple_write(uint8_t address, uint8_t *txbuf, uint8_t length, uint16_t timeout);


/**
 * @brief   Read multiple bytes of data from slave
 *
 * @param
 *          address         7 bit address of slave device
 *          rxbuf           link to a buffer array where to store read data
 *          length          number of bytes to read
 *          timeout         timeout value [ms]
 *
 * @return                  The operation status.
 * @retval  MSG_OK          if the function succeeded.
 * @retval  MSG_RESET       if one or more I2C errors occurred, the errors can
 *                          be retrieved using @p i2cGetErrors().
 * @retval  MSG_TIMEOUT     if a timeout occurred before operation end.
 */
msg_t i2c_simple_read(uint8_t address, uint8_t *rxbuf, uint8_t length, uint16_t timeout);


/**
 * @brief   Read multiple bytes of data from specific register of slave
 * @details Function writes register address data to slave and then performs
 *          repeated start action with read bit set
 * @param
 *          address         7 bit address of slave device
 *          register_addr   address of register to read
 *          rxbuf           link to a buffer array where to store read data
 *          length          number of bytes to read
 *          timeout         timeout value [ms]
 *
 * @return                  The operation status.
 * @retval  MSG_OK          if the function succeeded.
 * @retval  MSG_RESET       if one or more I2C errors occurred, the errors can
 *                          be retrieved using @p i2cGetErrors().
 * @retval  MSG_TIMEOUT     if a timeout occurred before operation end.
 */
msg_t i2c_register_read(uint8_t address, uint8_t register_addr, uint8_t *rxbuf, uint8_t length, uint16_t timeout);

#endif /* INCLUDE_I2C_H_ */
