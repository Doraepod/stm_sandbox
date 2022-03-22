#include "magwick.h"

float angular_speed[3]    = {0, 0, 0};
int16_t acceleration[3]   = {0, 0, 0};
int16_t magnetometer_data[3] = {0, 0, 0};

#define BETA 0.05
#define ZETA 0.001

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

quaternion_t gyro_drift = {
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

quaternion_t measured_magnetic_lines = {
    .w = 0,
    .x = 0,
    .y = 0,
    .z = 0
};

quaternion_t real_magnetic_lines = {
    .w = 0,
    .x = 1,
    .y = 0,
    .z = 0
};

quaternion_t gyro_error;

quaternion_t jacob(quaternion_t quat_a, quaternion_t quat_m)
{
    quaternion_t res;
    quaternion_t b = real_magnetic_lines;
    res.w = (-2) * base.y * quat_a.x + 2 * base.x * quat_a.y + 2 * base.w * quat_a.z + \
            quat_m.x * (2 * b.x * base.w - 2 * b.z * base.y) + quat_m.z * (2 * b.x * base.y + 2 * b.z * base.w) - quat_m.y * (2 * b.x * base.z - 2 * b.z * base.x);
    res.x = 2 * base.z * quat_a.x + 2 * base.w * quat_a.y - 2 * base.x * quat_a.z + \
            quat_m.y * (2 * b.x * base.y + 2 * b.z * base.w) + quat_m.x * (2 * b.x * base.x + 2 * b.z * base.z) + quat_m.z * (2 * b.x * base.z - 2 * b.z * base.x);
    res.y = (-2) * base.w * quat_a.x + 2 * base.z * quat_a.y - 2 * base.y * quat_a.z + \
            quat_m.z * (2 * b.x * base.w - 2 * b.z * base.y) - quat_m.x * (2 * b.x * base.y + 2 * b.z * base.w) + quat_m.y * (2 * b.x * base.x + 2 * b.z * base.z);
    res.z = 2 * base.x * quat_a.x + 2 * base.y * quat_a.y + 2 * base.z * quat_a.z + \
            quat_m.z * (2 * b.x * base.x + 2 * b.z * base.z) - quat_m.y * (2 * b.x * base.z - 2 * b.z * base.x) - quat_m.y * (2 * b.x * base.w - 2 * b.z * base.y);
    return res;
}

//quaternion_t jac(void)
//{
//    float f_1, f_2, f_3, f_4, f_5, f_6;
//    float J_11or24, J_12or23, J_13or22, J_14or21, J_32, J_33, // objective function Jacobian elements
//        J_41, J_42, J_43, J_44, J_51, J_52, J_53, J_54, J_61, J_62, J_63, J_64; //
//    float SEqHatDot_1, SEqHatDot_2, SEqHatDot_3, SEqHatDot_4;
//    float SEq_1 = base.w;
//    float SEq_2 = base.x;
//    float SEq_3 = base.y;
//    float SEq_4 = base.z;
//    float b_x = real_magnetic_lines.x;
//    float b_z = real_magnetic_lines.z;
//
//    float m_x = measured_magnetic_lines.x;
//    float m_y = measured_magnetic_lines.y;
//    float m_z = measured_magnetic_lines.z;
//    float a_x = measured_acceleration.x;
//    float a_y = measured_acceleration.y;
//    float a_z = measured_acceleration.z;
//
//
//    float twoSEq_1 = 2.0f * SEq_1;
//    float twoSEq_2 = 2.0f * SEq_2;
//    float twoSEq_3 = 2.0f * SEq_3;
//    float twob_x = 2.0f * b_x;
//    float twob_z = 2.0f * b_z;
//    float twob_xSEq_1 = 2.0f * b_x * SEq_1;
//    float twob_xSEq_2 = 2.0f * b_x * SEq_2;
//    float twob_xSEq_3 = 2.0f * b_x * SEq_3;
//    float twob_xSEq_4 = 2.0f * b_x * SEq_4;
//    float twob_zSEq_1 = 2.0f * b_z * SEq_1;
//    float twob_zSEq_2 = 2.0f * b_z * SEq_2;
//    float twob_zSEq_3 = 2.0f * b_z * SEq_3;
//    float twob_zSEq_4 = 2.0f * b_z * SEq_4;
//    float SEq_1SEq_3 = SEq_1 * SEq_3;
//
//    float SEq_2SEq_4 = SEq_2 * SEq_4;
//
//    // compute the objective function and Jacobian
//    f_1 = twoSEq_2 * SEq_4 - twoSEq_1 * SEq_3 - a_x;
//    f_2 = twoSEq_1 * SEq_2 + twoSEq_3 * SEq_4 - a_y;
//    f_3 = 1.0f - twoSEq_2 * SEq_2 - twoSEq_3 * SEq_3 - a_z;
//    f_4 = twob_x * (0.5f - SEq_3 * SEq_3 - SEq_4 * SEq_4) + twob_z * (SEq_2SEq_4 - SEq_1SEq_3) - m_x;
//    f_5 = twob_x * (SEq_2 * SEq_3 - SEq_1 * SEq_4) + twob_z * (SEq_1 * SEq_2 + SEq_3 * SEq_4) - m_y;
//    f_6 = twob_x * (SEq_1SEq_3 + SEq_2SEq_4) + twob_z * (0.5f - SEq_2 * SEq_2 - SEq_3 * SEq_3) - m_z;
//    J_11or24 = twoSEq_3; // J_11 negated in matrix multiplication
//    J_12or23 = 2.0f * SEq_4;
//    J_13or22 = twoSEq_1; // J_12 negated in matrix multiplication
//    J_14or21 = twoSEq_2;
//    J_32 = 2.0f * J_14or21; // negated in matrix multiplication
//    J_33 = 2.0f * J_11or24; // negated in matrix multiplication
//    J_41 = twob_zSEq_3; // negated in matrix multiplication
//    J_42 = twob_zSEq_4;
//    J_43 = 2.0f * twob_xSEq_3 + twob_zSEq_1; // negated in matrix multiplication
//    J_44 = 2.0f * twob_xSEq_4 - twob_zSEq_2; // negated in matrix multiplication
//    J_51 = twob_xSEq_4 - twob_zSEq_2; // negated in matrix multiplication
//    J_52 = twob_xSEq_3 + twob_zSEq_1;
//    J_53 = twob_xSEq_2 + twob_zSEq_4;
//    J_54 = twob_xSEq_1 - twob_zSEq_3; // negated in matrix multiplication
//    J_61 = twob_xSEq_3;
//    J_62 = twob_xSEq_4 - 2.0f * twob_zSEq_2;
//    J_63 = twob_xSEq_1 - 2.0f * twob_zSEq_3;
//    J_64 = twob_xSEq_2;
//    // compute the gradient (matrix multiplication)
//    SEqHatDot_1 = J_14or21 * f_2 - J_11or24 * f_1 - J_41 * f_4 - J_51 * f_5 + J_61 * f_6;
//    SEqHatDot_2 = J_12or23 * f_1 + J_13or22 * f_2 - J_32 * f_3 + J_42 * f_4 + J_52 * f_5 + J_62 * f_6;
//    SEqHatDot_3 = J_12or23 * f_2 - J_33 * f_3 - J_13or22 * f_1 - J_43 * f_4 + J_53 * f_5 + J_63 * f_6;
//    SEqHatDot_4 = J_14or21 * f_1 + J_11or24 * f_2 - J_44 * f_4 - J_54 * f_5 + J_64 * f_6;
//
//    quaternion_t gyro_error;
//    gyro_error.w = SEqHatDot_1;
//    gyro_error.x = SEqHatDot_2;
//    gyro_error.y = SEqHatDot_3;
//    gyro_error.z = SEqHatDot_4;
//    return quatNormalize(gyro_error);
//}

static THD_WORKING_AREA(magwickThread, 512); // 256 - stack size
static THD_FUNCTION(Magwick, arg)
{
    arg = arg;              // just to avoid warnings
    systime_t time = chVTGetSystemTime();
    while( true )
    {
        //Read gyroscope and update quatrenion
        gyroReadSpeed(angular_speed);
        gyro_speed.x = angular_speed[0] * DEG2RAD;
        gyro_speed.y = angular_speed[1] * DEG2RAD;
        gyro_speed.z = -angular_speed[2] * DEG2RAD;

        //read accelerometer, update quaternion and normalize it
        accelRead(acceleration);
        measured_acceleration.x = acceleration[0];
        measured_acceleration.y = acceleration[1];
        measured_acceleration.z = acceleration[2];

        measured_acceleration = quatNormalize(measured_acceleration);

        //read magnetometer, update quaternion and normalize it
        magRead(magnetometer_data);
        measured_magnetic_lines.x = (float)magnetometer_data[0]  - 762;
        measured_magnetic_lines.y = -((float)magnetometer_data[1] + 617.5);
        measured_magnetic_lines.z = -((float)magnetometer_data[2] + 119.5);

        measured_magnetic_lines = quatNormalize(measured_magnetic_lines);


        //Evaluate difference between measured and real values
        quaternion_t f_a = quatSubtract(quatRotate(real_acceleration, quatInvert(base)), measured_acceleration);
        quaternion_t f_m = quatSubtract(quatRotate(real_magnetic_lines, quatInvert(base)), measured_magnetic_lines);

        //gradient_search
        quaternion_t gyro_error = quatNormalize(jacob(f_a, f_m));

        quaternion_t a = quatScale(quatProduct(quatScale(quatInvert(base), 2), gyro_error), TIME_STEP_MS * 0.001 * ZETA);

        gyro_drift = quatAdd(gyro_drift, a);
//        gyro_speed = quatSubtract(gyro_speed, gyro_drift);
        //translate angular speed from gyro to Earth axes
        quaternion_t gyro_earth_speed = quatScale(quatProduct(base, gyro_speed), 0.5);

        //Integrate angular speed with filter corretction
        base = quatNormalize(quatAdd(base, quatScale(quatSubtract(gyro_earth_speed, quatScale(gyro_error, BETA)), TIME_STEP_MS * 0.001)));

        //evaluate real direction of magnetic lines
        quaternion_t h = quatRotate(measured_magnetic_lines, base);
        real_magnetic_lines.x = sqrt(h.x * h.x + h.y * h.y);
        real_magnetic_lines.z = h.z;

        time = chThdSleepUntilWindowed(time, time + TIME_MS2I(TIME_STEP_MS));
    }
}

static bool isInitialized = false;

void magwickInit(tprio_t priority)
{
    if(isInitialized)
        return;
    i2cStartUp();
    gaInit();
    magInit();

    chThdCreateStatic(magwickThread, sizeof(magwickThread), priority, Magwick, NULL);
    isInitialized = true;
}

quaternion_t getQuat(void)
{
    return base;
}



