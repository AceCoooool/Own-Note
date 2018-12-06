
#ifndef TEMP_3_EXP_H
#define TEMP_3_EXP_H

// Example code, expression template, and more length equations
// for simplicity, we use struct and make all members public
#include <cstdio>

// this is expression, all expressions must inheritate it,
// and put their type in subtype
// 能够将表达式"变为指定的类型"
template<typename SubType>
struct Exp {
    // returns const reference of the actual type of this expression
    // 其实本质上为了返回一个"匹配"的左值变量
    inline const SubType &self(void) const {
        return *static_cast<const SubType *>(this);   // 指针的强制类型转换
    }
};

// binary add expression
// note how it is inheritates from Exp
// and put its own type into the template argument
// 1. 利用继承, 便可以用"强制类型转换指针"
template<typename TLhs, typename TRhs>
struct BinaryAddExp3 : public Exp<BinaryAddExp3<TLhs, TRhs> > {
    const TLhs &lhs;
    const TRhs &rhs;

    BinaryAddExp3(const TLhs &lhs, const TRhs &rhs) : lhs(lhs), rhs(rhs) {}

    // 相比lazy而言, 新加的部分---单个位置就无需额外内存空间
    // 此外将Eval定义为各自取的方式
    // evaluation function, evaluate this expression at position i
    inline float Eval(int i) const {
        return lhs.Eval(i) + rhs.Eval(i);
    }
};

// no constructor and destructor to allocate
// and de-allocate memory, allocation done by user
struct Vec3 : public Exp<Vec3> {
    int len;
    float *dptr;

    Vec3(void) {}

    Vec3(float *dptr, int len) : len(len), dptr(dptr) {}

    // here is where evaluation happens
    template<typename EType>
    inline Vec3 &operator=(const Exp<EType> &src_) {
        const EType &src = src_.self();   // 类型转换过来---这样我们就可以利用模板采用任意的类型
        for (int i = 0; i < len; ++i) {
            dptr[i] = src.Eval(i);
        }
        return *this;
    }

    // evaluation function, evaluate this expression at position i
    inline float Eval(int i) const {
        return dptr[i];
    }
};

// template add, works for any expressions
template<typename TLhs, typename TRhs>
inline BinaryAddExp3<TLhs, TRhs> operator+(const Exp<TLhs> &lhs, const Exp<TRhs> &rhs) {
    return BinaryAddExp3<TLhs, TRhs>(lhs.self(), rhs.self());
}


#endif //TEMP_3_EXP_H
