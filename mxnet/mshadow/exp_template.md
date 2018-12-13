> 下述内容来自：
>
> 1. 英文版：[官方教程](https://github.com/dmlc/mshadow/tree/master/guide/exp-template)
> 2. 中译版：[Thaurun](http://www.cnblogs.com/heguanyou/p/7545344.html)（下述内容来源于此）

# 表达式模板教程

这里主要解释mshadow的工作原理，更确切的说是介绍mshadow背后的黑魔法：[表达式模板](https://en.wikipedia.org/wiki/Expression_templates)。我们会解释为什么它会影响编译代码的性能，表达式模板也是`C++`矩阵运算库（比如`Eigen, GSL, boost.uBLAS`等）用到的主要技巧。

## 如何写出高效的机器学习代码？ 

在开始之前，我们以梯度更新规则为例来思考下如何写出高效的机器学习代码：假设更新规则如下

```
weight =  - eta * (grad + lambda * weight);
```

> 注：更常见的更新规则为 `weight += - eta * (grad + lambda * weight)`（此处仅仅为了简化）

这里权重与梯度都是长度为`n`的向量。当你选择`C++`作为你的编程语言时，我想你主要考虑的是效率。下面这个原则在大多数`C/C++`程序中很重要：

- 预先分配必要的内存，在程序运行的过程中**没有分配临时内存**

下面是上述规则的一个实现例子：

```cpp
void UpdateWeight(const float *grad, float eta, float lambda,
                  int n, float *weight) {
    for (int i = 0; i < n; ++i) {
        weight[i] = -eta * (grad[i] + lambda * weight[i]);
    }
}
```

这个函数用了预先分配的梯度、权重空间来计算。写这样的一个函数十分简单，然而当我们要重复写很多类似的函数就很头大了（此处指的是比如你有`int, double`等等不同的情况）。所以问题转变为**我们能否写出类似下面的"代码"，并且能够保证和上面的代码具有一样的性能？**

```cpp
void UpdateWeight(const Vec &grad, float eta, float lambda, Vec &weight) {
    weight = -eta * (grad + lambda * weight);
}
```

答案是肯定的，但并没有那么显而易见。

### 一种直观但低效的方式

我们先来看一种最直接的解决方法：运算符重载。

```cpp
struct Vec {
    int len;
    float *dptr;

    Vec(int len) : len(len) {
        dptr = new float[len];
    }
    Vec(const Vec &src) : len(src.len) {
        dptr = new float[len];
        memcpy(dptr, src.dptr, sizeof(float) * len);
    }
    ~Vec(void) {
        delete[] dptr;
    }
};

inline Vec operator+(const Vec &lhs, const Vec &rhs) {   
    Vec res(lhs.len);    // 这里需要分配一次临时内存
    for (int i = 0; i < lhs.len; ++i) {
        res.dptr[i] = lhs.dptr[i] + rhs.dptr[i];
    }
    return res;          // 返回是调用复制构造函数需要再次分配内存, 且执行复制操作
}
```

如果我们用同样的方式重载更多的运算符，我们就可以得到我们的想要的直接写等式而不是循环的方法（即上一节提出的"伪代码"方式）。然而，这种方法明显是低效的，因为中间有临时内存被分配与释放，所以存在更优的方案。

有一种更高效的选择是：我们可以仅重载运算符`+=,-=`，这两个运算符是不用分配临时内存的，但是这会限制我们写等式。

等会我们将会讨论为什么我们需要表达式模板，即使`C++11`提供了移动赋值运算符和右值引用（本教程的末尾提供这两者）。

### 延迟计算

在做运算符`+`时，为什么我们要分配临时内存呢？这是因为我们**不知道**在运算符`+`中分配的目标（即运算结果应该放哪），否则我们可以直接将结果存入到目标中，而不是放在临时变量中。

但是如果我们知道目标呢？下述代码便可以实现这个目的（代码在[exp_lazy.cpp](https://github.com/dmlc/mshadow/blob/master/guide/exp-template/exp_lazy.cpp)中）：

```cpp
// Example Lazy evaluation code
// for simplicity, we use struct and make all members public
#include <cstdio>
struct Vec;
// expression structure holds the expression
// 先将+表达式的输入"存起来---引用的方式"
struct BinaryAddExp {
  const Vec &lhs;
  const Vec &rhs;
  BinaryAddExp(const Vec &lhs, const Vec &rhs)
  : lhs(lhs), rhs(rhs) {}
};
// no constructor and destructor to allocate and de-allocate memory,
// allocation done by user
struct Vec {
  int len;
  float* dptr;
  Vec(void) {}
  Vec(float *dptr, int len)
      : len(len), dptr(dptr) {}
  // here is where evaluation happens
  // 真正计算的地方
  inline Vec &operator=(const BinaryAddExp &src) {
    for (int i = 0; i < len; ++i) {
      dptr[i] = src.lhs.dptr[i] + src.rhs.dptr[i];
    }
    return *this;
  }
};
// "假计算"---延迟到=的时候再"真"计算
inline BinaryAddExp operator+(const Vec &lhs, const Vec &rhs) {
  return BinaryAddExp(lhs, rhs);
}

const int n = 3;
int main(void) {
  float sa[n] = {1, 2, 3};
  float sb[n] = {2, 3, 4};
  float sc[n] = {3, 4, 5};
  Vec A(sa, n), B(sb, n), C(sc, n);
  // run expression
  A = B + C;
  for (int i = 0; i < n; ++i) {
    printf("%d:%f==%f+%f\n", i, A.dptr[i], B.dptr[i], C.dptr[i]);
  }
  return 0;
}
```

我们实现的思想是在运算符`+`中并没有进行直接的计算，而是返回一个表达式的对象（像抽象语法树）。当我们重载运算符`=`时，我们就能够同时知道目标和所有的操作，这样我们就可以直接计算而且不需要临时变量。类似的，我们也将`DotExp`（即乘法运算）也采用延迟计算，在运算符`=`上定义延迟计算，并将矩阵（向量）的乘法"放到"BLAS库上计算。

### 更冗长的表达式与表达式模板

使用延迟计算，我们可以很好地避免临时内存的分配，但是代码的扩展能力被限制了，比如：

- 我们只能写出`A=B+C`，不能写出更长的表达式了
- 当我们加入更多的表达式时，我们需要重载更多的`=`运算符来计算每一个等式

这里我们利用**模板程序的魔力**来解决。下述代码（略长）能够允许你写出更冗长的表达式（[exp_template.cpp](https://github.com/dmlc/mshadow/blob/master/guide/exp-template/exp_template.cpp)）

```cpp
// Example code, expression template, and more length equations
// for simplicity, we use struct and make all members public
#include <cstdio>

// this is expression, all expressions must inheritate it,
// and put their type in subtype
// 能够将表达式"变为指定的类型"
template<typename SubType>
struct Exp {
  // returns const reference of the actual type of this expression
  inline const SubType& self(void) const {
    return *static_cast<const SubType*>(this);
  }
};

// binary add expression
// note how it is inheritates from Exp
// and put its own type into the template argument
// 1. 利用继承, 便可以用"强制类型转换指针"
template<typename TLhs, typename TRhs>
struct BinaryAddExp: public Exp<BinaryAddExp<TLhs, TRhs> > {
  const TLhs &lhs;
  const TRhs &rhs;
  BinaryAddExp(const TLhs& lhs, const TRhs& rhs)
      : lhs(lhs), rhs(rhs) {}
  // 相比lazy而言, 新加的部分---单个位置就无需额外内存空间
  // 此外将Eval定义为各自取的方式
  // evaluation function, evaluate this expression at position i
  inline float Eval(int i) const {
    return lhs.Eval(i) + rhs.Eval(i);
  }
};
// no constructor and destructor to allocate
// and de-allocate memory, allocation done by user
struct Vec: public Exp<Vec> {
  int len;
  float* dptr;
  Vec(void) {}
  Vec(float *dptr, int len)
      :len(len), dptr(dptr) {}
  // here is where evaluation happens
  template<typename EType>
  inline Vec& operator= (const Exp<EType>& src_) {
    const EType &src = src_.self();   // 类型转换过来---这样我们就可以利用模板采用任意的类型
    for (int i = 0; i < len; ++i) {
      dptr[i] = src.Eval(i);
    }
    return *this;
  }
  // evaluation function, evaluate this expression at position i
  inline float Eval(int i) const {
    return dptr[i];
  }
};
// template add, works for any expressions
template<typename TLhs, typename TRhs>
inline BinaryAddExp<TLhs, TRhs>
operator+(const Exp<TLhs> &lhs, const Exp<TRhs> &rhs) {
  return BinaryAddExp<TLhs, TRhs>(lhs.self(), rhs.self());
}

const int n = 3;
int main(void) {
  float sa[n] = {1, 2, 3};
  float sb[n] = {2, 3, 4};
  float sc[n] = {3, 4, 5};
  Vec A(sa, n), B(sb, n), C(sc, n);
  // run expression, this expression is longer:)
  A = B + C + C;
  for (int i = 0; i < n; ++i) {
    printf("%d:%f == %f + %f + %f\n", i,
           A.dptr[i], B.dptr[i],
           C.dptr[i], C.dptr[i]);
  }
  return 0;
}
```

关键的思想是模板`Exp<SubType>`将派生的类作为模板参数，这样就可以将这个模板的自身通过`self()`转换成`SubTpye`（即派生类）。`BinaryAddExp`现在是一个模板类，可以将表达式复合在一起，就像一个复合模式的模板版本一样。计算通过函数`Eval`完成，它在`BinaryAddExp`中以递归的方式完成。

- 由于内联，当函数在执行`=`运算符调用`src.Eval(i)` 时会在编译时被编译成`B.dptr[i] + C.dptr[i] + C.dptr[i]`。
- 我们可以像循环一样高效地将等式写成逐元素的方式。

> 奇怪的是，采用这种方式和naive在时间上面基本没有差别！比lazy慢了一倍（但是改为`inline __attribute__((always_inline))`速度就明显上来了。）

### 使之更灵活

通过上面的例子，我们可以发现模板编程可以使得程序更灵活（在编译阶段）。最后的例子比较接近mshadow了，能够允许用户使用二元运算符（[exp_template_op.cpp](https://github.com/dmlc/mshadow/blob/master/guide/exp-template/exp_template_op.cpp)）

```cpp
// Example code, expression template
// with binary operator definition and extension
// for simplicity, we use struct and make all members public
#include <cstdio>

// this is expression, all expressions must inheritate it,
// and put their type in subtype
template<typename SubType>
struct Exp{
  // returns const reference of the actual type of this expression
  inline const SubType& self(void) const {
    return *static_cast<const SubType*>(this);
  }
};

// binary operators
struct mul{
  inline static float Map(float a, float b) {
    return a * b;
  }
};

// binary add expression
// note how it is inheritates from Exp
// and put its own type into the template argument
template<typename OP, typename TLhs, typename TRhs>
struct BinaryMapExp: public Exp<BinaryMapExp<OP, TLhs, TRhs> >{
  const TLhs& lhs;
  const TRhs& rhs;
  BinaryMapExp(const TLhs& lhs, const TRhs& rhs)
      :lhs(lhs), rhs(rhs) {}
  // evaluation function, evaluate this expression at position i
  inline float Eval(int i) const {
    return OP::Map(lhs.Eval(i), rhs.Eval(i));   // 核心在于这部分改了下
  }
};
// no constructor and destructor to allocate and de-allocate memory
// allocation done by user
struct Vec: public Exp<Vec>{
  int len;
  float* dptr;
  Vec(void) {}
  Vec(float *dptr, int len)
      : len(len), dptr(dptr) {}
  // here is where evaluation happens
  template<typename EType>
  inline Vec& operator=(const Exp<EType>& src_) {
    const EType &src = src_.self();
    for (int i = 0; i < len; ++i) {
      dptr[i] = src.Eval(i);
    }
    return *this;
  }
  // evaluation function, evaluate this expression at position i
  inline float Eval(int i) const {
    return dptr[i];
  }
};
// template binary operation, works for any expressions
template<typename OP, typename TLhs, typename TRhs>
inline BinaryMapExp<OP, TLhs, TRhs>
F(const Exp<TLhs>& lhs, const Exp<TRhs>& rhs) {
  return BinaryMapExp<OP, TLhs, TRhs>(lhs.self(), rhs.self());
}

template<typename TLhs, typename TRhs>
inline BinaryMapExp<mul, TLhs, TRhs>
operator*(const Exp<TLhs>& lhs, const Exp<TRhs>& rhs) {
  return F<mul>(lhs, rhs);
}

// user defined operation
struct maximum{
  inline static float Map(float a, float b) {
    return a > b ? a : b;
  }
};

const int n = 3;
int main(void) {
  float sa[n] = {1, 2, 3};
  float sb[n] = {2, 3, 4};
  float sc[n] = {3, 4, 5};
  Vec A(sa, n), B(sb, n), C(sc, n);
  // run expression, this expression is longer:)
  A = B * F<maximum>(C, B);
  for (int i = 0; i < n; ++i) {
    printf("%d:%f == %f * max(%f, %f)\n",
           i, A.dptr[i], B.dptr[i], C.dptr[i], B.dptr[i]);
  }
  return 0;
}
```

### 总结

到这里为止，你应该已经明白它背后的基本思想：

- 延迟计算，使得我们能知道所有的操作数和目标。
- 复合模板和递归计算，使得我们能够计算逐元素操作的任意复合表达式。
- 由于模板和内联的设计，我们写出来的表达式像用循环实现更新规则的一样高效。

所以在编写机器学习代码时其实是写表达式，并将精力集中在重要的算法上。

## 在MShadow中的表达式模板

Mshadow的表达式模板用到上面我们介绍的关键思想，但有几个小的不同点：

- 我们将评估代码与表达式构建和组成代码分开：
  - 在表达中创建`Plan`类用来替代`Exp`类的计算函数`Eval`，用来计算结果。
  - 这允许我们在`Plan`中放置较少的变量，例如，当我们评估数据时，我们不需要数组长度。
  - 一个重要的原因是CUDA内核不能使用const引用来接受类。
  - 虽然这种设计选择是有争议的，但我们发现迄今为止还是有用的。
- 延迟还支持复式的表达式，比如矩阵的乘法
  - 除了逐元素的表达式，我们还支持比这样`A = dot(B.T(), C)`的运算，同样延迟表达是不需要分配临时内存的。
- 类型检查和数组长度检查。

## 备注

表达式模板与`C++11`：在`C++11`中，移动构造函数可以用来保存重复的分配内存，这样就**省去了一些需要的表达模板。然后，仍然要分配最少一次的空间**。

- 这只是删除了表达式模板中表达式所需的内存，比如`dst = A+B+C`，`dst`并没有包括赋值前所分配的空间。
- 如果我们想保留所有的变量预先分配内存的语法，并且表达式执行时没有内存分配（这是我们在mshadow中所做的），我们仍然需要表达式模板。

