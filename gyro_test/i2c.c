#include "include_all.h"

uint8_t register_address[1] = {0};


/**
 * @brief   Start hardware i2c module as master
 */
void i2c_start(void)
{
    palSetLineMode( SCL_LINE,  PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN  );
    palSetLineMode( DATA_LINE, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN );

    i2cStart(i2cDriver, &i2c1conf);
}


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
msg_t i2c_simple_write(uint8_t address, uint8_t *txbuf, uint8_t length, uint16_t timeout)
{
    msg_t msg = i2cMasterTransmitTimeout( i2cDriver, address, txbuf, length, NULL, 0, timeout );
    return msg;
}


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
msg_t i2c_simple_read(uint8_t address, uint8_t *rxbuf, uint8_t length, uint16_t timeout)
{
    msg_t msg = i2cMasterReceiveTimeout( i2cDriver, address, rxbuf, length, timeout );
    return msg;
}


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
msg_t i2c_register_read(uint8_t address, uint8_t register_addr, uint8_t *rxbuf, uint8_t length, uint16_t timeout)
{
    register_address[0] = register_addr;
    msg_t msg = i2cMasterTransmitTimeout( i2cDriver, address, register_address, 1, rxbuf, length, timeout );
    return msg;
}

