#ifndef MINISTL_STL_ALLOC_H
#define MINISTL_STL_ALLOC_H

#include <cstddef>  // ptrdiff_t
#include "stl_alloc_second.h"

namespace MiniSTL {
    //默认二级适配器
    template<class T, class Alloc = __default_alloc>
    class simpleAlloc {
    public:
        using value_type = T;
        using pointer = T *;
        using const_pointer = const T *;
        using reference = T &;
        using const_refernce = const T &;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
    public:
        // 内存分配和释放
        static T *allocate();
        static T *allocate(size_t n);
        static void deallocate(T *ptr);
        static void deallocate(T *ptr, size_t n);

        // 构造和析构
        static void construct(T *ptr);
        static void construct(T *ptr, const T &value);
        static void destroy(T *ptr);
        static void destroy(T *first, T *last);
    };

    template<class T, class Alloc>
    T *simpleAlloc<T, Alloc>::allocate() {
        return static_cast<T *>(Alloc::allocate(sizeof(T)));
    }

    template<class T, class Alloc>
    T *simpleAlloc<T, Alloc>::allocate(size_t n) {
        if (n == 0) return 0;
        return static_cast<T *>(Alloc::allocate(sizeof(T) * n));
    }

    template<class T, class Alloc>
    void simpleAlloc<T, Alloc>::deallocate(T *ptr) {
        Alloc::deallocate(static_cast<void *>(ptr), sizeof(T));
    }

    template<class T, class Alloc>
    void simpleAlloc<T, Alloc>::deallocate(T *ptr, size_t n) {
        if (n == 0) return;
        Alloc::deallocate(static_cast<void *>(ptr), sizeof(T) * n);
    }

    template<class T, class Alloc>
    void simpleAlloc<T, Alloc>::construct(T *ptr) {
        new(ptr) T();
    }

    template<class T, class Alloc>
    void simpleAlloc<T, Alloc>::construct(T *ptr, const T &value) {
        new(ptr) T(value);
    }

    template<class T, class Alloc>
    void simpleAlloc<T, Alloc>::destroy(T *ptr) {
        ptr->~T();
    }

    template<class T, class Alloc>
    void simpleAlloc<T, Alloc>::destroy(T *first, T *last) {
        for (; first != last; ++first)
            first->~T();
    }

}

#endif //MINISTL_STL_ALLOC_H
