#ifndef HEADERS_MAGWICK_H_
#define HEADERS_MAGWICK_H_

#include "quaternions.h"
#include "lld_gyro_axel.h"
#include "lld_magnetometer.h"
#include "debug.h"
#include "math.h"



#define DEG2RAD 3.14 / 180
#define RAD2DEG 180 / 3.14
#define TIME_STEP_MS 100
#define TIME_STEP_S TIME_STEP_MS * 0.001

void magwickInit(tprio_t priority);


quaternion_t getQuat(void);






#endif /* HEADERS_MAGWICK_H_ */
