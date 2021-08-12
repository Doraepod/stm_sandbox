#ifndef INCLUDE_QUATERNIONS_H_
#define INCLUDE_QUATERNIONS_H_

#include "math.h"

typedef struct {
    float w;
    float x;
    float y;
    float z;
}quaternion_t;

typedef struct {
    float yaw;
    float pitch;
    float roll;
}euler_angles_t;

quaternion_t quatProduct(quaternion_t first, quaternion_t second);
quaternion_t quatRotate(quaternion_t axis, quaternion_t rotor);
quaternion_t quatInvert(quaternion_t quat);
quaternion_t quatScale(quaternion_t quat, float scaler);
quaternion_t quatAdd(quaternion_t first, quaternion_t second);
quaternion_t quatNormalize(quaternion_t quat);
euler_angles_t quat2Euler(quaternion_t quat);








#endif
