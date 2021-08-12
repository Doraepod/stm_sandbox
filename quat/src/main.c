#include "ch.h"
#include "hal.h"
#include "quaternions.h"
#include "debug.h"
#include "lld_gyroscope.h"

#define DEG2RAD 3.14 / 180
#define RAD2DEG 180 / 3.14
#define TIME_STEP 50

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

int main(void) {

    halInit();
    chSysInit();
    gyroscopeInit(NORMALPRIO);


    debug_stream_init();
    dbgprintf("Test\n\r");
    systime_t time = chVTGetSystemTime();
    while (true) {
        gyro_speed.x = getGyroSpeed(GYRO_AXIS_X) * DEG2RAD;
        gyro_speed.y = getGyroSpeed(GYRO_AXIS_Y) * DEG2RAD;
        gyro_speed.z = getGyroSpeed(GYRO_AXIS_Z) * DEG2RAD;

        gyro_earth_speed = quatScale(quatProduct(gyro_angle, gyro_speed), 0.5);
        gyro_angle = quatAdd(gyro_angle, quatScale(gyro_earth_speed, TIME_STEP * 0.001));
        euler_angles_t angles = quat2Euler(gyro_angle);
        dbgprintf("w = %d, x = %d, y = %d, z = %d\r\nyaw = %d, pitch = %d, roll = %d\r\n", (int)(gyro_angle.w*100), (int)(gyro_angle.x*100), (int)(gyro_angle.y*100), (int)(gyro_angle.z*100), (int)(angles.yaw * RAD2DEG), (int)(angles.pitch * RAD2DEG), (int)(angles.roll * RAD2DEG));
        time = chThdSleepUntilWindowed(time, time + TIME_MS2I(TIME_STEP));
    }
}
