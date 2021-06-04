#include "include_all.h"

float       angle[3] = {0, 0, 0};
float       pitch_angle = 0;
float       roll_angle = 0;
int16_t    acceleration[3] = {0, 0, 0};

float calcAccelAngle(int16_t main_projection, int16_t second_projection, int16_t third_projection)
{
    if((second_projection == 0) && (third_projection == 0))
    {
        second_projection = 1;
        third_projection = 1;
    }
    float angle = atan(main_projection / sqrt(powf(second_projection, 2) + powf(third_projection, 2)));

    if(abs(second_projection) > abs(third_projection))
    {
        if(second_projection < 0) angle = main_projection < 0 ? -M_PI - angle : M_PI - angle;
    }
    else
    {
        if(third_projection < 0) angle = main_projection < 0 ? -M_PI - angle : M_PI - angle;
    }
    return angle * RAD2DEG;
}

void readAccel(void)
{
    msg_t msg = readAccelerometer(acceleration);
    uint8_t i = 0;

    // First try
//    for(i = 0; i < 3; i++)
//    {
//        if(acceleration[i] == 0) acceleration[i] = 1;
//    }
//    float x = atan(acceleration[0] / sqrt(powf(acceleration[1], 2) + powf(acceleration[2], 2)));
//    float y = atan(acceleration[1] / sqrt(powf(acceleration[0], 2) + powf(acceleration[2], 2)));
//    //float z = atan(acceleration[2] / sqrt(powf(acceleration[1], 2) + powf(acceleration[0], 2)));
//
//    //pitch angle
//    if(abs(acceleration[2]) > abs(acceleration[1]))
//    {
//        if(acceleration[2] < 0)
//        {
//            if(acceleration[0] < 0) x = -3.14 - x;
//            else x = 3.14 - x;
//        }
//    }
//    else
//    {
//        if(acceleration[1] < 0)
//        {
//            if(acceleration[0] < 0) x = -3.14 - x;
//            else x = 3.14 - x;
//        }
//    }
//    //roll angle
//    if(abs(acceleration[2]) > abs(acceleration[0]))
//    {
//        if(acceleration[2] < 0)
//        {
//            if(acceleration[1] < 0) y = -3.14 - y;
//            else y = 3.14 - y;
//        }
//    }
//    else
//    {
//        if(acceleration[0] < 0)
//        {
//            if(acceleration[1] < 0) y = -3.14 - y;
//            else y = 3.14 - y;
//        }
//    }
//    roll_angle = y * 57.325;
//    pitch_angle = x * 57.325;
//    roll_angle = calcAccelAngle(acceleration[1], acceleration[0], acceleration[2]);
//    pitch_angle = calcAccelAngle(acceleration[0], acceleration[1], acceleration[2]);




//    // Second try
//    int8_t z_sign = (acceleration[2] > 0) ? 1 : -1;
//    roll_angle = atan2(acceleration[1], z_sign * sqrt(powf(acceleration[0], 2) + powf(acceleration[2], 2))) * RAD2DEG;
//    pitch_angle = atan2(acceleration[0], sqrt(powf(acceleration[1], 2) + powf(acceleration[2], 2))) * RAD2DEG;

    // Third try
    float length = sqrt(powf(acceleration[0], 2) + powf(acceleration[1], 2) + powf(acceleration[2], 2));
    roll_angle = atan2(acceleration[1] / length, acceleration[2] / length) * RAD2DEG;
    pitch_angle = atan2(acceleration[0] / length, acceleration[2] / length) * RAD2DEG;
}



int main(void)
{
    float       angular_speed[3] = {0, 0, 0};
    float       gyro_mean_error[3] = {0, 0, 0};


    chSysInit();
    halInit();
    i2c_start();
    debug_stream_init();


    accelerometerInit();
    gyroscopeInit();
    chThdSleepSeconds(1);
    readAccel();
    angle[0] = pitch_angle;
    angle[1] = roll_angle;
    getGyroError(gyro_mean_error);

    systime_t time;
    time = chVTGetSystemTime();

    while (true)
    {
        uint8_t i;
        msg_t msg = getGyroSpeed(angular_speed);
        for(i = 0; i < 3; i++)
        {
            angular_speed[i] -= gyro_mean_error[i];
            if (abs(angular_speed[i]) < 0.01) angular_speed[i] = 0;
//            angle[i] += angular_speed[i] * 0.1;
        }
        angle[2] += angular_speed[2] * 0.1;
        angle[0] = 1 * (angle[0] + angular_speed[0] * 0.1) + 0.00 * pitch_angle;
        angle[1] = 1 * (angle[1] + angular_speed[1] * 0.1) + 0.00 * roll_angle;
        readAccel();
        dbgprintf("X: %d\tY: %d\tZ: %d\n\rpitch: %d\troll: %d\n\r", (int)(angle[0]), (int)(angle[1]), (int)(angle[2]), (int)(pitch_angle), (int)(roll_angle));

        chThdSleepUntil(time += MS2ST(100));

    }
}
