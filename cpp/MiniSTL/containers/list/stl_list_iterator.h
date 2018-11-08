#ifndef MINISTL_STL_LIST_ITERATOR_H
#define MINISTL_STL_LIST_ITERATOR_H

#pragma once
#include "../../iterator/stl_iterator.h"

namespace MiniSTL {
    /* ---------------------list节点--------------------- */
    template<class T>
    struct __list_node {
        using void_pointer = void *;
        void_pointer prev;  // 其实可以设置为__list_node<T>*
        void_pointer next;
        T data;
    };

    /* ---------------------list迭代器--------------------- */
    template<class T, class Ref, class Ptr>
    struct __list_iterator {
        using iterator = __list_iterator<T, T &, T *>;
        using self = __list_iterator<T, Ref, Ptr>;

        using iterator_category = bidirectional_iterator_tag;
        using value_type = T;
        using pointer = Ptr;
        using reference = Ref;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using link_type = __list_node<T> *;

        link_type node;   // 迭代器内部存在一个raw pointer指向__list_node

        // constructor
        __list_iterator(link_type x) : node(x) {}
        __list_iterator() {}
        __list_iterator(const iterator &x) : node(x.node) {}

        // __list_iterator析构函数为trivial

        bool operator==(const self &rhs) const { return node == rhs.node; }
        bool operator!=(const self &rhs) const { return node != rhs.node; }

        // 重载*, 返回链表节点的值
        reference operator*() const { return node->data; }
        //member access
        pointer operator->() const { return &(operator*()); }
        //自增
        self &operator++() {
            node = static_cast<link_type>(node->next);  // 将__link_node*强制转为迭代器
            return *this;
        }

        self operator++(int i) {
            self temp = *this;
            ++(*this);
            return temp;
        }
        //自减
        self &operator--() {
            node = static_cast<link_type>(node->prev);  // 将__link_node*强制转为迭代器
            return *this;
        }

        self operator--(int i) {
            self temp = *this;
            --(*this);
            return temp;
        }
    };
}

#endif //MINISTL_STL_LIST_ITERATOR_H
