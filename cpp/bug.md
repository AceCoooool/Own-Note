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