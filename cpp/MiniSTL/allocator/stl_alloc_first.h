#ifndef MINISTL_STL_ALLOC_FIRST_H
#define MINISTL_STL_ALLOC_FIRST_H

#include <cstdlib>  // malloc, free, size_t
#include <new>      // bad_alloc


namespace MiniSTL {
    /* ----- first allocator ------ */
    class __malloc_alloc {
    public:
        // alias declaration
        using malloc_handler = void (*)();
    private:
        // 以下函数指针用以处理内存不足的情况: oom=out-of-memory
        static void *oom_malloc(size_t);
        static void *oom_realloc(void *, size_t);
        static malloc_handler __malloc_alloc_oom_handler;

    public:
        static void *allocate(size_t n) {
            void *result = malloc(n);
            if (result == nullptr)
                result = oom_malloc(n);
            return result;
        }

        static void deallocate(void *p, size_t /*n*/) {
            free(p);
        }

        static void *reallocate(void *p, size_t /*old_sz*/, size_t new_sz) {
            void *result = realloc(p, new_sz);
            if (result == nullptr)
                oom_realloc(p, new_sz);
            return result;
        }

        // 模拟set_new_handler: 原因在于并未使用new与delete配置内存, 因此无法使用set_new_handler
        static malloc_handler set_malloc_handler(malloc_handler f) {
            malloc_handler old = __malloc_alloc_oom_handler;
            __malloc_alloc_oom_handler = f;
            return old;
        }
    };

    // 定义handler
    typename __malloc_alloc::malloc_handler __malloc_alloc::__malloc_alloc_oom_handler = nullptr;

    void *__malloc_alloc::oom_malloc(size_t n) {
        malloc_handler new_alloc_handler;
        void *result;
        for (;;) {   // 不断尝试释放, 配置
            new_alloc_handler = __malloc_alloc_oom_handler;
            if (!new_alloc_handler)
                throw std::bad_alloc();
            (*new_alloc_handler)();   // 调用handler, 试图释放内存
            result = malloc(n);
            if (result)
                return result;
        }
    }

    void *__malloc_alloc::oom_realloc(void *p, size_t n) {
        malloc_handler new_alloc_handler;
        void *result;
        for (;;) {
            new_alloc_handler = __malloc_alloc_oom_handler;
            if (!new_alloc_handler)
                throw std::bad_alloc();
            (*new_alloc_handler)();
            result = realloc(p, n);
            if (result)
                return result;
        }
    }

    using malloc_alloc = __malloc_alloc;
}

#endif //MINISTL_STL_ALLOC_FIRST_H
