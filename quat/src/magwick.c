#include "magwick.h"
#include "debug.h"

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

static THD_WORKING_AREA(magwickThread, 256); // 256 - stack size
static THD_FUNCTION(Magwick, arg)
{
    arg = arg;              // just to avoid warnings
    systime_t time = chVTGetSystemTime();
    while( true )
    {
        gyroReadSpeed(angular_speed);
        gyro_speed.x = angular_speed[1] * DEG2RAD;
        gyro_speed.y = angular_speed[0] * DEG2RAD * (-1);
        gyro_speed.z = angular_speed[2] * DEG2RAD;

        accelRead(acceleration);
        measured_acceleration.x = acceleration[0];
        measured_acceleration.y = acceleration[1];
        measured_acceleration.z = acceleration[2];
       // dbgprintf("x = %d, y = %d, z = %d\r\n", acceleration[0], acceleration[1], acceleration[2]);

        measured_acceleration = quatNormalize(measured_acceleration);

        float a_x = measured_acceleration.x;
        float a_y = measured_acceleration.y;
        float a_z = measured_acceleration.z;

        float SEq_1 = base.w;
        float SEq_2 = base.x;
        float SEq_3 = base.y;
        float SEq_4 = base.z;

        float twoSEq_1 = 2.0f * SEq_1;
        float twoSEq_2 = 2.0f * SEq_2;
        float twoSEq_3 = 2.0f * SEq_3;
        // Compute the objective function and Jacobian
        float f_1 = twoSEq_2 * SEq_4 - twoSEq_1 * SEq_3 - a_x;
        float f_2 = twoSEq_1 * SEq_2 + twoSEq_3 * SEq_4 - a_y;
        float f_3 = 1.0f - twoSEq_2 * SEq_2 - twoSEq_3 * SEq_3 - a_z;
        float J_11or24 = twoSEq_3; // J_11 negated in matrix multiplication
        float J_12or23 = 2.0f * SEq_4;
        float J_13or22 = twoSEq_1; // J_12 negated in matrix multiplication
        float J_14or21 = twoSEq_2;
        float J_32 = 2.0f * J_14or21; // negated in matrix multiplication
        float J_33 = 2.0f * J_11or24; // negated in matrix multiplication
        // Compute the gradient (matrix multiplication)
        float SEqHatDot_1 = J_14or21 * f_2 - J_11or24 * f_1;
        float SEqHatDot_2 = J_12or23 * f_1 + J_13or22 * f_2 - J_32 * f_3;
        float SEqHatDot_3 = J_12or23 * f_2 - J_33 * f_3 - J_13or22 * f_1;
        float SEqHatDot_4 = J_14or21 * f_1 + J_11or24 * f_2;
        // Normalise the gradient
        float norm = sqrt(SEqHatDot_1 * SEqHatDot_1 + SEqHatDot_2 * SEqHatDot_2 + SEqHatDot_3 * SEqHatDot_3 + SEqHatDot_4 * SEqHatDot_4);
        SEqHatDot_1 /= norm;
        SEqHatDot_2 /= norm;
        SEqHatDot_3 /= norm;
        SEqHatDot_4 /= norm;

        gyro_error.w = SEqHatDot_1;
        gyro_error.x = SEqHatDot_2;
        gyro_error.y = SEqHatDot_3;
        gyro_error.z = SEqHatDot_4;

        //quaternion_t quatNormalize(quatSubtract(quatRotate(real_acceleration, base), measured_acceleration));


        gyro_earth_speed = quatScale(quatProduct(base, gyro_speed), 0.5);

        base = quatNormalize(quatAdd(base, quatScale(quatSubtract(gyro_earth_speed, quatScale(gyro_error, BETA)), TIME_STEP * 0.001)));

//        base = quatAdd(base, quatScale(gyro_earth_speed, TIME_STEP * 0.001));



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
    return base;
}



