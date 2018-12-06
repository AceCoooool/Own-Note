#ifndef TEMP_1_NAIVE_H
#define TEMP_1_NAIVE_H

#include <cstring>

struct Vec1 {
    int len;
    float *dptr;

    Vec1(int len) : len(len) {
        dptr = new float[len];
    }

    Vec1(const float *src, int len) : len(len) {
        dptr = new float[len];
        memcpy(dptr, src, sizeof(float) * len);
    }

    Vec1(const Vec1 &src) : len(src.len) {
        dptr = new float[len];
        memcpy(dptr, src.dptr, sizeof(float) * len);
    }

    ~Vec1(void) {
        delete[] dptr;
    }
};

inline Vec1 operator+(const Vec1 &lhs, const Vec1 &rhs) {
    Vec1 res(lhs.len);
    for (int i = 0; i < lhs.len; ++i) {
        res.dptr[i] = lhs.dptr[i] + rhs.dptr[i];
    }
    return res;
}

#endif //TEMP_1_NAIVE_H
