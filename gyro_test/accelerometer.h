#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_


#define ACCEL_ADDR      0x18    // i2c address of accelerometer
#define ACCEL_CTRL1_REG  0xA0    // address of control register with autoincrement bit set
#define WHO_AM_I        0x0F    // sensor id register
#define ACCEL_ID        0x33    // id of the accelerometer to compare
#define ACCEL_DATA_REG   0xA8    // address of OUT_X_L register with autoincrement bit set


void accelerometer_init(void);

msg_t read_accelerometer(int16_t *axis_values);

#endif
