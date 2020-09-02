#include "include_all.h"

int main(void)
{
    chSysInit();
    halInit();
    i2c_start();

    lcdInit();
    lcdClear();
    gyroscope_init();

    startGyroPosition();
    systime_t time;
    time = chVTGetSystemTime();

    while (true)
    {
        //myFunc();
        chThdSleepUntil(time += MS2ST(20));

    }
}
