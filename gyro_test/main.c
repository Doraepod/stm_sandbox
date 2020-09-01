#include "include_all.h"

float   angular_speed[3] = {0, 0, 0};
float   gyro_mean_error[3] = {0, 0, 0};
float   angle[3] = {0, 0, 0};

int main(void)
{
    chSysInit();
    halInit();
    i2c_start();

    lcdInit();
    lcdClear();
    gyroscope_init();

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
            angle[i] += angular_speed[i] * 0.2;
        }
        lcdClear();
        lcdSendNumber(0, 0, angle[0]);
        lcdSendNumber(8, 0, angle[1]);
        lcdSendNumber(0, 1, angle[2]);

        chThdSleepUntil(time += MS2ST(200));

    }
}
