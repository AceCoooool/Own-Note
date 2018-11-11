# Ubuntu下多个gcc版本切换

> 下述内容来自：[CSDN](https://blog.csdn.net/astrotycoon/article/details/8069621)

1. 检查当前使用的gcc版本：

   ```shell
   gcc -v
   ```

2. 查看当前所有安装好的gcc版本

   ```shell
   ls /usr/bin/gcc*
   ```

3. （此步可选）如果没有安装你想要的gcc版本（比如gcc-8.1，可以采用下述命令进行安装）

   ```cpp
   sudo apt-get install gcc-8.1 g++-8.1
   ```

   > 可能存在的问题，你会找不到源（即出现无法定位软件包 gcc-8.1）
   >
   > 解决方法：添加源（下述两个源供参考）
   >
   > ```shell
   > # 源1  https://launchpad.net/~ubuntu-toolchain-r/+archive/ubuntu/test
   > sudo add-apt-repository ppa:ubuntu-toolchain-r/test
   > # 源2  https://launchpad.net/~jonathonf
   > sudo add-apt-repository ppa:jonathonf/gcc-8.1
   > ```
   >
   > 在执行`sudo apt-get update`即可

4. 安装之后可以采用下述命令"设定"优先级

   ```shell
   sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8.1 100  # 优先级为100
   ```

5. 选择此版本为"默认采用版本"

   ```shell
   sudo update-alternatives --config gcc
   # 会出现类似下面的界面
   选择       路径             优先级       状态
   * 0       /usr/bin/gcc-4.8   50        自动模式
     1       /usr/bin/gcc-4.6   100       手动模式
    
   要维持当前值[*]请按回车键，或者键入选择的编号：
   ```

   选择你想设置的即可。

6. 类似的操作对g++进行操作

   ```shell
   sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-8.1 100  # 优先级为100
   sudo update-alternatives --config g++
   ```

   ​