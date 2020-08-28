#include <ch.h>
#include <hal.h>

#include <chprintf.h>
#include <stdint.h>
#include <stdlib.h>

#include "lcd.h"
#include "i2c.h"


/*****************************************/
/********** I2C Configuration ************/
/*****************************************/





/*****************************************/



int main(void)
{
    chSysInit();
    halInit();
    i2c_start( );

    lcdInit( );
    lcdClear( );


    int16_t number = -777;

    while (true)
    {
//        msg_t msg = i2cMasterTransmitTimeout( i2cDriver, (uint16_t)0b1101011, buf, 2, NULL, 0, 1000 );
        lcdSendChar( 15, 1, '!' );
        lcdSendString( 0, 0, "Big brother" );

        lcdSendNumber( 12, 0, number );
        lcdSendString( 0, 1, "is watching you" );
//        if( msg == MSG_OK ) // just to be sure that the address of LCD is correct
//            {
//                palToggleLine( LINE_LED3 );
//            }
//
        chThdSleepMilliseconds( 1000 );
    }
}
