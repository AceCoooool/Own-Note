#ifndef MINISTL_STL_DEQUE_H
#define MINISTL_STL_DEQUE_H

#include "stl_deque_iterator.h"
#include "../../allocator/stl_alloc.h"

namespace MiniSTL {
    template<class T, class Alloc = simpleAlloc<T>, size_t BufSiz = 0>
    class deque {
    public:
        /* ---------------------------类型别名----------------------- */
        using value_type = T;
        using pointer = T *;
        using reference = T &;
        using const_reference = const T &;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using iterator = typename __deque_iterator<T, T &, T *>::iterator;
        using const_iterator = typename __deque_iterator<T, T &, T *>::const_iterator;
//        using reverse_iterator = reverse_iterator<iterator>;
//        using const_reverse_iterator = MiniSTL::reverse_iterator<const_iterator>;

    private:
        using map_pointer = pointer *;  // 元素的指针的指针
        // 专属空间配置器
        using data_allocator = simpleAlloc<value_type>;  // 每次配置一个元素大小
        using map_allocator = simpleAlloc<pointer>;      // 每次配置一个指针大小

        /* ---------------------------成员变量----------------------- */
        iterator start;      // 第一个节点
        iterator finish;     // 最后一个节点
        map_pointer map;     // 指向map, map是块连续空间(其每个元素指向一个缓冲区)
        size_type map_size;  // map内有多少指针

    private:
        /* ------------------------内置辅助函数-------------------- */
        size_type initial_map_size() { return 8U; }
        // 缓存区大小
        size_type buffer_size() const { return iterator::buffer_size(); }
        // 分配一个缓冲区
        value_type *allocate_node() { return data_allocator::allocate(buffer_size()); }
        // 释放一个缓冲区
        void deallocate_node(value_type *p) { data_allocator::deallocate(p, buffer_size()))); }
        // 产生并安排好deque结构
        void create_map_and_nodes(size_type);
        void fill_initialized(size_type, const value_type &value = value_type());
        void reallocate_map(size_type, bool);
        void reverse_map_at_back(size_type nodes_to_add = 1);
        void reverse_map_at_front(size_type nodes_to_add = 1);
        void push_back_aux(const value_type &);
        void push_front_aux(const value_type &);
        void pop_back_aux();
        void pop_front_aux();
        iterator insert_aux(iterator, const value_type &);

    };
}

#endif //MINISTL_STL_DEQUE_H
