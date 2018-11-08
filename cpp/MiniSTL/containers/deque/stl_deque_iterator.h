#ifndef MINISTL_STL_DEQUE_ITERATOR_H
#define MINISTL_STL_DEQUE_ITERATOR_H

#include "../../iterator/stl_iterator.h"

namespace MiniSTL {
    // 缓冲区大小设定函数 (在预设情况下传回可容纳的元素个数)
    // 若n不为0, 则传回n, 表示由用户自定
    // 若n为0则采用预设值 预设值根据sz(元素大小)而定
    inline size_t __deque_buf_size(size_t n, size_t sz) {
        return n > 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
    }

    template<class T, class Ref, class Ptr, size_t BufSiz=0>
    struct __deque_iterator {
        // 符合迭代器的"元素" --- 符合迭代器萃取
        using iterator_category = random_access_iterator_tag;
        using value_type = T;
        using pointer = Ptr;
        using reference = Ref;
        using size_type = size_t;
        using difference_type = ptrdiff_t;

        using map_pointer = T **;
        using self = __deque_iterator;
        using iterator = __deque_iterator<T, T &, T *>;
        using const_iterator = __deque_iterator<T, const T &, const T *>;

        // 保持与容器的连接
        value_type *cur;     // 当前缓冲区的当前元素
        value_type *first;   // 当前缓冲区头
        value_type *last;    // 当前缓冲区尾(含备用空间）
        map_pointer node;    // 指向管控中心

        // 决定缓存区大小
        static size_t buffer_size() { return __deque_buf_size(BufSiz, sizeof(T)); }

        // ctor
        __deque_iterator() : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}
        __deque_iterator(pointer x, map_pointer y) : cur(x), first(*y), last(*y + buffer_size()), node(y) {}
        __deque_iterator(const iterator &rhs) : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}

        // 跳转缓冲区
        void set_node(map_pointer new_node) {
            node = new_node;
            first = *new_node;
            last = first + static_cast<difference_type>(buffer_size());
        }

        /* ----------------------------解引用运算符---------------------------- */
        reference operator*() const { return *cur; }
        pointer operator->() const { return &(operator*()); }

        // 计算两个位置距离
        difference_type operator-(const self &rhs) const {
            return buffer_size() * (node - rhs.node - 1) + (cur - first) + (rhs.last - rhs.cur);
        }

        /* ----------------------------"移动"运算符---------------------------- */
        // ++操作
        self &operator++() {
            ++cur;                //单纯指针运算
            if (cur == last) {    //若已抵达尾端
                set_node(++node);
                cur = first;
            }
            return *this;
        }

        self operator++(int) {
            self temp = *this;
            ++*this;
            return temp;
        }

        // --操作
        self &operator--() {
            if (cur == first) {
                set_node(--node);
                cur = last;
            }
            --cur;
            return *this;
        }

        self operator--(int) {
            self temp = *this;
            --*this;
            return temp;
        }

        // 移动n步操作(+=, -=, +, -)
        self &operator+=(difference_type n) {
            difference_type off_set = n + (cur - first);
            if (off_set >= 0 && off_set < static_cast<difference_type>(buffer_size())) {  // 不需要跳转
                cur += n;
            } else { // 需要注意的是off_set小于0则必然需要跳转
                difference_type node_offset = off_set > 0 ? off_set / static_cast<difference_type>(buffer_size()) :
                                              -static_cast<difference_type>((-off_set - 1) / buffer_size()) - 1;
                node += node_offset;
                cur = first + (off_set - static_cast<difference_type>(node_offset * (buffer_size())));
            }
            return *this;
        }

        self operator+(difference_type n) {
            self temp = *this;
            return temp += (n);
        }

        self &operator-=(difference_type n) {
            return *this += -n;
        }

        self operator-(difference_type n) {
            self temp = *this;
            return temp -= n;
        }

        reference operator[](difference_type n) {
            return *(*this += n);
        }

        /* ----------------------------"比较"运算符---------------------------- */
        bool operator==(const self &rhs) {
            return cur == rhs.cur;
        }

        bool operator!=(const self &rhs) {
            return !(*this == rhs);
        }

        bool operator<(const self &rhs) {  // 指针的比较: 空间位置的关系
            return node == rhs.node ? cur < rhs.cur : node < rhs.node;
        }
    };

    // using c++14 style
    template<class T, class Ref, class Ptr>
    using iterator_d = typename __deque_iterator<T, Ref, Ptr>::iterator;

    template<class T, class Ref, class Ptr>
    using const_iterator_d = typename __deque_iterator<T, Ref, Ptr>::const_iterator;

    template<class T, class Ref, class Ptr>
    using difference_type_d = typename __deque_iterator<T, Ref, Ptr>::difference_type;

    // 二元操作符
    template<class T, class Ref, class Ptr>
    iterator_d<T, Ref, Ptr> operator+(difference_type_d<T, Ref, Ptr> n, const_iterator_d<T, Ref, Ptr> &it) {
        return it + n;
    }

    template<class T, class Ref, class Ptr>
    iterator_d<T, Ref, Ptr> operator-(const_iterator_d<T, Ref, Ptr> &it, difference_type_d<T, Ref, Ptr> n) {
        return it + (-n);
    }

    template<class T, class Ref, class Ptr>
    inline difference_type_d<T, Ref, Ptr>
    operator-(const_iterator_d<T, Ref, Ptr> &it1, const_iterator_d<T, Ref, Ptr> &it2) {
        return it1.operator-(it2);
    }

    template<class T, class Ref, class Ptr>
    inline bool operator==(const_iterator_d<T, Ref, Ptr> &it1, const_iterator_d<T, Ref, Ptr> &it2) {
        return const_cast<iterator_d<T, Ref, Ptr> &>(it1).operator==(it2);
    }

    template<class T, class Ref, class Ptr>
    inline bool operator!=(const_iterator_d<T, Ref, Ptr> &it1, const_iterator_d<T, Ref, Ptr> &it2) {
        return const_cast<iterator_d<T, Ref, Ptr> &>(it1).operator!=(it2);
    }
}

#endif //MINISTL_STL_DEQUE_ITERATOR_H
