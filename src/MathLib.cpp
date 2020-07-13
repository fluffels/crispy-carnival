#include <math.h>

#include "Math.h"

struct Quaternion {
    float w;
    float x;
    float y;
    float z;
};

inline float quaternionMagnitude(Quaternion& q) {
    float result;

    result += powf(q.w, 2);
    result += powf(q.x, 2);
    result += powf(q.y, 2);
    result += powf(q.z, 2);
    result /= sqrtf(result);

    return result;
}

inline void quaternionNormalize(Quaternion& q) {
    float magnitude = quaternionMagnitude(q);
    q.w /= magnitude;
    q.x /= magnitude;
    q.y /= magnitude;
    q.z /= magnitude;
}

inline Quaternion quaternionMultiply(Quaternion& q1, Quaternion& q2) {
    Quaternion r;

    r.w = q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z;
    r.x = q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y;
    r.y = q1.w*q2.y - q1.x*q2.z + q1.y*q2.w + q1.z*q2.x;
    r.z = q1.w*q2.z + q1.x*q2.y - q1.y*q2.x + q1.z*q2.w;

    return r;
}

inline Quaternion quaternionRotation(float x, float y, float z, float angle) {
    Quaternion r;

    r.w = cosf(angle/2);
    r.x = x * sinf(angle/2);
    r.y = y * sinf(angle/2);
    r.z = z * sinf(angle/2);

    return r;
}

inline void quaternionToMatrix(Quaternion& q, float* m) {
    *m++ = pow(q.w, 2) + pow(q.x, 2) - pow(q.y, 2) - pow(q.z, 2);
    *m++ = 2*q.x*q.y - 2*q.w*q.z;
    *m++ = 2*q.x*q.z + 2*q.w*q.y;
    *m++ = 0;

    *m++ = 2*q.x*q.y + 2*q.w*q.z;
    *m++ = pow(q.w, 2) - pow(q.x, 2) + pow(q.y, 2) - pow(q.z, 2);
    *m++ = 2*q.y*q.z + 2*q.w*q.x;
    *m++ = 0;

    *m++ = 2*q.x*q.z - 2*q.w*q.y;
    *m++ = 2*q.y*q.z - 2*q.w*q.x;
    *m++ = pow(q.w, 2) - pow(q.x, 2) - pow(q.y, 2) + pow(q.z, 2);
    *m++ = 0;

    *m++ = 0;
    *m++ = 0;
    *m++ = 0;
    *m++ = 1;
}
