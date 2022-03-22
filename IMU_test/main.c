#include "ch.h"
#include "hal.h"
#include <chprintf.h>
#include "i2c.h"
#include "math.h"

SPIDriver* spi1 = &SPID1;

// Структура конфигурации, circular режим выключен в halconf.h, CS задается как PAL_LINE
static SPIConfig conf = {
    // Не используем прерывания
    .end_cb = NULL,
    // CS вешаем на ногу А4
    .ssline = PAL_LINE(GPIOA, 4),
    // Значение предделителя 16
    .cr1 = SPI_CR1_BR_1 | SPI_CR1_BR_0,
    // Длина слова 8 бит
    .cr2 = SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0
};


#define WHO_AM_I_ADR 0b00001111

#define GYRO_ADR 0b1101010
#define GYRO_ID 0b01101011

#define BAR_ADR 0b1011100
#define BAR_ID 0b10110011

#define MAG_WHO_AM_I_ADR 0b01001111
#define MAG_ADR 0b0011110
#define MAG_ID 0b01000000

static const SerialConfig sd_st_cfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};

static SerialDriver         *debug_serial = &SD3;
static BaseSequentialStream *debug_stream = NULL;

void debug_stream_init( void )
{
    sdStart( debug_serial, &sd_st_cfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );

    debug_stream = (BaseSequentialStream *)debug_serial;
}

void dbgprintf( const char* format, ... )
{
    if ( !debug_stream )
        return;

    va_list ap;
    va_start(ap, format);
    chvprintf(debug_stream, format, ap);
    va_end(ap);
}

void magInit(void)
{
    uint8_t ctrl_regs[4];
    ctrl_regs[0] = 0b11100000;  //Config register A with autoincrement
    ctrl_regs[1] = 0b00001100;  //no reset, continious mode, 100Hz, temperature compensation
    ctrl_regs[2] = 0b00000000;  //no offset cancelation, no lpf
    ctrl_regs[3] = 0b00010000;  //no self-test, no bits inversion, use i2c, avoid incorrect data

    i2cSimpleWrite(MAG_ADR, ctrl_regs, 4, TIME_MS2I(1000));
}


float mag_raw_to_mgauss(int16_t raw)
{
    return ((float)raw) * 1.5f;
}

void barInit(void)
{
    uint8_t ctrl_regs[3];
    ctrl_regs[0] = 0b00010000;
    ctrl_regs[1] = 0b00111000;
    ctrl_regs[2] = 0b00010010;

    i2cSimpleWrite(BAR_ADR, ctrl_regs, 3, TIME_MS2I(1000));
}

int main(void) {

    halInit();
    chSysInit();

    palSetLineMode(PAL_LINE(GPIOA, 4), PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(PAL_LINE(GPIOA, 5), PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);
    palSetLineMode(PAL_LINE(GPIOA, 6), PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);
    palSetLineMode(PAL_LINE(GPIOA, 7), PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);

    spiStart(spi1, &conf);

    i2cStartUp();
    debug_stream_init();
//    dbgprintf("Test\n\r");
    magInit();
    barInit();

    uint8_t buf[1] = {0};
    i2cRegisterRead(BAR_ADR, WHO_AM_I_ADR, buf, 1, TIME_MS2I(1000));
    if(buf[0] == BAR_ID)
        palSetLine(LINE_LED2);

    i2cRegisterRead(GYRO_ADR, WHO_AM_I_ADR, buf, 1, TIME_MS2I(1000));
    if(buf[0] == GYRO_ID)
        palSetLine(LINE_LED1);

    i2cRegisterRead(MAG_ADR, MAG_WHO_AM_I_ADR, buf, 1, TIME_MS2I(1000));
    if(buf[0] == MAG_ID)
        palSetLine(LINE_LED3);


    uint8_t mag_buf[6];
    int16_t raw_axis_values[3];

    uint8_t bar_buf[3] = {0, 0, 0};
    int16_t temp_buf = 0;
    uint32_t checkval = 2538843507;
    uint32_t outbuf[3] = {0, 0, 0};
    outbuf[0] = checkval;


    while (true) {
//        i2cRegisterRead(MAG_ADR, 0xE8, mag_buf, 6, TIME_MS2I(1000));
//        uint8_t i = 0;
//        for(i = 0; i < 3; i++)
//        {
//            raw_axis_values[i] = (int16_t)((uint16_t)(mag_buf[i * 2]) | ((uint16_t)(mag_buf[(i * 2) + 1]) << 8));
//
//        }
//      dbgprintf("x: %d y: %d z: %d length: %d\r\n", raw_axis_values[0], raw_axis_values[1], raw_axis_values[2], (int)sqrt(raw_axis_values[0] * raw_axis_values[0] + raw_axis_values[1] * raw_axis_values[1]));

      i2cRegisterRead(BAR_ADR, 0x28, bar_buf, 3, TIME_MS2I(1000));
      i2cRegisterRead(BAR_ADR, 0x2B, (uint8_t*) &temp_buf, 2, TIME_MS2I(1000));
      uint32_t bar_raw_data = (uint32_t) bar_buf[0] | ((uint32_t) bar_buf[1] << 8) | (uint32_t) bar_buf[2] << 16;
      float temperature = (float)temp_buf * 0.01;
      float bar_data = (float) bar_raw_data / 40.96;// * 0.750064;
      outbuf[1] = *(uint32_t *)&bar_data;
      outbuf[2] = *(uint32_t *)&temperature;
      //dbgprintf("bar = %f, temp = %.2f \n\r", bar_data, temperature);
      sdWrite(debug_serial, (uint8_t *)outbuf, 24);

        chThdSleepMilliseconds(100);
    }
}
