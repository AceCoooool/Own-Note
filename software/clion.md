# CLion使用教程

### 1. 安装

1. 在[CLion官网](https://www.jetbrains.com/clion/)官网下载（写这个教程的时候用的是2018.02版本）

2. 运行IDE：

   ```shell
   cd /your_path_of_pycharm/bin
   sudo ./pycharm.sh
   ```

   > 可以改为更方便的方式：
   >
   > ```shell
   > sudo ln -s /your_path_to_clion/bin/clion.sh /usr/local/bin/clion
   > ```
   >
   > 之后你只需在命令行输入：
   >
   > ```shell
   > clion
   > ```
   >
   > 即可

### 2. 破解

> 有钱请支持正版，教程来自：[成都没有派对](https://www.imsxm.com/2018/07/idea-2018-1-5-crack-patcher.html)

1. 下载补丁：[JetbrainsCrack-3.1](https://www.imsxm.com/wp-content/uploads/2018/07/JetbrainsCrack-3.1.zip)，并解压（可以放到Clion文件夹下面）
2. 打开CLion（可以使用试用30天版本）后点击`Help->Edit Custom VM Options ..`会生成一个文件且会自动打开。在这个文件最后添加依一句`-javaagent:your_path_to_crack/JetbrainsCrack.jar`
3. 重启CLion就完成了

### 3. CLion支持cuda程序运行

> 安装插件：[clion-cuda-run-patcher](https://github.com/anantapalani/clion-cuda-run-patcher)，感谢anantapalani写了这个插件

1. 选择`File->Settings->Plugins`
2. 在搜索栏输入`CUDA`，就可以搜到`CLion CUDA Run Patcher`这个插件，选择`Install`或者`Update`
3. 弹出警告，同意即可
4. 安装完成后，选择`Restart CLion`
5. 点击`OK`并关闭`Settings`
6. 重启CLion即完成了

下面提供一个测试代码：

① `CMakeLists`文件里面：

```cmake
cmake_minimum_required(VERSION 3.8)
project(temp)

set(CMAKE_CXX_STANDARD 11)

find_package(CUDA REQUIRED)
enable_language(CUDA)   # 这句是关键, 当然教程里面还有另一种方式

add_executable(temp tmp.cu)
set_property(TARGET temp PROPERTY CUDA_STANDARD 11)  # 如果你准备采用c++11, 则请加上这句话
```

② `tmp.cu`

```cpp
// 数组每个元素进行平方操作程序
#include <stdio.h>

/* --------------------内核程序----------------------*/
__global__ void square(float *d_out, float *d_in) {
    int idx = threadIdx.x;  // threadIdx代表线程的索引
    float f = d_in[idx];
    d_out[idx] = f * f;
}

/* -------------------主程序------------------------*/
int main(int argc, char **argv) {
    const int ARRAY_SIZE = 64;
    const int ARRAY_BYTES = ARRAY_SIZE * sizeof(float);
    // 在主机(host---CPU的内存)上存储数据
    float h_in[ARRAY_SIZE];
    for (int i = 0; i < ARRAY_SIZE; i++) {
        h_in[i] = float(i);
    }
    float h_out[ARRAY_SIZE];

    // 定义指向GPU内存的指针
    float *d_in;
    float *d_out;
    // 1. 分配GPU内存
    cudaMalloc((void **) &d_in, ARRAY_BYTES);
    cudaMalloc((void **) &d_out, ARRAY_BYTES);
    // 2. 将数据从CPU内存复制到GPU内存上
    cudaMemcpy(d_in, h_in, ARRAY_BYTES, cudaMemcpyHostToDevice);
    // 3. 启动内核
    square << < 1, ARRAY_SIZE >> > (d_out, d_in);
    // 4. 将数据从GPU内存复制回CPU内存上
    cudaMemcpy(h_out, d_out, ARRAY_BYTES, cudaMemcpyDeviceToHost);

    // 显示结果
    for (int i = 0; i < ARRAY_SIZE; i++) {
        printf("%f", h_out[i]);
        printf(((i % 4) != 3) ? "\t" : "\n");
    }
    // 释放GPU内存
    cudaFree(d_in);
    cudaFree(d_out);
    return 0;
}
```

直接按`Ctrl+F10`即可运行。

> 关于如何利用CLion对cuda程序进行debug，可以参考：[CLion CUDA](http://www.taolizhicheng.com/%E6%95%99%E7%A8%8B/2018/06/20/clion-with-cuda/)（这部分个人没尝试，等找到更便捷的方式会在分享）

