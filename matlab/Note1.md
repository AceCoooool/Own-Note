## matlab c++扩展

下述是一个demo

```cpp
#include "mex.h"
#include "matrix.h"
#include <iostream>

// nlhs: 返回数据的个数   plhs[]: 代表返回数据
// nrhs: 输入数据的个数   prhs[]: 代表输入数据
void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]){
    float *proj = (float *)mxGetPr(prhs[0]); 
    int n = (int)mxGetScalar(prhs[1]);
    for(int i=0; i<n; ++i)
        std::cout<<*(proj+i)<<" ";
    std::cout<<std::endl;
}
```

此时如果在matlab里面调用：

```matlab
p = [1, 2, 3; 2,3,4];

test_dim(single(p), 6);
```

此时输出的顺序为`1 2 2 3 3 4`（即2x3的情况先按照各维依次下去---比如`p(i, j)`对应`p+(j-1)*dim1+i`）

## 常见操作

- `fliplr(A)`: 矩阵左右翻转
- `flipup(A)`: 矩阵上下翻转
- `linspace(a, b, num)`：将`[a, b]`拆分成`num`份
- `strcat(s1, s2)`：连接两个字符串


## 文件相关

- `dir(root)`：root下面所有的子文件
- `fullfile(p1, p2, p3, ...)`：返回`p1/p2/p3`（类似`os.path.join`）



## Image

- `imread(path)`：读取图片
- `imwrite(A, path)`：保存图片
- `rgb2gray(image)`：彩色图片转为灰度图
- ​

