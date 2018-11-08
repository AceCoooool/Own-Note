#ifndef MINISTL_STL_DEQUE_IMP_H
#define MINISTL_STL_DEQUE_IMP_H

#include "stl_deque.h"

namespace MiniSTL {
    /* ------------------------内置辅助函数-------------------- */
    // 产生并安排好deque结构
    template<class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::create_map_and_nodes(size_type n) {
        // 所需节点数=(元素个数/每个缓冲区可容纳的元素个数)+1
        size_type num_nodes = n / iterator::buffer_size() + 1;
        // 一个map要管理几个节点, 最少8个, 最多所需节点加2
        map_size = max(initial_map_size(), num_nodes + 2);
        map = map_allocator::allocate(map_size);
        // 令nstart与nfinish指向map所拥有的全部node的最中央区段, 以便日后扩充头尾
        map_pointer nstart = map + (map_size - num_nodes) / 2;
        map_pointer nfinish = nstart + num_nodes - 1;

        map_pointer cur;
        try { // 为每一个节点配置空间
            for (cur = nstart; cur <= nfinish; ++cur)
                *cur = allocate_node();
        } catch (std::exception &) {
            clear();
        }
        start.set_node(nstart);
        finish.set_node(nfinish);
        start.cur = start.first;
        finish.cur = finish.first + n % buffer_size();// 若n%buffer_size==0,会多配置一个节点, 此时cur指向该节点头部
    }

    template<class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::fill_initialized(size_type n, const value_type &value) {
        create_map_and_nodes(n);
        map_pointer cur;
        try {  //为每个缓冲区设定初值
            for (cur = start.node; cur < finish.node; ++cur)
                uninitialized_fill(*cur, *cur + iterator::buffer_size(), value);
            // 最后一个缓冲区只设定至需要处
            uninitialized_fill(finish.first, finish.cur, value);
        } catch (std::exception &) {
            clear();
        }
    }

    


}

#endif //MINISTL_STL_DEQUE_IMP_H
