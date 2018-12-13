## 泛型的坑

### 1. 泛型的声明与定义

正常的情况声明在.h文件，定义在.cpp文件中。但是泛型不行！！！（当然有些技巧可以实现）

一般采用的方式是，一个头文件里完成声明；另一个头文件里完成定义。**但是注意：包含头文件时，我们只需包含定义的那个头文件即可**

关于这方面的一些参考：

1. [如何实现的几种方式](https://www.codeproject.com/Articles/48575/How-to-define-a-template-class-in-a-h-file-and-imp)
2. [Storing C++ template function definitions in a .CPP file](https://stackoverflow.com/questions/115703/storing-c-template-function-definitions-in-a-cpp-file)

### 2. 关于using的"别名"存在的bug

下述的方式是会出error的：（至于为什么，还不清楚）

```cpp
template <class T>
class A{
private:
    using t = T;

public:
    t add(T a, T b);
};

// using别名
template<class T>
using tt = typename A<T>::t;

// error !!!!
template<class T>
tt<T> A<T>::add(T a, T b){
    return a+b;
}

// 正确的方式
template<class T>
typename A<T>::t A<T>::add(T a, T b){
    return a+b;
}
```

### 3. 模板匹配"过强"存在的问题

```cpp
class A {
public:
    A(size_t a, int b) {
        cout << "A(size_t, int)" << endl;
    }

    template<class T, class U>
    A(T a, U b) {
        cout << "A(T, U)" << endl;
    }
};

A a(5, 5);         // 输出A(T, U)
A b(size_t(5), 5); // 输出A(size_t, int)
```

可以发现只有"完美匹配"才会优先特化版本，这里比如在`vector<int> a(2, 10)`这种情况在实现上就需要非常注意了

## 析构函数

不管你是采用临时变量，还是move之后的，其销毁都是经过析构函数的。所以这方面要小心。详见`stl_deque`里面的移动操作。



## cstring

1. `void* memcpy( void* dest, const void* src, std::size_t count)`：从 `src` 所指向的对象复制 `count` 个字符到 `dest` 所指向的对象。两个对象都被转译成 `unsigned char` 的数组。
   - 若对象重叠，则行为未定义！！！
   - 若对象不[可平凡复制](https://zh.cppreference.com/w/cpp/named_req/TriviallyCopyable) (TriviallyCopyable) ，则 `memcpy` 的行为未指定而且[可能未定义](http://stackoverflow.com/questions/29777492)

> `std::memcpy` 理应是最快的内存到内存复制子程序。它通常比必须扫描其所复制数据的 [std::strcpy](https://zh.cppreference.com/w/cpp/string/byte/strcpy) ，或必须预防以处理重叠输入的 [std::memmove](https://zh.cppreference.com/w/cpp/string/byte/memmove) 更高效。

2. `void* memmove( void* dest, const void* src, std::size_t count )`：从 `src` 所指向的对象复制 `count` 个字节到 `dest` 所指向的对象。两个对象都被转译成 `unsigned char` 的数组。

> 尽管说明了“如同”使用临时缓冲区，此函数的实际实现不会带来二次复制或额外内存的开销。常用方法（ glibc 和 bsd libc ）是若目标在源之前开始，则从缓冲区开始正向复制，否则从末尾反向复制，完全无重叠时回落到更高效的 [std::memcpy](https://zh.cppreference.com/w/cpp/string/byte/memcpy) 。



## 继承关系

```cpp
class A {
public:
    int a;
    int b;
};

class B : public A {
public:
    int c;
};

// 类型1
B b;
b.a = 1; b.b = 2; b.c = 3;

A a = b;
auto c = static_cast<B &>(a);
cout << c.c << endl;  // 1（不正确的结果）
// auto c = static_cast<B>(a)  // Error, 无法转换

// 类型2
B *bb = new B();
bb->a = 1; bb->b = 2; bb->c = 3;
A *aa = bb;
B *cc = static_cast<B *>(aa);
cout << cc->c << endl;  // 3 正确！！！
```

## 速度方面

### inline

采用`inline`和`inline __attribute__((always_inline))`两者其实速度方面还是有差异的：后者会强制编译期进行inline，即使优化是不可用的。（详见：[what “inline __attribute__((always_inline))” means in the function?](https://stackoverflow.com/questions/22767523/what-inline-attribute-always-inline-means-in-the-function)）

不同编译期其实采用哪一种更快是"不一定的"，详见：[always_inline performance](https://indico.cern.ch/event/386232/sessions/159923/attachments/771039/1057534/always_inline_performance.pdf)

> 注：gcc我们可以采用后者，基本上会更快