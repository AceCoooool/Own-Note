# Python安装相关

## 1. Python版本管理

由于python存在多个版本，以及各版本之间存在一定的差异，因此可以采用[pyenv](https://github.com/yyuu/pyenv)进行版本管理和切换，具体安装可参考[官方教程](https://github.com/yyuu/pyenv)和[pyenv安装教程](https://my.oschina.net/lionets/blog/267469)。

主要步骤如下：

1. 依赖包安装，可参考[依赖包安装](https://github.com/yyuu/pyenv/wiki/Common-build-problems#requirements)

   ```shell
   sudo apt-get install -y make build-essential libssl-dev zlib1g-dev libbz2-dev \
   libreadline-dev libsqlite3-dev wget curl llvm libncurses5-dev libncursesw5-dev xz-utils
   ```

2. clone文件

   ```shell
   git clone git://github.com/yyuu/pyenv.git .pyenv
   ```

3. 配置指令

   ```shell
   # 添加环境变量
   echo 'export PYENV_ROOT="$HOME/.pyenv"' >> ~/.bashrc
   echo 'export PATH="$PYENV_ROOT/bin:$PATH"' >> ~/.bashrc
   # 向 shell 添加 pyenv init 以启用 shims 和命令补完功能
   echo 'eval "$(pyenv init -)"' >> ~/.bashrc
   # 重启 shell
   exec $SHELL
   ```

4. 安装python

   > 可以通过`pyenv install -l`查看可以安装哪些版本的python
   >
   > 建议安装anaconda：例如`pyenv install anaconda3-5.2.0`
   >
   
   在安装python之前，先安装tk-dev（因为在之后安装matplotlib时需thinker模块）

   ```shell
   # 安装tk-dev
   sudo apt-get install tk-dev
   # 安装python3.6
   pyenv install 3.6.0
   # 设置全局python
   pyenv global 3.6.0
   ```

5. pip安装库
   由于数据科学要用到一些库，现在全局python已经为3.6.0，因此可以直接使用pip进行安装，例如安装numpy库，大部分库都可以通过类似方法完成

   ```python
   pip install -U numpy # -U代表如果存在更新则更新
   ```

> 关于pyenv更新，可以参考[pyenv update](https://github.com/pyenv/pyenv-update)
>
> ```shell
> git clone git://github.com/pyenv/pyenv-update.git ~/.pyenv/plugins/pyenv-update
> pyenv update
> ```

6. 如果在`pyenv install anancondaxxx`出现"下载不了"（类似下述错误）

   ```cpp
   Downloading Anaconda3-5.1.0-Linux-x86_64.sh...
   -> https://repo.continuum.io/archive/Anaconda3-5.1.0-Linux-x86_64.sh
   error: failed to download Anaconda3-5.1.0-Linux-x86_64.sh

   BUILD FAILED (Ubuntu 16.04 using python-build 1.2.2-119-gec9fb54)

   Inspect or clean up the working tree at /tmp/python-build.20181213201209.3537
   Results logged to /tmp/python-build.20181213201209.3537.log

   Last 10 log lines:
   /tmp/python-build.20181213201209.3537 ~
   curl: (56) GnuTLS recv error (-54): Error in the pull function.
   ```

   可以参考[pyenv issue597](https://github.com/pyenv/pyenv/issues/597)，采用下面方式手动安装

   ```shell
   $ wget -P $(pyenv root)/cache https://repo.continuum.io/archive/Anaconda3-4.0.0-Linux-x86_64.sh
   ```

   ​

## 2. Pycharm IDE

在[官网](https://www.jetbrains.com/pycharm/)下载最新的pycharm IDE，之后在本地解压即可

1. 运行IDE

   ```shell
   cd /your_path_of_pycharm/bin
   sudo ./pycharm.sh
   ```

> 可以改为更方便的方式：
>
> ```shell
> sudo ln -s /your_path_to_pycharm/bin/pycharm.sh /usr/local/bin/pycharm
> ```
>
> 之后你只需在命令行输入：
>
> ```shell
> pycharm
> ```
>
> 即可

2. 添加fcitx支持
   将`./pycharm.sh`文件里面下入下述语句

   ```shell
   export GTK_IM_MODULE=fcitx
   export QT_IM_MODULE=fcitx
   export XMODIFIERS=@im=fcitx
   ```



   ​	