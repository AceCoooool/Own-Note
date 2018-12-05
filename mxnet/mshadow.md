# mshadow

> 由于在看torch的时候，如何改写成更高效的c++有所困惑。所以先看下这个库。

### 1. 安装

step1：`git clone https://github.com/dmlc/mshadow.git`

step2：在你自己写的测试代码的CMakeLists.txt加入下面这段`include_directories("your_path/mshadow")`

step3：如果这样编译会存在错误（找不到`#include<mkl_blas.h>`等），为了方便可以直接令这个`base.h`文件的下述两个宏改为0：

```cpp
#ifndef MSHADOW_USE_MKL
  #define MSHADOW_USE_MKL   0   // 原本1
#endif

#ifndef MSHADOW_USE_CUDA
  #define MSHADOW_USE_CUDA   0   // 原本1
#endif
```

