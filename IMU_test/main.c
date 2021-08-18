#include "ch.h"
#include "hal.h"
#include <chprintf.h>
#include "i2c.h"

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

int main(void) {

    halInit();
    chSysInit();

    i2cStartUp();
    debug_stream_init();
    dbgprintf("Test\n\r");

    uint8_t buf[1] = {0};
    i2cRegisterRead(BAR_ADR, WHO_AM_I_ADR, buf, 1, TIME_MS2I(1000));
    if(buf[0] == BAR_ID)
        palSetLine(LINE_LED2);
    dbgprintf("bar %d\r\n", buf[0]);

    i2cRegisterRead(GYRO_ADR, WHO_AM_I_ADR, buf, 1, TIME_MS2I(1000));
    if(buf[0] == GYRO_ID)
        palSetLine(LINE_LED1);
    dbgprintf("gyr %d\r\n", buf[0]);

    i2cRegisterRead(MAG_ADR, MAG_WHO_AM_I_ADR, buf, 1, TIME_MS2I(1000));
    if(buf[0] == MAG_ID)
        palSetLine(LINE_LED3);
    dbgprintf("mag %d\r\n", buf[0]);
//    uint8_t i = 1;

//    for(i = 29; i <= 127; i++)
//    {
//        chThdSleepMilliseconds(50);
//        i2cRegisterRead(i, WHO_AM_I_ADR, buf, 1, TIME_MS2I(1000));
//        uint8_t flag = i2cGetErrors(&I2CD1);
//        dbgprintf("%d flag %d\r\n", i, flag);
//
//    }



    while (true) {
        chThdSleepMilliseconds(1000);
    }
}
