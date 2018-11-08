#ifndef MINISTL_STL_ALLOC_SECOND_IMP_H
#define MINISTL_STL_ALLOC_SECOND_IMP_H

#include "stl_alloc_second.h"

namespace MiniSTL {

    // 当free_list无可用区块时, 重新填充空间.
    // 新空间取自内存池, 默认获取20个节点(区块)
    // 若内存池不足, 则获取的将小于20
    void *__default_alloc::refill(size_t n) {
        int nobjs = 20;
        // 尝试调用chunk_alloc, 注意nobjs以pass-by-reference传入
        char *chunk = chunk_alloc(n, nobjs);
        obj *volatile *my_free_list;
        obj *result;
        obj *current_obj, *next_obj;

        // 若只获取了一个区块则直接分配给调用者, 不加入free_list
        if (1 == nobjs)
            return (chunk);

        // 在chunk空间内建立free_list
        my_free_list = free_list + FREELIST_INDEX(n);
        result = reinterpret_cast<obj *>(chunk);
        // 引导free_list指向内存池分配的空间
        // chunk指向的内存直接分给用户, free_list指向剩下（19或更少）的区块
        *my_free_list = next_obj = reinterpret_cast<obj *>(chunk + n);
        for (int i = 1; i != nobjs - 1; ++i) {
            current_obj = next_obj;
            next_obj = reinterpret_cast<obj *>(reinterpret_cast<char *>(next_obj) + n);
            current_obj->free_list_link = next_obj;
        }
        next_obj->free_list_link = nullptr;
        return result;
    }

    // 默认size为8的整数倍
    char *__default_alloc::chunk_alloc(size_t size, int &nobjs) {
        char *result;
        size_t total_bytes = size * nobjs;
        size_t bytes_left = end_free - start_free;  // 内存池剩余空间
        if (bytes_left > total_bytes) {  // case1: 容量满足需求
            result = start_free;
            start_free += total_bytes;
            return result;
        } else if (bytes_left > size) {  // case2: 容量至少满足一个区块需求
            nobjs = bytes_left / size;
            total_bytes = size * nobjs;
            result = start_free;
            start_free += total_bytes;
            return result;
        } else {   // case3: 内存池一个区块都无法提供
            // 向heap申请注入的内存, heap_size将随着配置次数增加而增加
            size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
            if (bytes_left > 0) {   // 当前内存池还有一部分内存, 为了不浪费分配给free_list
                obj *volatile *my_free_list = free_list + FREELIST_INDEX(bytes_left);
                reinterpret_cast<obj *>(start_free)->free_list_link = *my_free_list;
                *my_free_list = reinterpret_cast<obj *>(start_free);
            }
            // 配置heap空间以补充内存池
            start_free = static_cast<char *>(malloc(bytes_to_get));
            if (!start_free) {  // heap空间不足, malloc失败
                obj *volatile *my_free_list, *p;
                // 在free_list中检查是否有符合需求的区块
                for (int i = size; i <= __MAX_BYTES; i += __ALIGN) {
                    my_free_list = free_list + FREELIST_INDEX(i);
                    p = *my_free_list;
                    if (p != nullptr) {  // 存在足以分配的区块
                        *my_free_list = p->free_list_link;   // 抽离当前区块
                        start_free = reinterpret_cast<char *>(p);
                        end_free = start_free + i;
                        return (chunk_alloc(size, nobjs)); // 递归调用以修正nobjs, 此时必然进入else_if分支
                    }
                }
                end_free = nullptr;            // 到处都找不到内存
                // 调用第一级适配器观察其能否分配内存, 或抛出异常(主要看oom机制能否给力)
                start_free = static_cast<char *>(malloc_alloc::allocate(bytes_to_get));
            }
            heap_size += bytes_to_get;        // 已占用的堆内存
            end_free = start_free + bytes_to_get;
            return chunk_alloc(size, nobjs);  // 调用自身以修正nobjs
        }
    }

    void *__default_alloc::allocate(size_t n) {
        obj *volatile *my_free_list;
        obj *result;
        //若n大于128, 则采用第一级适配器
        if (n > __MAX_BYTES)
            return (malloc_alloc::allocate(n));
        //选择采用第几区块
        my_free_list = free_list + FREELIST_INDEX(n);
        result = *my_free_list;
        if (result == nullptr) {
            // 未找到可用free_list, 准备填充free_list
            void *r = refill(ROUND_UP(n));
            return r;
        }
        //调整freelist
        *my_free_list = result->free_list_link;
        return (result);
    }

    void __default_alloc::deallocate(void *p, size_t n) {
        // p不可为nullptr
        obj *q = static_cast<obj *>(p);
        obj *volatile *my_free_list;

        if (n > __MAX_BYTES)   // 采用第一级适配器
            malloc_alloc::deallocate(p, n);
        // 寻找对应的free list
        my_free_list = free_list + FREELIST_INDEX(n);
        // 调整free list, 回收区块
        q->free_list_link = *my_free_list;
        *my_free_list = q;
    }

    void *__default_alloc::reallocate(void *p, size_t old_sz, size_t new_sz) {
        deallocate(p, old_sz);
        p = allocate(new_sz);
        return p;
    }

}

#endif //MINISTL_STL_ALLOC_SECOND_IMP_H
