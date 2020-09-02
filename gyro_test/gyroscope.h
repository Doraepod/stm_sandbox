#ifndef GYROSCOPE_H_
#define GYROSCOPE_H_


#define GYRO_ADDR       0x6B    // i2c address of gyroscope
#define GYRO_CtRL1_REG  0xA0    // address of control register with autoincrement bit set
#define WHO_AM_I        0x0F    // sensor id register
#define GYRO_ID         0xD7    // id of the gyroscope to compare
#define GYRO_DATA_REG   0xA8    // address of OUT_X_L register with autoincrement bit set
#define GYRO_INT_PERIOD 100

void myFunc(void);

void startGyroPosition(void);


void gyroscope_init(void);

msg_t read_gyroscope(int16_t *axis_values);

msg_t get_gyro_speed(float *axis_speed);

void get_gyro_error(float *buf);



#endif /* GYROSCOPE_H_ */
