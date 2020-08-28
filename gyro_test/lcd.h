#ifndef LCD_H_
#define LCD_H_

#define e_set()     lcdWriteByte( port_lcd |= 0x04 )  // set E in 1
#define e_reset()   lcdWriteByte( port_lcd &= ~0x04 ) // set E in 0
#define rs_set()    lcdWriteByte( port_lcd |= 0x01 )  // set RS in 1
#define rs_reset()  lcdWriteByte( port_lcd &= ~0x01 ) // set RS in 0
#define set_led()   lcdWriteByte( port_lcd |= 0x08 )  // turn on backlight
#define set_write() lcdWriteByte( port_lcd &= ~0x02 ) // writing into lcd-memory

/*
 * @brief   Send information to LCD via I2C (aka send command)
 */
void lcdWriteByte( uint8_t byte );

/*
 * @brief   Send 4 significant bits
 */
void lcdSendHalfByte( uint8_t byte );

/*
 * @brief   Send 8 significant bits (1 byte)
 */
void lcdSendByte( uint8_t byte, uint8_t mode );

/*
 * @brief   Clear LCD
 */
void lcdClear(void);

/*
 * @brief   Set the position of cursor
 * @note    Work with 2 rows only
 *          The initial position is ( 0, 0 )
 */
void lcdSetCursorPos( uint8_t x, uint8_t y );

/*
 * @brief   Show 1 character on display
 * @note    The initial position of cursor is ( 0, 0 )
 */
void lcdSendChar( uint8_t x, uint8_t y, char ch );

/*
 * @brief   Show string on display
 * @note    The initial position of cursor is ( 0, 0 )
 */
void lcdSendString( uint8_t x, uint8_t y, char* str );

/*
 * @brief   Show signed integer number on display
 * @note    The initial position of cursor is ( 0, 0 )
 */
void lcdSendNumber( uint8_t x, uint8_t y, int16_t num );

/*
 * @brief   Initialization of LCD-display
 */
void lcdInit( void );

#endif /* LCD_H_ */
