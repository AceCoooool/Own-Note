## 手动安装

1. 安装依赖项：

   ```shell
   sudo apt-get install build-essential libgtk2.0-dev libvtk5-dev libjpeg-dev libtiff4-dev libjasper-dev libopenexr-dev libtbb-dev
   ```

   > 其实进一步安装一些依赖的话，可以参照下面（Options）：
   >
   > ```shell
   > sudo apt-get install build-essential
   > sudo apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
   > sudo apt-get install python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev # 处理图像所需的包
   > sudo apt-get install libavcodec-dev libavformat-dev libswscale-dev libv4l-dev
   > sudo apt-get install libxvidcore-dev libx264-dev # 处理视频所需的包
   > sudo apt-get install libatlas-base-dev gfortran # 优化opencv功能
   > sudo apt-get install ffmpeg  # 此项可以采用其他方式
   > ```
   >
   > 注：其中`ffmpeg`可以采用PPA方式安装（二选一）
   >
   > ```shell
   > sudo apt-add-repository ppa:mc3man/trusty-media
   > sudo apt-get update
   > sudo apt-get install ffmpeg
   > ```

2. 下载opencv（可以直接从官网下载：[OpenCV-release](https://opencv.org/releases.html)）

   ```
   wget https://github.com/opencv/opencv/archive/3.4.5.zip   # 从github上直接下载或者clone也可
   wget https://github.com/opencv/opencv_contrib/archive/3.4.5.zip
   ```
