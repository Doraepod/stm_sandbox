#ifndef HEADERS_MAGWICK_H_
#define HEADERS_MAGWICK_H_

#include "quaternions.h"
#include "lld_gyroscope.h"
#include "lld_accelerometer.h"



#define DEG2RAD 3.14 / 180
#define RAD2DEG 180 / 3.14
#define TIME_STEP 50

void magwickInit(tprio_t priority);


quaternion_t getQuat(void);






#endif /* HEADERS_MAGWICK_H_ */
