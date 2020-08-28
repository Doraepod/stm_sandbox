void i2c_start(void)
{
    palSetLineMode( SCL_LINE,  PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN  );
    palSetLineMode( DATA_LINE, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN );

    i2cStart(i2cDriver, &i2c1conf);
}
