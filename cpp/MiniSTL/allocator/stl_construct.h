#ifndef MINISTL_STL_CONSTRUCT_H
#define MINISTL_STL_CONSTRUCT_H

#include "../iterator/type_traits.h"

namespace MiniSTL {
    // 将初值value设定到指针所指的空间上.
    template<class T1, class T2>
    inline void construct(T1 *ptr1, const T2 &value) {
        new(ptr1) T1(value);  // placement new; 调用T1::T1(value)
    }

    // destroy()第一个版本, 接受一个指针, 准备将该指针所指之物析构掉
    template<class T>
    inline void destroy(T *ptr) {
        ptr->~T();
    }

    /* ----------------------------- 第二个版本的destroy -------------------------*/
    // 设法利用traits批量析构对象
    template<class ForwardIterator>
    inline void destroy(ForwardIterator beg, ForwardIterator end) {
        _destroy_aux(beg, end, is_POD_type_t<ForwardIterator>());
    }

    // 如果元素的value_type不存在non—trivial destructor
    template<class ForwardIterator>
    inline void _destroy_aux(ForwardIterator beg, ForwardIterator end, _false_type) {
        for (; beg != end; ++beg)
            destroy(&*beg);   // 毕竟迭代器不是真正的地址
    }

    // 存在trivial destructor (如果对象的析构函数无关痛痒, 那么反复调用它是一种效率上的巨大浪费)
    template<class ForwardIterator>
    inline void _destroy_aux(ForwardIterator, ForwardIterator, _true_type) {}

    // 针对char*, wchar_t*的特化
    inline void destroy(char *, char *) {}
    inline void destroy(wchar_t *, wchar_t *) {}
}

#endif //MINISTL_STL_CONSTRUCT_H
