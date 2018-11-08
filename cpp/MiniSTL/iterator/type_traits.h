#ifndef MINISTL_TYPE_TRAITS_H
#define MINISTL_TYPE_TRAITS_H

/* _type_traits: 提供了一种机制, 允许针对不同的类型属性, 在编译时期完成函数派送决定(function dispatch). */

namespace MiniSTL {
    // 没有任何成员的class, 不会带来额外负担, 却又能够标示真假
    struct _true_type {};
    struct _false_type {};

    // trivial 平凡
    template<class T>
    struct _type_traits {
        using has_trivial_default_constructor = _false_type;
        using has_trivial_copy_constructor = _false_type;
        using has_trivial_assignment_operator = _false_type;
        using has_trivial_destructor = _false_type;
        using is_POD_type = _false_type;
    };

    // C++14 style
    template<class T>
    using has_trivial_default_constructor_t = typename _type_traits<T>::has_trivial_default_constructor;

    template<class T>
    using has_trivial_copy_constructor_t = typename _type_traits<T>::has_trivial_copy_constructor;

    template<class T>
    using has_trivial_assignment_operator_t = typename _type_traits<T>::has_trivial_assignment_operator;

    template<class T>
    using has_trivial_destructor_t = typename _type_traits<T>::has_trivial_destructor;

    template<class T>
    using is_POD_type_t = typename _type_traits<T>::is_POD_type;



    /* 对C++基本类型进行特化处理 */
    template<>
    struct _type_traits<bool> {
        using has_trivial_default_constructor = _true_type;
        using has_trivial_copy_constructor = _true_type;
        using has_trivial_assignment_operator = _true_type;
        using has_trivial_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<char> {
        using has_trivial_default_constructor = _true_type;
        using has_trivial_copy_constructor = _true_type;
        using has_trivial_assignment_operator = _true_type;
        using has_trivial_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<unsigned char> {
        using has_trivial_default_constructor = _true_type;
        using has_trivial_copy_constructor = _true_type;
        using has_trivial_assignment_operator = _true_type;
        using has_trivial_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<signed char> {
        using has_trivial_default_constructor = _true_type;
        using has_trivial_copy_constructor = _true_type;
        using has_trivial_assignment_operator = _true_type;
        using has_trivial_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<wchar_t> {
        using has_trivial_default_constructor = _true_type;
        using has_trivial_copy_constructor = _true_type;
        using has_trivial_assignment_operator = _true_type;
        using has_trivial_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<short> {
        using has_trivial_default_constructor = _true_type;
        using has_trivial_copy_constructor = _true_type;
        using has_trivial_assignment_operator = _true_type;
        using has_trivial_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<unsigned short> {
        using has_trivial_default_constructor = _true_type;
        using has_trivial_copy_constructor = _true_type;
        using has_trivial_assignment_operator = _true_type;
        using has_trivial_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<int> {
        using has_trivial_default_constructor = _true_type;
        using has_trivial_copy_constructor = _true_type;
        using has_trivial_assignment_operator = _true_type;
        using has_trivial_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<unsigned int> {
        using has_trivial_default_constructor = _true_type;
        using has_trivial_copy_constructor = _true_type;
        using has_trivial_assignment_operator = _true_type;
        using has_trivial_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<long> {
        using has_trivial_default_constructor = _true_type;
        using has_trivial_copy_constructor = _true_type;
        using has_trivial_assignment_operator = _true_type;
        using has_trivial_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<unsigned long> {
        using has_trivial_default_constructor = _true_type;
        using has_trivial_copy_constructor = _true_type;
        using has_trivial_assignment_operator = _true_type;
        using has_trivial_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<long long> {
        using has_trivial_default_constructor = _true_type;
        using has_trivial_copy_constructor = _true_type;
        using has_trivial_assignment_operator = _true_type;
        using has_trivial_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<unsigned long long> {
        using has_trivial_default_constructor = _true_type;
        using has_trivial_copy_constructor = _true_type;
        using has_trivial_assignment_operator = _true_type;
        using has_trivial_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<float> {
        using has_trivial_default_constructor = _true_type;
        using has_trivial_copy_constructor = _true_type;
        using has_trivial_assignment_operator = _true_type;
        using has_trivial_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<double> {
        using has_trivial_default_constructor = _true_type;
        using has_trivial_copy_constructor = _true_type;
        using has_trivial_assignment_operator = _true_type;
        using has_trivial_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<long double> {
        using has_trivial_default_constructor = _true_type;
        using has_trivial_copy_constructor = _true_type;
        using has_trivial_assignment_operator = _true_type;
        using has_trivial_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<class T>
    struct _type_traits<T *> {
        using has_trivial_default_constructor = _true_type;
        using has_trivial_copy_constructor = _true_type;
        using has_trivial_assignment_operator = _true_type;
        using has_trivial_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<class T>
    struct _type_traits<const T *> {
        using has_trivial_default_constructor = _true_type;
        using has_trivial_copy_constructor = _true_type;
        using has_trivial_assignment_operator = _true_type;
        using has_trivial_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<char *> {
        using has_trivial_default_constructor = _true_type;
        using has_trivial_copy_constructor = _true_type;
        using has_trivial_assignment_operator = _true_type;
        using has_trivial_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<unsigned char *> {
        using has_trivial_default_constructor = _true_type;
        using has_trivial_copy_constructor = _true_type;
        using has_trivial_assignment_operator = _true_type;
        using has_trivial_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<signed char *> {
        using has_trivial_default_constructor = _true_type;
        using has_trivial_copy_constructor = _true_type;
        using has_trivial_assignment_operator = _true_type;
        using has_trivial_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<const char *> {
        using has_trivial_default_constructor = _true_type;
        using has_trivial_copy_constructor = _true_type;
        using has_trivial_assignment_operator = _true_type;
        using has_trivial_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<const unsigned char *> {
        using has_trivial_default_constructor = _true_type;
        using has_trivial_copy_constructor = _true_type;
        using has_trivial_assignment_operator = _true_type;
        using has_trivial_destructor = _true_type;
        using is_POD_type = _true_type;
    };

    template<>
    struct _type_traits<const signed char *> {
        using has_trivial_default_constructor = _true_type;
        using has_trivial_copy_constructor = _true_type;
        using has_trivial_assignment_operator = _true_type;
        using has_trivial_destructor = _true_type;
        using is_POD_type = _true_type;
    };
}

#endif //MINISTL_TYPE_TRAITS_H
