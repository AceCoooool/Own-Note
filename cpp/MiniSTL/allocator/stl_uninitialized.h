#ifndef MINISTL_STL_UNINITIALIZED_H
#define MINISTL_STL_UNINITIALIZED_H

#include "stl_construct.h"
#include "../iterator/type_traits.h"
#include "../iterator/stl_iterator.h"
#include <cstring>   // memcpy, memmove

namespace MiniSTL {
    /* ------将[first, last)复制到result中: 这里result的空间足够容纳----- */
    template<class InputIterator, class ForwardIterator>
    inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
        return __uninitialized_copy_aux(first, last, result, is_POD_type_t<InputIterator>());
    }

    template<class InputIterator, class ForwardIterator>
    inline ForwardIterator
    __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, _true_type) {
        memcpy(result, first, (last - first) * sizeof(*first));
        return result + (last - first);
    }

    template<class InputIterator, class ForwardIterator>
    inline ForwardIterator
    __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, _false_type) {
        ForwardIterator cur = result;
        for (; first != last; ++cur, ++first)
            construct(&*cur, *first);
        return cur;
    }

    //针对char*, wchar_t*存在特化版本 memmove直接移动内存
    inline char *uninitialized_copy(char *first, char *last, char *result) {
        memmove(result, first, last - first);
        return result + (last - first);
    }

    inline wchar_t *uninitialized_copy(wchar_t *first, wchar_t *last, wchar_t *result) {
        memmove(result, first, sizeof(wchar_t) * (last - first));
        return result + (last - first);
    }

    /* -------------------将[first, last)用value填充----------------- */
    template<class ForwardIterator, class T>
    inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &value) {
        __uninitialized_fill_aux(first, last, value, is_POD_type_t<ForwardIterator>());
    }

    template<class ForwardIterator, class T>
    inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T &value, _true_type) {
        std::fill(first, last, value);   // TODO: change after ch7
    }

    template<class ForwardIterator, class T>
    void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T &value, _false_type) {
        ForwardIterator cur = first;
        for (; cur != last; ++cur)
            construct(&*cur, value);
    }

    /* ---------------将[first, first+n)用value填充-------------------- */
    template<class ForwardIterator, class Size, class T>
    inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T &value) {
        return __uninitialized_fill_n_aux(first, n, value, is_POD_type_t<ForwardIterator>());
    }

    template<class ForwardIterator, class Size, class T>
    inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T &value, _true_type) {
        return std::fill_n(first, n, value);  // TODO: change after ch7
    }

    template<class ForwardIterator, class Size, class T>
    ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T &value, _false_type) {
        // 忽略异常处理 --- 明确的是一旦一个对象构造失败则需要析构所有对象
        ForwardIterator cur = first;
        for (; n > 0; --n, ++cur)
            construct(&*cur, value);
        return cur;
    }
}


#endif //MINISTL_STL_UNINITIALIZED_H
