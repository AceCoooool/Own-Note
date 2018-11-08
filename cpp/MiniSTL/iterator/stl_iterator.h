#ifndef MINISTL_STL_ITERATOR_H
#define MINISTL_STL_ITERATOR_H

#include <cstddef> //ptrdiff_t定义
#include <iostream>

namespace MiniSTL {
    // 五种迭代器类型的标记
    struct input_iterator_tag {};
    struct output_iterator_tag {};
    struct forward_iterator_tag : public input_iterator_tag {};
    struct bidirectional_iterator_tag : public forward_iterator_tag {};
    struct random_access_iterator_tag : public bidirectional_iterator_tag {};

    // 为了避免遗忘记录traits, 此基类以供自定义iterator继承之
    template<class Category, class T, class Distance = ptrdiff_t, class Pointer = T *, class Reference = T &>
    struct iterator {
        using iterator_category = Category;
        using value_type = T;
        using difference_type = Distance;
        using pointer = Pointer;
        using reference = Reference;
    };

    // traits 展现Iterator所有特性
    // 之所以不直接使用别名模板完全替代是因为存在偏特化版本，（似乎别名模板无法偏特化）
    template<class Iterator>
    struct iterator_traits {
        using iterator_category = typename Iterator::iterator_category;
        using value_type = typename Iterator::value_type;
        using difference_type = typename Iterator::difference_type;
        using pointer = typename Iterator::pointer;
        using reference = typename Iterator::reference;
    };

    //针对raw pointer设计的偏特化版本
    template<class T>
    struct iterator_traits<T *> {
        using iterator_category = random_access_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T *;
        using reference = T &;
    };

    //针对pointer-to-const设计的偏特化版本
    template<class T>
    struct iterator_traits<const T *> {
        using iterator_category = random_access_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = const T *;
        using reference = const T &;
    };

    // 别名模板c++14
    template<class Iterator>
    using iterator_category_t = typename iterator_traits<Iterator>::iterator_category;

    template<class Iterator>
    using value_type_t = typename iterator_traits<Iterator>::value_type;

    template<class Iterator>
    using difference_type_t = typename iterator_traits<Iterator>::difference_type;

    template<class Iterator>
    using pointer_t = typename iterator_traits<Iterator>::pointer;

    template<class Iterator>
    using reference_t = typename iterator_traits<Iterator>::reference;

    //以下为整组distance函数
    template<class InputIterator>
    inline difference_type_t<InputIterator> __distance(InputIterator first, InputIterator last, input_iterator_tag) {
        difference_type_t<InputIterator> n = 0;
        while (first != last)
            ++first, ++n;
        return n;
    }

    template<class InputIterator>
    inline difference_type_t<InputIterator>
    __distance(InputIterator first, InputIterator last, random_access_iterator_tag) {
        return last-first;
    }

    template<class InputIterator>
    inline difference_type_t<InputIterator> distance(InputIterator first, InputIterator last) {
        return __distance(first, last, iterator_category_t<InputIterator>());
    }

    //以下为整组advance函数
    template<class InputIterator, class Distance>
    inline void __advance(InputIterator &i, Distance n, input_iterator_tag) {
        while (n--) ++i;
    }

    template<class InputIterator, class Distance>
    inline void __advance(InputIterator &i, Distance n, bidirectional_iterator_tag) {
        if (n >= 0)
            while (n--) ++i;
        else
            while (n++) --i;
    }

    template<class InputIterator, class Distance>
    inline void __advance(InputIterator &i, Distance n, random_access_iterator_tag) {
        i += n;
    }

    template<class InputIterator, class Distance>
    inline void advance(InputIterator &i, Distance n) {
        __advance(i, n, iterator_category_t<InputIterator>());
    }

    // 以下为三种迭代器适配器
    // insert,reverse,stream


}


#endif //MINISTL_STL_ITERATOR_H
