#ifndef MINISTL_STL_LIST_H
#define MINISTL_STL_LIST_H

#include "../../allocator/stl_alloc.h"
#include "stl_list_iterator.h"

namespace MiniSTL {
    template<class T, class Alloc = simpleAlloc<T>>
    class list {
    public:
        /* ---------------------------类型别名----------------------- */
        using iterator = __list_iterator<T, T &, T *>;
        using const_iterator = __list_iterator<T, const T &, const T *>;
//        using reverse_iterator = MiniSTL::reverse_iterator<iterator>;
//        using const_reverse_iterator = MiniSTL::reverse_iterator<const_iterator>;
        using value_type = typename __list_iterator<T, T &, T *>::value_type;
        using pointer = typename __list_iterator<T, T &, T *>::pointer;
        using reference = typename __list_iterator<T, T &, T *>::reference;
        using size_type = typename __list_iterator<T, T &, T *>::size_type;
        using difference_type = typename __list_iterator<T, T &, T *>::difference_type;
        using link_type = typename __list_iterator<T, T &, T *>::link_type;

    private:
        /* ---------------------成员变量&辅助函数------------------- */
        // 只要一个指针, 便可表示整个环状双向链表, 空白节点
        link_type node;

        // 两个别名
        using list_node = __list_node<T>;
        using list_node_allocator = simpleAlloc<list_node>;

        // 内存管理函数
        link_type get_node() { return list_node_allocator::allocate(); }
        void put_node(link_type p) { list_node_allocator::deallocate(p); }
        link_type create_node(const T &);
        void destroy_node(link_type);

        // 其他辅助函数
        void empety_initialized();  // 初始化空链表
        void transfer(iterator position, iterator first, iterator last);  // Move [first, last) before pos
        void swap(list &) noexcept;   // 交换链表


    public:
        /* ------------------------ctor && dtor--------------------- */
        list() { empety_initialized(); }
        explicit list(size_type, const value_type &value = value_type());
        list(std::initializer_list<T> il);
        list(const list &);
        list(list &&) noexcept;

        ~list() noexcept;

        /* ---------------------------赋值&友元函数------------------------- */
        list &operator=(const list &) noexcept;
        list &operator=(list &&) noexcept;

        // 几个友元函数
        friend void swap(list<T> &, list<T> &);
        friend bool operator==(const list<T> &lhs, const list<T> &rhs);
        friend bool operator!=(const list<T> &lhs, const list<T> &rhs);

        /* --------------------------getter----------------------- */
        bool empty() const noexcept { return node->next == node; }
        size_type size() const noexcept { return distance(cbegin(), cend()); }
        const_iterator cbegin() const noexcept { return static_cast<link_type>(node->next); }
        const_iterator cend() const noexcept { return node; }
//        const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }
//        const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

        /* --------------------------setter---------------------- */
        iterator begin() noexcept { return static_cast<link_type>(node->next); }
        iterator end() noexcept { return node; }
//        reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
//        reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
        reference front() noexcept { return *begin(); }
        reference back() noexcept { return *(--end()); }

        /* -------------------------other op--------------------- */
        // insert操作
        iterator insert(iterator, const value_type &);
        void insert(iterator, size_type, const value_type &);
        template<class InputIterator>
        void insert(iterator, InputIterator, InputIterator);

        // erase操作
        iterator erase(iterator);
        iterator erase(iterator, iterator);

        // push和pop
        void push_front(const T &value) { insert(begin(), value); }
        void push_back(const T &value) { insert(end(), value); }
        void pop_fornt() { erase(begin()); }
        void pop_back();

        // 其他一些操作
        void unique();                              // 去掉链表中相同的元素
        void splice(iterator, list &);              // 将list接合this的position所指位置之前, list必须不同于*this
        void splice(iterator, list &, iterator);    // 将i接到position之前
        void splice(iterator, list &, iterator, iterator); // 将[first, last)接到position之前
        void merge(list &);                         // 合并两个链表---按升序
        void reverse();                             // 将list“翻转”
        void sort();                                // 排序链表: https://blog.csdn.net/qq276592716/article/details/7932483
        void clear();
        void remove(const T &);                     // 移除所有等于value的结点

    };
}

#endif //MINISTL_STL_LIST_H
