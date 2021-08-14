#include "quaternions.h"

quaternion_t quatProduct(quaternion_t first, quaternion_t second)
{
    quaternion_t res;
    res.w = first.w * second.w - first.x * second.x - first.y * second.y - first.z * second.z;
    res.x = first.w * second.x + first.x * second.w + first.y * second.z - first.z * second.y;
    res.y = first.w * second.y - first.x * second.z + first.y * second.w - first.z * second.x;
    res.z = first.w * second.z + first.x * second.y + first.y * second.x - first.z * second.w;
    return res;
}

quaternion_t quatInvert(quaternion_t quat)
{
    quaternion_t res;
    res.w = quat.w;
    res.x = -quat.x;
    res.y = -quat.y;
    res.z = -quat.z;
    return res;
}

quaternion_t quatRotate(quaternion_t axis, quaternion_t rotor)
{
    quaternion_t res;
    res = quatProduct(rotor, axis);
    res = quatProduct(res, quatInvert(rotor));
    return res;
}

quaternion_t quatScale(quaternion_t quat, float scaler)
{
    quaternion_t res;
    res.w = quat.w * scaler;
    res.x = quat.x * scaler;
    res.y = quat.y * scaler;
    res.z = quat.z * scaler;
    return res;
}

quaternion_t quatAdd(quaternion_t first, quaternion_t second)
{
    quaternion_t res;
    res.w = first.w + second.w;
    res.x = first.x + second.x;
    res.y = first.y + second.y;
    res.z = first.z + second.z;
    return res;
}

quaternion_t quatSubtract(quaternion_t first, quaternion_t second)
{
    quaternion_t res;
    res.w = first.w - second.w;
    res.x = first.x - second.x;
    res.y = first.y - second.y;
    res.z = first.z - second.z;
    return res;
}

euler_angles_t quat2Euler(quaternion_t quat)
{
    euler_angles_t res;
    res.yaw = atan2(2 * (quat.z * quat.w + quat.x * quat.y), 1 - 2 * (quat.y * quat.y + quat.z * quat.z));
    res.pitch = asin(2 * (quat.y * quat.w - quat.z * quat.x));
    res.roll = atan2(2 * (quat.x * quat.w + quat.y * quat.z), 1 - 2 * (quat.x * quat.x + quat.y * quat.y));
    return res;
}

quaternion_t quatNormalize(quaternion_t quat)
{
    quaternion_t res;
    float length = sqrt(quat.w * quat.w + quat.x * quat.x + quat.y * quat.y + quat.z * quat.z);
    res = quatScale(quat, 1 / length);
    return res;
}
