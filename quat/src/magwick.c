#include "magwick.h"

float angular_speed[3]    = {0, 0, 0};
int16_t acceleration[3]   = {0, 0, 0};

#define BETA 0.07557

quaternion_t base = {
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
    .w = 0,
    .x = 0,
    .y = 0,
    .z = 0
};

quaternion_t measured_acceleration = {
    .w = 0,
    .x = 0,
    .y = 0,
    .z = 0
};

quaternion_t real_acceleration = {
    .w = 0,
    .x = 0,
    .y = 0,
    .z = 1
};

quaternion_t gyro_error = {
    .w = 0,
    .x = 0,
    .y = 0,
    .z = 0
};

quaternion_t jacob(quaternion_t quat)
{
    quaternion_t res;
    res.w = (-2) * base.y * quat.x + 2 * base.x * quat.y;
    res.x = 2 * base.z * quat.x + 2 * base.w * quat.y - 4 * base.x * quat.z;
    res.y = (-2) * base.w * quat.x + 2 * base.z * quat.y - 4 * base.y * quat.z;
    res.z = 2 * base.x * quat.x + 2 * base.y * quat.y;
    return res;
}

static THD_WORKING_AREA(magwickThread, 256); // 256 - stack size
static THD_FUNCTION(Magwick, arg)
{
    arg = arg;              // just to avoid warnings
    systime_t time = chVTGetSystemTime();
    while( true )
    {
        gyroReadSpeed(angular_speed);
        gyro_speed.x = angular_speed[0] * DEG2RAD;
        gyro_speed.y = angular_speed[1] * DEG2RAD;
        gyro_speed.z = angular_speed[2] * DEG2RAD;

        accelRead(acceleration);
        measured_acceleration.x = acceleration[0];
        measured_acceleration.y = acceleration[1];
        measured_acceleration.z = acceleration[2];

        measured_acceleration = quatNormalize(measured_acceleration);

        quaternion_t f = quatSubtract(quatRotate(real_acceleration, quatInvert(base)), measured_acceleration);
        gyro_error = quatNormalize(jacob(f));

        gyro_earth_speed = quatScale(quatProduct(base, gyro_speed), 0.5);

        base = quatNormalize(quatAdd(base, quatScale(quatSubtract(gyro_earth_speed, quatScale(gyro_error, BETA)), TIME_STEP * 0.001)));

        time = chThdSleepUntilWindowed(time, time + TIME_MS2I(TIME_STEP));
    }
}

static bool             isInitialized       = false;

void magwickInit(tprio_t priority)
{
    if(isInitialized)
        return;
    i2cStartUp();
    gaInit();

    chThdCreateStatic(magwickThread, sizeof(magwickThread), priority, Magwick, NULL);
    isInitialized = true;
}

quaternion_t getQuat(void)
{
    return base;
}



