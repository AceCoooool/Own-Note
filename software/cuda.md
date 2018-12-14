## 1. 显卡驱动

安装显卡驱动：**系统设置→软件和更新→附加驱动**，选择**NVIDIA驱动**就好了。

> 可以更新附加驱动版本（因为有些CUDA需要更高版本的驱动，具体对应关系请看[CUDA Toolkit](https://docs.nvidia.com/cuda/cuda-toolkit-release-notes/index.html)）：
>
> `sudo add-apt-repository ppa:graphics-drivers/ppa && sudo apt update`

下载安装CUDA和cudnn：

- 在官网[CUDA下载界面](https://developer.nvidia.com/cuda-downloads)，选择Linux，x86_64，Ubuntu，16.04，runfile再下载下来。后面在含有该文件的目录下运行`sudo sh cuda\_8.0.61_375.26\_linux.run`（名字改为你下载下来的版本）之后一直空格直到出现让你选择是否接受条款开始，之后的除了是否安装NVIDIA驱动选择“否”之外，其他的都选择“是”。具体可以参照[Ubuntu+CUDA](http://lib.csdn.net/article/deeplearning/55144)这篇文章中的选择。执行完上述过程后，要修改环境变量，采用下面的命令, (后来遇到了点小问题，参考了下[nvida cuda guide](http://docs.nvidia.com/cuda/cuda-installation-guide-linux/index.html#axzz4f2Gn9xWL))

  ```shell
  # cuda-8.0可能需要改为你对应的版本 --- 比如cuda-9.2
  export PATH=/usr/local/cuda-8.0/bin${PATH:+:${PATH}}
  export LD_LIBRARY_PATH=/usr/local/cuda-8.0/lib64\
                           ${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}
  export LD_LIBRARY_PATH=/usr/local/cuda-8.0/lib\
                           ${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}
  # echo 'export LD_LIBRARY_PATH=/usr/local/cuda-8.0/lib64:$LD_LIBRARY_PATH' >> ~/.bashrc
  ```

- 安装cudnn，首先从[cudnn下载](https://developer.nvidia.com/rdp/cudnn-download)（提示：这里需要注册账号，但其实注册很方便的）。下载`cuDNNv5.1 Library for linux`（这个也可能采用更新版本的cudnn），然后解压，根据下述命令

  ```shell
  # cudnn-xxx改为你自己下载的版本，以及cuda-8.0改为你下载的版本
  tar -zxvf cudnn-8.0-linux-x64-v5.1.solitairetheme8
  cd cuda
  sudo cp lib64/* /usr/local/cuda-8.0/lib64/
  sudo cp include/cudnn.h /usr/local/cuda-8.0/include/
  ```

- 检验是否安装成功，可参照[cuda+cudnn](http://shomy.top/2016/12/29/gpu-tensorflow-install/)这篇博客，具体步骤如下：
  ① 进入`/usr/local/cuda/samples`，编译运行某个sample:

  ```shell
  sudo make all -j4
  ```

  ② 全部编译完成后，进入 `samples/bin/x86_64/linux/release`，运行`deviceQuery`：

  ```shell
  ./deviceQuery
  ```

  显示有可用的GPU，即可成功。

## 2. 多个CUDA版本切换

按照上面的方式，再安装新的CUDA版本（同样的步骤 --- 那个建立软链接 `Do you want to install a symbolic link at /usr/local/cuda`可以选择no，其实选yes也没关系）

更改`~/.bashrc`中的环境变量：

```shell
export PATH=\$PATH:/usr/local/cuda/bin
export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:/usr/local/cuda/lib64  
export LIBRARY_PATH=$LIBRARY_PATH:/usr/local/cuda/lib64 
```

切换方式

```shell
cd
cd /usr/local
sudo rm -rf cuda   # 删除原有软链接
sudo ln -s /usr/local/cuda-9.0/ /usr/local/cuda   # 建立9.0的软链接

sudo rm -rf cuda  
sudo ln -s /usr/local/cuda-8.0/ /usr/local/cuda   # 切换到8.0的软链接
```

