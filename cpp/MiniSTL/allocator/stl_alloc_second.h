#ifndef MINISTL_STL_ALLOC_SECOND_H
#define MINISTL_STL_ALLOC_SECOND_H

#include "stl_alloc_first.h"

namespace MiniSTL {
    // freelist参数设定: 区块上调边界 区块上限 freelist个数
    enum __freelist_setting {
        __ALIGN = 8, __MAX_BYTES = 128, __NFREELISTS = __MAX_BYTES / __ALIGN
    };

    class __default_alloc {
    private: // member variable
        // free_list节点 (由于union特性, 并不需要占用额外的内存)
        union obj {
            union obj *free_list_link;  // 指向下一节点
            char client_data[1];        // 指向资源
        };
        // chunk allocation state
        static char *start_free;        // 内存池起始位置, 只在chunk_alloc()中变化
        static char *end_free;          // 内存池结束位置, 只在chunk_alloc()中变化
        static size_t heap_size;

        static obj *volatile free_list[__NFREELISTS];

    private: // aux function
        // 将bytes上调至8的倍数 (tips:将x扩大至y的整数倍---取 (x+y-1)&~(y-1))
        static size_t ROUND_UP(size_t bytes) {
            return (((bytes) + __ALIGN - 1) & ~(__ALIGN - 1));
        }
        // 决定使用第几号节点
        static size_t FREELIST_INDEX(size_t bytes) {
            return (bytes + __ALIGN - 1) / __ALIGN - 1;
        }

        static void *refill(size_t n);
        static char *chunk_alloc(size_t size, int &nobjs);

    public:
        static void *allocate(size_t n);
        static void deallocate(void *p, size_t n);
        static void *reallocate(void *p, size_t old_sz, size_t new_sz);
    };

    // init static member
    char *__default_alloc::start_free = nullptr;
    char *__default_alloc::end_free = nullptr;
    size_t __default_alloc::heap_size = 0;
    __default_alloc::obj *volatile __default_alloc::free_list[__NFREELISTS] = {
            nullptr, nullptr, nullptr, nullptr,
            nullptr, nullptr, nullptr, nullptr,
            nullptr, nullptr, nullptr, nullptr,
            nullptr, nullptr, nullptr, nullptr
    };

}

#endif //MINISTL_STL_ALLOC_SECOND_H
