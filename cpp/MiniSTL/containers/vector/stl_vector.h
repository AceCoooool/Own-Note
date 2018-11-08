#ifndef MINISTL_STL_VECTOR_H
#define MINISTL_STL_VECTOR_H

#pragma once
#include "../../allocator/stl_alloc.h"
#include "../../allocator/stl_uninitialized.h"

namespace MiniSTL {
    template<class T, class Alloc = simpleAlloc<T> >
    class vector {
    public:
        /* ----------vector的嵌套类型定义(为了stl_iterator萃取)----------- */
        using value_type = T;
        using pointer = value_type *;
        using iterator = value_type *;  // vector中采用的迭代器iterator是raw pointer
        using const_iterator = value_type *;
//        using reverse_iterator = reverse_iterator<iterator>;    //TODO: add after chapter7
//        using const_reverse_iterator = reverse_iterator<const_iterator>;
        using reference = value_type &;
        using const_reference = const value_type &;
        using size_type = size_t;
        using difference_type = ptrdiff_t;

    private:
        /* ---------------------成员变量------------------- */
        iterator start;           // 表示目前使用空间的头
        iterator finish;          // 表示目前使用空间的尾
        iterator end_of_storage;  // 表示目前可用空间的尾

    public:
        /* ---------------------ctor && dtor------------------ */
        vector() : start(nullptr), finish(nullptr), end_of_storage(nullptr) {}
        explicit vector(size_type n) { fill_initialize(n, value_type()); }
        vector(size_type n, const value_type &value) { fill_initialize(n, value); }
        template<class InputIterator>
        vector(InputIterator first, InputIterator last) { allocate_and_copy(first, last); }
        vector(std::initializer_list<value_type>);
        vector(const vector &rhs) { allocate_and_copy(rhs.start, rhs.finish); }
        vector(vector &&) noexcept;

        ~vector() noexcept;

        /* ---------------------赋值&比较------------------ */
        vector &operator=(const vector &);
        vector &operator=(std::initializer_list<value_type>);
        vector &operator=(vector &&) noexcept;
        bool operator==(const vector &) const noexcept;
        bool operator!=(const vector &rhs) const noexcept { return !(*this == rhs); }

        // 两个比较友元函数
        friend bool operator==(const vector<T, Alloc> &, const vector<T, Alloc> &);
        friend bool operator!=(const vector<T, Alloc> &, const vector<T, Alloc> &);

        /* ---------------------getter------------------ */
        const_iterator cbegin() const noexcept { return start; }
        const_iterator cend() const noexcept { return finish; }
//        const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(finish); }
//        const_reverse_iterator crend() const noexcept { return const_reverse_iterator(start); }
        const_reference front() const noexcept { return *start; }
        const_reference back() const noexcept { return *(finish - 1); }
        const_reference operator[](const size_type n) const noexcept { return *(start + n); } // whether noexcept
        size_type size() const noexcept { return static_cast<size_type>(finish - start); }
        size_type capacity() const noexcept { return static_cast<size_type>(end_of_storage - start); }
        bool empty() const noexcept { return start == finish; }

        /* ---------------------setter------------------ */
        iterator begin() noexcept { return start; }
        iterator end() noexcept { return finish; }
//        reverse_iterator rbegin() noexcept { return reverse_iterator(finish); }
//        reverse_iterator rend() noexcept { return reverse_iterator(start); }
        reference operator[](const size_type n) noexcept { return *(start + n); }
        reference front() noexcept { return *start; }
        reference back() noexcept { return *(finish - 1); }

        /* ---------------------other op------------------ */
        void push_back(const value_type &);
        void pop_back();

        iterator erase(iterator, iterator);
        iterator erase(iterator position) { return erase(position, position + 1); }
        void clear() { erase(begin(), end()); }
        void insert(iterator, size_type, const value_type &);
        iterator insert(iterator, const value_type &);

        void resize(size_type, const value_type &);
        void resize(size_type new_size) { resize(new_size, value_type()); }
        void reserve(size_type);  // 扩充capacity
        void shrink_to_fit() noexcept;  // 强制减少capacity到size大小

    private:
        using data_allocator = Alloc;
        /* ---------------------辅助函数------------------- */
        void swap(vector &) noexcept;  // 交换操作

        void insert_aux(iterator position, const value_type &value);  // insert的辅助函数

        // 分配内存+初始化(构造函数)
        iterator allocate_and_fill(size_type n, const value_type &value) {
            iterator result = data_allocator::allocate(n);
            uninitialized_fill_n(result, n, value);
            return result;
        }

        void fill_initialize(size_type n, const value_type &value) {
            start = allocate_and_fill(n, value);
            finish = start + n;
            end_of_storage = finish;
        }

        // 分配内存+复制区间
        template<class InputIterator>
        iterator allocate_and_copy(InputIterator first, InputIterator last) {
            start = data_allocator::allocate(last - first);
            finish = uninitialized_copy(first, last, start);
            end_of_storage = finish;
            return start;
        }

        // 回收内存
        void deallocate() noexcept {
            if (start) data_allocator::deallocate(start, end_of_storage - start);
        }

        // 析构+回收内存
        void destroy_and_deallocate() noexcept {
            destroy(start, finish);
            deallocate();
        }
    };
}

#endif //MINISTL_STL_VECTOR_H
