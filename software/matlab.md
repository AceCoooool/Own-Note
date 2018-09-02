# Matlab安装教程

主要参考：[Ubuntu Matlab2016b](http://www.linuxdiyf.com/linux/27432.html)

## 下载与安装

1. [百度网盘](https://pan.baidu.com/s/1mi0PRqK)下载对应Linux版本的matlab (如果linux下不方便，可以windows下载后再拷贝)

2. 假设拷贝到`Documents`下面，挂载：

   ```
   cd Documents/
   mkdir matlab  # 临时与挂载盘建立联系
   sudo mount -t auto -o loop R2016b_glnxa64_dvd1.iso matlab/
   sudo ./matlab/install
   ```

   ① Use a File installation Key ② Yes : Next ③ 输入key: 09806-07443-53955-64350-21751-41297 ④ 建议自己改下位置，默认安装位置是在`/usr/local/MATLAB/R2016b`（可以改为/home下面的地方） ⑤ Next （我是没管，一股脑全装了）

   安装到`82%`之后，会弹出提示要求加载dvd2

   ```shell
   sudo mount -t auto -o loop R2016b_glnxa64_dvd2.iso matlab/   # 注意此时是在Documents文件夹下
   ```

## 激活

```shell
cd /usr/local/MATLAB    # 注：改为你安装的matlab位置
sudo chmod -R 777 R2016b/  # 提高权限
cd Documents/Crack文件/R2016b     # 注：改为你存放破解文件的位置
cd chmod 777 license_standalone.lic  # 提高权限
cd /usr/local/MATLAB/R2016b/bin   # 注：改为你安装的matlab位置
./matlab
```

1. Activate manually without the Internet
2. Browse: 找到`license_standalone.lic`文件
3. Next
4. 激活成功

将Crack文件夹中几个文件复制到matlab安装文件中：

```shell
# 注：下面的/usr/local/MATLAB/R2016b/改为你自己安装的位置
cd Documents/Crack/R2016b/bin/   # 改为你自己的破解文件夹位置
sudo cp glnxa64/libcufft.so.7.5.18 /usr/local/MATLAB/R2016b/bin/glnxa64
sudo cp glnxa64/libinstutil.so /usr/local/MATLAB/R2016b/bin/glnxa64
sudo cp glnxa64/libmwlmgrimpl.so /usr/local/MATLAB/R2016b/bin/glnxa64
sudo cp glnxa64/libmwservices.so  /usr/local/MATLAB/R2016b/bin/glnxa64
```

## 设置简易进入方式

如果是默认安装路径，每次打开matlab需下述命令：

```shell
cd /usr/local/MATLAB/R2016b/bin   # 注：改为你安装的matlab位置
./matlab
```

利用：

```shell
sudo ln -s /usr/local/MATLAB/R2015b/bin/matlab /usr/local/bin/matlab  # 前半部分改为你自己的matlab路径
```

之后无论在哪，只要命令行输入：`matlab`就能打开

## 更改字体

预设 ---- 字体

个人改为了`YaHei Consolas Hybrid`，视个人喜好而定