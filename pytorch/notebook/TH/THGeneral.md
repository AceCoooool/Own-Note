# THGeneral

主要包含下述几个主要内容：

- 实现一些常用的函数
- 内存管理以及一些内存管理规则
- 通知垃圾回收机制(GC)在什么情况下起作用

## 1. 常用的函数

### 1.1 错误处理函数

1. `void _THError(const char *file, const int line, const char *fmt, ...)`
   别名：`THError(...)`；输出错误信息，包含文件和行号
2. `void _THArgCheck(const char *file, int line, int condition, int argNumber, const char *fmt, ...)`
   别名(其实不算别名，是上述函数的一个循环): `THArgCheck(...)` 参数错误情况(输出参数错误信息)，`condition=1`时代表无错，即参数无误
3. `void _THAssertionFailed(const char *file, const int line, const char *exp, const char *fmt, ...)`
   别名：`THAssert(exp)` 输出断言错误信息
4. 其他一些错误处理函数

### 1.2 一些其他函数

1. `double THLog1p(const double x)`
   log1p操作

2. `TH_API THDescBuff _THSizeDesc(const long *size, const long ndim)`
   返回维度信息: ndim维，且每一维的长度存储在`size`指针对应的空间（如[10x20x30]这种形式）

3. ```c
   #define TH_CONCAT_2(x,y) TH_CONCAT_2_EXPAND(x,y)
   #define TH_CONCAT_2_EXPAND(x,y) x ## y
   ```

   在定义函数时命名规则采用的宏操作

## 2. 垃圾回收机制

垃圾回收机制是由高级语言(如`python`,`Lua`等含有的)，C中相当于告诉这些垃圾回收机制几时调用垃圾回收机制，调用垃圾回收机制的两种情况：

1. 当分配内存失败时（如采用`malloc`或`realloc`等失败）
2. 当堆中内存达到动态调整的“阈值”时

这部分函数具体不介绍，主要包含以下一些内容：

- 通过heapSize来记录当前所有使用的堆的大小：分配内存，释放内存等都会影响该值
- 当heapSize超过heapSoftmax时，调用垃圾回收机制，如果清理后，依旧能够达到heapSoftmax的80%，则将heapSoftmax增大到原来的140%

## 3. 内存分配&释放

1. `void* THAlloc(ptrdiff_t size)`
   分配一块大小为size的内存空间：返回指向该空间的指针
2. `void* THRealloc(void *ptr, ptrdiff_t size)`
   将ptr指向的空间的大小调整为size
3. `void THFree(void *ptr)`
   释放ptr指向的空间

注：上述的函数均对heapSize进行了改变，且会判断是否调用垃圾回收机制（通过`THHeapUpdate`和`THAllocInternal`作为桥梁进行）