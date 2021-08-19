#include "ch.h"
#include "hal.h"
#include "quaternions.h"
#include "debug.h"
#include "magwick.h"
#include "lld_gyro_axel.h"
#include "lld_magnetometer.h"
#include <Fusion.h>
#include "i2c.h"


#define DEG2RAD 3.14 / 180
#define RAD2DEG 180 / 3.14


#ifdef SELF_MADE

FusionAhrs fusionAhrs;

FusionVector3 accel;
FusionVector3 mag;
FusionVector3 gyro;
int main(void) {

    halInit();
    chSysInit();
    //magwickInit(NORMALPRIO);
    FusionAhrsInitialise(&fusionAhrs, 0.5f);

    i2cStartUp();
    magInit();
    gaInit();

    debug_stream_init();
    dbgprintf("Test\n\r");
    chThdSleepMilliseconds(100);
    while (true) {
        int16_t buf[3];
        magRead(buf);

        mag.axis.x = buf[0];
        mag.axis.y = -buf[1];
        mag.axis.z = buf[2];
        accelRead(buf);
        accel.axis.x = buf[0];
        accel.axis.y = buf[1];
        accel.axis.z = buf[2];
        gyroRead(buf);
        gyro.axis.x = buf[0];
        gyro.axis.y = buf[1];
        gyro.axis.z = buf[2];

        FusionAhrsUpdate(&fusionAhrs, gyro, accel, mag, 0.2);
        FusionEulerAngles angles = FusionQuaternionToEulerAngles(FusionAhrsGetQuaternion(&fusionAhrs));
        //euler_angles_t angles = quat2Euler(getQuat());
        dbgprintf("yaw = %d, pitch = %d, roll = %d\r\n", (int)(angles.angle.yaw), (int)(angles.angle.pitch), (int)(angles.angle.roll));
        chThdSleepMilliseconds(200);

    }
#else
int main(void) {

    halInit();
    chSysInit();
    magwickInit(NORMALPRIO);
    debug_stream_init();
    dbgprintf("Test\n\r");
    while(1)
    {
        euler_angles_t angles = quat2Euler(getQuat());
        dbgprintf("yaw = %d, pitch = %d, roll = %d\r\n", (int)(angles.yaw * RAD2DEG), (int)(angles.pitch * RAD2DEG), (int)(angles.roll * RAD2DEG));
        chThdSleepMilliseconds(200);
    }
#endif
}
