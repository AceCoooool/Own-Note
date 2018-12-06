#ifndef TEMP_2_LAZY_H
#define TEMP_2_LAZY_H

// Example Lazy evaluation code
// for simplicity, we use struct and make all members public
#include <cstdio>

struct Vec2;

// expression structure holds the expression
struct BinaryAddExp2 {
    const Vec2 &lhs;
    const Vec2 &rhs;

    BinaryAddExp2(const Vec2 &lhs, const Vec2 &rhs)
            : lhs(lhs), rhs(rhs) {}
};

// no constructor and destructor to allocate and de-allocate memory,
//  allocation done by user
struct Vec2 {
    int len;
    float *dptr;

    Vec2(void) {}

    Vec2(float *dptr, int len)
            : len(len), dptr(dptr) {}

    // here is where evaluation happens
    inline Vec2 &operator=(const BinaryAddExp2 &src) {
        for (int i = 0; i < len; ++i) {
            dptr[i] = src.lhs.dptr[i] + src.rhs.dptr[i];
        }
        return *this;
    }
};

// no evaluation happens here
inline BinaryAddExp2 operator+(const Vec2 &lhs, const Vec2 &rhs) {
    return BinaryAddExp2(lhs, rhs);
}

#endif //TEMP_2_LAZY_H
