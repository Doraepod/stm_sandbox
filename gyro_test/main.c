#include "include_all.h"

float       angle[3] = {0, 0, 0};
float       pitch_angle = 0;
float       roll_angle = 0;
int16_t    acceleration[3] = {0, 0, 0};

void read_accel(void)
{
    msg_t msg = read_accelerometer(acceleration);
    uint8_t i = 0;
    for(i = 0; i < 3; i++)
    {
        if(acceleration[i] == 0) acceleration[i] = 1;
    }
    float x = atan(acceleration[0] / sqrt(powf(acceleration[1], 2) + powf(acceleration[2], 2)));
    float y = atan(acceleration[1] / sqrt(powf(acceleration[0], 2) + powf(acceleration[2], 2)));
    //float z = atan(acceleration[2] / sqrt(powf(acceleration[1], 2) + powf(acceleration[0], 2)));

    //pitch angle
    if(abs(acceleration[2]) > abs(acceleration[1]))
    {
        if(acceleration[2] < 0)
        {
            if(acceleration[0] < 0) x = -3.14 - x;
            else x = 3.14 - x;
        }
    }
    else
    {
        if(acceleration[1] < 0)
        {
            if(acceleration[0] < 0) x = -3.14 - x;
            else x = 3.14 - x;
        }
    }
    //roll angle
    if(abs(acceleration[2]) > abs(acceleration[0]))
    {
        if(acceleration[2] < 0)
        {
            if(acceleration[1] < 0) y = -3.14 - y;
            else y = 3.14 - y;
        }
    }
    else
    {
        if(acceleration[0] < 0)
        {
            if(acceleration[1] < 0) y = -3.14 - y;
            else y = 3.14 - y;
        }
    }

    roll_angle = y * 57.325;
    pitch_angle = x * 57.325;
}



int main(void)
{
    float       angular_speed[3] = {0, 0, 0};
    float       gyro_mean_error[3] = {0, 0, 0};


    chSysInit();
    halInit();
    i2c_start();
    debug_stream_init();


    accelerometer_init();
    gyroscope_init();
    chThdSleepSeconds(1);
    read_accel();
    angle[0] = pitch_angle;
    angle[1] = roll_angle;
    get_gyro_error(gyro_mean_error);

    systime_t time;
    time = chVTGetSystemTime();

    while (true)
    {
        uint8_t i;
        msg_t msg = get_gyro_speed(angular_speed);
        for(i = 0; i < 3; i++)
        {
            angular_speed[i] -= gyro_mean_error[i];
            if (abs(angular_speed[i]) < 0.01) angular_speed[i] = 0;
//            angle[i] += angular_speed[i] * 0.1;
        }
        angle[2] += angular_speed[2] * 0.1;
        angle[0] = 0.95 * (angle[0] + angular_speed[0] * 0.1) + 0.05 * pitch_angle;
        angle[1] = 0.95 * (angle[1] + angular_speed[1] * 0.1) + 0.05 * roll_angle;
        read_accel();
        dbgprintf("X: %d\tY: %d\tZ: %d\n\rpitch: %d\troll: %d\n\r", (int)(angle[0]), (int)(angle[1]), (int)(angle[2]), (int)(pitch_angle), (int)(roll_angle));

        chThdSleepUntil(time += MS2ST(100));

    }
}
