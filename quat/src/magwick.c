#include "magwick.h"

float angular_speed[3]    = {0, 0, 0};
int16_t acceleration[3]   = {0, 0, 0};


quaternion_t base = {
    .w = 1,
    .x = 1,
    .y = 0,
    .z = 0
};

quaternion_t gyro_angle = {
    .w = 1,
    .x = 0,
    .y = 0,
    .z = 0
};

quaternion_t gyro_speed = {
    .w = 0,
    .x = 0,
    .y = 0,
    .z = 0
};

quaternion_t gyro_earth_speed = {
    .w = 1,
    .x = 0,
    .y = 0,
    .z = 0
};


static THD_WORKING_AREA(magwickThread, 256); // 256 - stack size
static THD_FUNCTION(Magwick, arg)
{
    arg = arg;              // just to avoid warnings
    systime_t time = chVTGetSystemTime();
    while( true )
    {
        msg_t msg = gyroReadSpeed(angular_speed);
        msg = accelRead(acceleration);
        if(msg == MSG_OK)
        {
            gyro_speed.x = angular_speed[1] * DEG2RAD;
            gyro_speed.y = angular_speed[0] * DEG2RAD * (-1);
            gyro_speed.z = angular_speed[2] * DEG2RAD;
            gyro_earth_speed = quatScale(quatProduct(gyro_angle, gyro_speed), 0.5);
            gyro_angle = quatAdd(gyro_angle, quatScale(gyro_earth_speed, TIME_STEP * 0.001));
        }


        time = chThdSleepUntilWindowed(time, time + TIME_MS2I(TIME_STEP));
    }
}

static bool             isInitialized       = false;

void magwickInit(tprio_t priority)
{
    if(isInitialized)
        return;
    i2cStartUp();
    gyroInit();
    accelInit();

    chThdCreateStatic(magwickThread, sizeof(magwickThread), priority, Magwick, NULL);
    isInitialized = true;
}

quaternion_t getQuat(void)
{
    return gyro_angle;
}



