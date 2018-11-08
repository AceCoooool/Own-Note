#ifndef MINISTL_STL_VECTOR_IMP_H
#define MINISTL_STL_VECTOR_IMP_H

#include "stl_vector.h"

namespace MiniSTL {
    /* ---------------------ctor && dtor------------------ */
    template<class T, class Alloc>
    inline vector<T, Alloc>::vector(std::initializer_list<T> il) {
        start = allocate_and_copy(il.begin(), il.end());
        finish = end_of_storage = start + (il.end() - il.begin());
    }

    template<class T, class Alloc>
    inline vector<T, Alloc>::vector(vector &&rhs) noexcept {
        start = rhs.start;
        finish = rhs.finish;
        end_of_storage = rhs.end_of_storage;
        rhs.start = rhs.finish = rhs.end_of_storage = nullptr;
    }

    template<class T, class Alloc>
    inline vector<T, Alloc>::~vector() noexcept {
        destroy(start, finish);
        deallocate();
    }

    /* ---------------------赋值&比较------------------ */
    template<class T, class Alloc>
    inline vector<T, Alloc> &vector<T, Alloc>::operator=(const vector &rhs) {
        vector temp(rhs);
        swap(temp);
        return *this;
    }

    template<class T, class Alloc>
    inline vector<T, Alloc> &vector<T, Alloc>::operator=(std::initializer_list<value_type> il) {
        destroy_and_deallocate();
        start = allocate_and_copy(il.begin(), il.end());
        finish = end_of_storage = start + (il.end() - il.begin());
    }

    template<class T, class Alloc>
    inline vector<T, Alloc> &vector<T, Alloc>::operator=(vector &&rhs) noexcept {
        if (this != &rhs) {
            destroy_and_deallocate();
            start = rhs.start;
            finish = rhs.finish;
            end_of_storage = rhs.end_of_storage;
            rhs.start = rhs.finish = rhs.end_of_storage = nullptr;
        }
        return *this;
    }

    template<class T, class Alloc>
    inline bool vector<T, Alloc>::operator==(const vector &v) const noexcept {
        if (size() != v.size()) {
            return false;
        } else {
            auto ptr1 = start;
            auto ptr2 = v.start;
            for (; ptr1 != finish && ptr2 != v.finish; ++ptr1, ++ptr2) {
                if (*ptr1 != *ptr2)
                    return false;
            }
            return true;
        }
    }

    template<class T, class Alloc>
    inline bool operator==(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
        return lhs.operator==(rhs);
    }

    template<class T, class Alloc>
    inline bool operator!=(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
        return !(lhs == rhs);
    }

    /* ---------------------getter------------------ */
    // None

    /* ---------------------setter------------------ */
    // None

    /* ---------------------other op------------------ */
    template<class T, class Alloc>
    inline void vector<T, Alloc>::push_back(const value_type &value) {
        if (finish != end_of_storage) {
            construct(finish, value);
            ++finish;
        } else
            insert_aux(end(), value);
    }

    template<class T, class Alloc>
    inline void vector<T, Alloc>::pop_back() {
        --finish;
        destroy(finish);
    }

    template<class T, class Alloc>
    inline typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator first, iterator last) {
        iterator i = std::copy(last, finish, first);   // TODO
        destroy(i, finish);
        finish -= (last - first);
        return first;
    }

    template<class T, class Alloc>
    void vector<T, Alloc>::insert(iterator position, size_type n, const value_type &value) {
        if (n) {
            if (static_cast<size_type>(end_of_storage - finish) >= n) {// needn't expand
                value_type value_copy = value;
                const size_type elems_after = finish - position;
                iterator old_finish = finish;
                if (elems_after > n) {
                    uninitialized_copy(finish - n, finish, finish);
                    finish += n;
                    // copy_backward needs _SCL_SECURE_NO_WARNINGS
                    std::copy_backward(position, old_finish - n, old_finish); // TODO
                    std::fill(position, position + n, value_copy);  // TODO
                } else {
                    uninitialized_fill_n(finish, n - elems_after, value_copy);
                    finish += n - elems_after;
                    uninitialized_copy(position, old_finish, finish);
                    finish += elems_after;
                    std::fill(position, old_finish, value_copy); // TODO
                }
            } else { // expand
                const size_type old_size = size();
                const size_type new_size = old_size + (old_size > n ? old_size : n);
                iterator new_start = data_allocator::allocate(new_size);
                iterator new_finish = new_start;
                try {
                    new_finish = uninitialized_copy(start, position, new_start);
                    new_finish = uninitialized_fill_n(new_finish, n, value);
                    new_finish = uninitialized_copy(position, finish, new_finish);
                }
                catch (std::exception &) {
                    destroy(new_start, new_finish);
                    data_allocator::deallocate(new_start, new_size);
                    throw;
                }
                destroy_and_deallocate();
                start = new_start;
                finish = new_finish;
                end_of_storage = new_start + new_size;
            }
        }
    }

    template<class T, class Alloc>
    inline typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(iterator position, const value_type &value) {
        difference_type diff = position - begin();
        insert(position, 1, value);
        return begin() + diff;
    }

    template<class T, class Alloc>
    inline void vector<T, Alloc>::resize(size_type new_size, const value_type &value) {
        if (new_size < size())
            erase(begin() + new_size, end());
        else
            insert(end(), new_size - size(), value);
    }

    template<class T, class Alloc>
    inline void vector<T, Alloc>::reserve(size_type n) {
        if (n <= capacity())
            return;
        T *newStart = data_allocator::allocate(n);
        T *newFinish = uninitialized_copy(begin(), end(), newStart);
        destroy_and_deallocate();

        start = newStart;
        finish = newFinish;
        end_of_storage = start + n;
    }

    template<class T, class Alloc>
    inline void vector<T, Alloc>::shrink_to_fit() noexcept {
        vector temp(*this);
        swap(temp);
    }

    /* ---------------------辅助函数------------------- */
    template<class T, class Alloc>
    inline void vector<T, Alloc>::swap(vector &rhs) noexcept {
        using std::swap;
        swap(start, rhs.start);
        swap(finish, rhs.finish);
        swap(end_of_storage, rhs.end_of_storage);
    }

    template<class T, class Alloc>
    void vector<T, Alloc>::insert_aux(iterator position, const value_type &value) {
        if (finish != end_of_storage) {         // 还有备用空间
            construct(finish, *(finish - 1));
            ++finish;
            value_type value_copy = value;
            std::copy_backward(position, finish - 2, finish - 1);   // TODO
            *position = value_copy;
        } else {      // 无备用空间
            const size_type old_size = size();
            const size_type new_size = old_size ? 2 * old_size : 1; // new_cap=2*old_cap
            iterator new_start = data_allocator::allocate(new_size);
            iterator new_finish = new_start;
            try {
                new_finish = uninitialized_copy(start, position, new_start);   // 复制前半段
                construct(new_finish, value);
                ++new_finish;
                new_finish = uninitialized_copy(position, finish, new_finish); // 复制后半段
            }
            catch (std::exception &) {
                //commit or rollback
                destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, new_size);
                throw;
            }
            destroy_and_deallocate();  // 释放原本的空间
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + new_size;
        }
    }

}

#endif //MINISTL_STL_VECTOR_IMP_H
