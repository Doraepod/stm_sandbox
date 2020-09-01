#include "include_all.h"

float   angular_speed[3] = {0, 0, 0};
float   gyro_mean_error[3] = {0, 0, 0};
float   angle[3] = {0, 0, 0};

int main(void)
{
    chSysInit();
    halInit();
    i2c_start();

    lcdInit();
    lcdClear();
    gyroscope_init();

    get_gyro_error(gyro_mean_error);

    systime_t time;
    time = chVTGetSystemTime();

//    uint8_t wrbuf[2] = {0x20, 0x0F};
//    int8_t readbuf[2] = {0, 0};
//    msg_t msg = i2c_simple_write(0x6B, wrbuf, 2, 1000);

    while (true)
    {
        uint8_t i;
        msg_t msg = get_gyro_speed(angular_speed);
        for(i = 0; i < 3; i++)
        {
            angular_speed[i] -= gyro_mean_error[i];
            if (abs(angular_speed[i]) < 0.01) angular_speed[i] = 0;
            angle[i] += angular_speed[i] * 0.2;
        }
        lcdClear();
        lcdSendNumber(0, 0, angle[0]);
        lcdSendNumber(8, 0, angle[1]);
        lcdSendNumber(0, 1, angle[2]);


//        msg_t msg = i2c_register_read( 0x6B, 0xA8, readbuf, 2, 1000 );
//        if ( msg == MSG_OK )
//        {
//            lcdSendNumber( 0, 0, (readbuf[0] | (readbuf[1] << 8)) );
//        }
//        else
//        {
//            lcdSendString(0, 0, "Nope");
//        }
//        lcdSendChar( 15, 1, '!' );
//        lcdSendString( 0, 0, "Big brother" );
//
//        lcdSendNumber( 12, 0, number );
//        lcdSendString( 0, 1, "is watching you" );
//        if( msg == MSG_OK ) // just to be sure that the address of LCD is correct
//            {
//                palToggleLine( LINE_LED3 );
//            }
//
        chThdSleepUntil(time += MS2ST(200));

    }
}
