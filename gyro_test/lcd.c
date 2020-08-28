uint8_t port_lcd    = 0;

uint8_t buf[1] = {0};

void lcdInit( void )
{
    chThdSleepMilliseconds( 20 );
    lcdSendHalfByte( 0x03 );
    chThdSleepMilliseconds( 4 );
    lcdSendHalfByte( 0x03 );
    chThdSleepMicroseconds( 100 );
    lcdSendHalfByte( 0x03 );
    chThdSleepMilliseconds( 1 );
    lcdSendHalfByte( 0x02 );
    chThdSleepMilliseconds( 1 );

    lcdSendByte( 0x28, 0 );         // 4 bit mode (DL = 0), 2 lines (N = 1)
    chThdSleepMilliseconds( 1 );
    lcdSendByte( 0x0C, 0 );         // show image on LCD (D = 1)
    chThdSleepMilliseconds( 1 );
    lcdSendByte( 0x01, 0 );         //  clean the screen
    chThdSleepMilliseconds(2);
    lcdSendByte(0x06, 0);           // cursor will move to the right
    chThdSleepMilliseconds(1);

    set_led();                      // turn on LED
    set_write();
}

void lcdSendNumber( uint8_t x, uint8_t y, int16_t num )
{
    char buff[16];                  // the width of LCD is 16 cells

    itoa( num, buff, 10 );          // 10 -> decimal radix
    lcdSendString( x, y, buff );
}

void lcdSendString( uint8_t x, uint8_t y, char* str )
{
    uint8_t i = 0;
    lcdSetCursorPos( x, y );
    while( str[i] != 0 )
    {
        lcdSendByte( str[i], 1 );
        i++;
    }
}

void lcdSendChar( uint8_t x, uint8_t y, char ch )
{
    lcdSetCursorPos( x, y );
    lcdSendByte( ch, 1 );
}

void lcdSetCursorPos( uint8_t x, uint8_t y )
{
    uint8_t command = (y == 0) ? 0x80 : 0xc0;
    command |= x;
    lcdSendByte( command, 0);
}

void lcdClear(void)
{
    lcdSendByte( 0x01, 0 );
    chThdSleepMicroseconds( 1500 );
}

void lcdSendByte( uint8_t byte, uint8_t mode )
{
    if( mode == 0 ) rs_reset();
    else rs_set();

    uint8_t hc = 0;
    hc = byte >> 4;
    lcdSendHalfByte( hc );
    lcdSendHalfByte( byte );
}

void lcdSendHalfByte( uint8_t byte )
{
    byte <<= 4;         // send 4 high bits (only that 4 bits are important)
    e_set();
    chThdSleepMicroseconds( 50 );
    lcdWriteByte( port_lcd|byte );
    e_reset();
    chThdSleepMicroseconds( 50 );
}

void lcdWriteByte( uint8_t byte )
{

    buf[0] = byte;

    msg_t msg = i2cMasterTransmitTimeout( i2cDriver, (uint16_t)0x27, buf, 1, NULL, 0, 1000 );

    if( msg == MSG_OK ) // just to be sure that the address of LCD is correct
    {
        palSetLine( LINE_LED3 );
    }
}
