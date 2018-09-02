# Ubuntu系统安装教程

## 1. 系统安装

> 采用的是U盘安装，与win10共存的双系统方案。
>
> 1. 详细内容可以参考[ubuntu安装](https://jingyan.baidu.com/article/3c48dd348bc005e10be358eb.html)（万一链接失效，你就自己再百度吧）
> 2. 推荐一个磁盘分区管理等的工具：[diskGenius](http://www.diskgenius.cn/)，能够解决大部分的分区问题

### ① 安装步骤

1. 下载Ubuntu16.04：[ubuntu16.04](http://mirror.pnl.gov/releases/xenial/)（你也可以下载更新的[Ubuntu18.04](https://www.ubuntu.com/download/desktop)）
2. 制作启动盘：
   - 下载并安装UltraISO软件
   - 打开软件，点击`文件(F)-->打开`，选择1下载的Ubuntu对应的ISO文件
   - 完成加载ISO文件后，点击`启动(B)-->写入硬盘映像`
   - 选择你的U盘（至少4G大小），然后点击写入即可（一般采用默认选项即可）
3. 安装Ubuntu16.04：（选择其他方式安装，没什么其他需要注意的只是给出个人采用的划分方式）
   - `/`(主分区，Ext4日志文件系统，60G，如果有**固态硬盘**可以分到上面) 
   - `swap`(逻辑分区，16G, 这块建议不要装在**固态硬盘**) 
   - `/boot`(逻辑分区，Ext4日志文件系统，500M) --- 说实话个人觉得可以在开辟大一点，不然更新内核后时不时要删点东西
   - `/home`(逻辑分区，Ext4日志文件系统，200G---剩多少给多少)

### ② 一些可能的"意外情况"

- 一个在安装系统中经常遇到的问题：剩余空间无法使用问题，大部分看到的解决方法是将“/磁盘分区”设置为主分区，其他磁盘分区设置为逻辑分区。但个人安装时此方式并不可行。采用的解决方法：当剩余空间无法使用时，将前面已经分配的磁盘分区删除掉（对应界面上的“-”号），再弄后续的，之后再回去将前面的磁盘再建立（简单点说就是隔位删除，这样就会有多个空闲的空间）。 —— 往往是在单块磁盘装双系统中可能存在的问题
- 如果你是是单块GPU而没有HD等，可能在安装时，无法进入安装界面。解决方法：在选择`install ubuntu`和`try ubuntu without installation`界面时，按`e`进入`grub`界面，将`quite splash ---`改为`quite splash nomodeset`，再按`F10`保存重新再次登录时选择`install ubuntu`，便可以进入安装界面了

## 2. 常用软件

### ① 显卡驱动---GPU相关

安装显卡驱动：**系统设置→软件和更新→附加驱动**，选择**NVIDIA驱动**就好了。

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

> 一些注意事项：
>
> 1. 可以关闭系统内核升级
>
>    ```shell
>    sudo apt-mark hold linux-image-generic linux-headers-generic
>    sudo apt-get update
>    sudo apt-get upgrade -y
>    ```
>
>    如果你想重新启动内核升级
>
>    ```shell
>    sudo apt-mark unhold linux-image-generic linux-headers-generic
>    ```
>
> 2. 

### ② 上网客户端

下载锐捷Linux的文件，第一次使用时在包含rjsupplicant.sh文件夹下执行下述语句，“赋予./sh”文件权限

```shell
chmod a+x ./rjsupplicant.sh
```

第一次运行时，采用的命令行如下

```shell
sudo ./rjsupplicant.sh -u 账号 -p 密码 -d 1 -S 1 #(后两项分别为动态ip和保存账号密码)
```

由于第一次运行时保存了账号密码信息，因此以后使用时直接采用下述命令（如果希望在后台运行则在后面加“ &”（有个空格）

```shell
sudo ./rjsupplicant.sh
```

### ③ chrome浏览器

从官网[chrome下载](http://www.google.cn/chrome/browser/desktop/)下载.deb版本的chrome，再在下载的目录下执行下述命令

```shell
sudo dpkg -i google-chrome-stable_current_amd64.deb
```

如果遇到任何Error的话，可以执行下述命令

```shell
sudo apt-get -f install
```

但偶尔执行上述命令后还是Error，则简单暴力的使用更新命令

```shell
sudo apt-get update
sudo apt-get upgrade #(这句很多情况下不需要)
```

更多安装的方法及存在的问题可以参考：[How to install Google Chrome](http://askubuntu.com/questions/510056/how-to-install-google-chrome)

### ④ Sogou输入法

首先安装fcitx输入法框架，在应用商店直接下载，或者利用下述命令（新版本已经默认装了fcitx）

```shell
sudo apt-get install fcitx-table-wbpy
```

从官网下载[搜狗输入法Linux版本](http://pinyin.sogou.com/linux/?r=pinyin)，在执行下述命令进行安装

```shell
sudo gdebi sogoupinyin_2.1.0.0082_amd64.deb
```

> 需要安装`gdebi`：`sudo apt-get install gdebi`

在系统设置里面将语言支持中的键盘输入法系统改为：fcitx
一般情况下需重启后才会显示sogou输入法

### ⑤ 主题美化

该部分主要来自于[ZeeCoder的博客](http://blog.csdn.net/terence1212/article/details/52270210)（这位作者的博客还有很多很不错的资料）
**unity-tweak-tool**: 图形化管理工具安装

```
sudo apt-get install unity-tweak-tool
```

**Flatabulous**主题

```shell
sudo add-apt-repository ppa:noobslab/themes
sudo apt-get update
sudo apt-get install flatabulous-theme
```

及其配套图标

```shell
sudo add-apt-repository ppa:noobslab/icons
sudo apt-get update
sudo apt-get install ultra-flat-icons
```

安装完成后，打开unity-tweak-tool软件，修改主题和图标。

### ⑥ 一些实用工具

1. Typora--markdown编辑器，安装可以见[Typora官网](https://typora.io/)

   ```shell
   # optional, but recommended
   sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys BA300B7755AFCFAE
   # add Typora's repository
   sudo add-apt-repository 'deb https://typora.io/linux ./'
   sudo apt-get update
   # install typora
   sudo apt-get install typora
   ```

2. notepadqq: 类似于windows下的notepad++

   ```shell
   sudo add-apt-repository ppa:notepadqq-team/notepadqq
   sudo apt-get update
   sudo apt-get install notepadqq
   ```

3. WPS系列：从[wps官网](http://linux.wps.cn/)下载.deb文件，执行下述命令

   ```shell
   sudo dpkg -i wps-office_10.1.0.5672-a21_amd64.deb
   ```

   打开时存在字体缺失问题，解决方法下载缺失的字体，在放到fonts文件夹下，可参考这篇文章[wps字体缺失问题](https://my.oschina.net/renwofei423/blog/635798)

