#include "ch.h"
#include "hal.h"
#include "quaternions.h"
#include "debug.h"
#include "magwick.h"

#define DEG2RAD 3.14 / 180
#define RAD2DEG 180 / 3.14
#define TIME_STEP 50



int main(void) {

    halInit();
    chSysInit();
    magwickInit(NORMALPRIO);

    debug_stream_init();
    dbgprintf("Test\n\r");
    while (true) {
        euler_angles_t angles = quat2Euler(getQuat());
        dbgprintf("yaw = %d, pitch = %d, roll = %d\r\n", (int)(angles.yaw * RAD2DEG), (int)(angles.pitch * RAD2DEG), (int)(angles.roll * RAD2DEG));
        chThdSleepMilliseconds(200);
    }
}
