#include <math.h>

#include "MathLib.h"

inline void matrixInit(float* m) {
    *m++ = 1;
    *m++ = 0;
    *m++ = 0;
    *m++ = 0;

    *m++ = 0;
    *m++ = 1;
    *m++ = 0;
    *m++ = 0;

    *m++ = 0;
    *m++ = 0;
    *m++ = 1;
    *m++ = 0;

    *m++ = 0;
    *m++ = 0;
    *m++ = 0;
    *m++ = 1;
}

inline void matrixCopy(float* s, float* d) {
    for (int i = 0; i < 16; i++) {
        *d++ = *s++;
    }
}

inline void matrixTranslate(float x, float y, float z, float* m) {
    m[12] += x;
    m[13] += y;
    m[14] += z;
}

inline void matrixMultiply(float* m, float* n, float* r) {
    for (int nIdx = 0; nIdx <= 12; nIdx += 4) {
        for (int mIdx = 0; mIdx <= 3; mIdx++) {
            *r = 0;
            for (int i = 0; i <= 3; i++) {
                *r += m[mIdx + i*4] * n[nIdx + i];
            }
            r++;
        }
    }
}

inline float quaternionMagnitude(Quaternion& q) {
    float result = 0;

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

inline Quaternion quaternionRotate(float x, float y, float z, float angle) {
    Quaternion r;

    r.w = cosf(angle/2);
    r.x = x * sinf(angle/2);
    r.y = y * sinf(angle/2);
    r.z = z * sinf(angle/2);

    return r;
}

inline void quaternionToMatrix(Quaternion& q, float* m) {
    *m++ = powf(q.w, 2) + powf(q.x, 2) - powf(q.y, 2) - powf(q.z, 2);
    *m++ = 2*q.x*q.y + 2*q.w*q.z;
    *m++ = 2*q.x*q.z - 2*q.w*q.y;
    *m++ = 0;

    *m++ = 2*q.x*q.y - 2*q.w*q.z;
    *m++ = powf(q.w, 2) - powf(q.x, 2) + powf(q.y, 2) - powf(q.z, 2);
    *m++ = 2*q.y*q.z - 2*q.w*q.x;
    *m++ = 0;

    *m++ = 2*q.x*q.z + 2*q.w*q.y;
    *m++ = 2*q.y*q.z + 2*q.w*q.x;
    *m++ = powf(q.w, 2) - powf(q.x, 2) - powf(q.y, 2) + powf(q.z, 2);
    *m++ = 0;

    *m++ = 0;
    *m++ = 0;
    *m++ = 0;
    *m++ = 1;
}
