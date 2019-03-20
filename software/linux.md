1. 退出ssh：`logout`或者`exit`

2. 解压`zip`文件：

   ```shell
   # zip文件
   unzip 文件名.zip
   # 移到指定目录
   unzip -d 指定文件夹 文件名.zip
   ```

3. 建立软链接：

   ```
   ln -s 源地址 目的地址
   ```

   > 删除软链接：
   >
   > ```
   > rm -rf symbolic_name
   > ```

4. 杀掉某个进程：

   ```
   kill -9 进程号
   ```

5. 查找某个进程

   ```shell
   # 这个字符串可以看做匹配查找 --- 找到运行名字在有字符串的进程
   ps aux | grep 字符串
   ```

6. 解压`tar`文件

   ```
   tar -xvf file.tar //解压 tar包
   tar -xzvf file.tar.gz //解压tar.gz
   tar -xjvf file.tar.bz2   //解压 tar.bz2
   tar -xZvf file.tar.Z   //解压tar.Z
   unrar e file.rar //解压rar
   unzip file.zip //解压zip
   ```

7. 移动文件：

   ````shell
   mv [选项(option)] 源文件或目录 目标文件或目录
   # 例子: 将file1中所有文件移到file2中
   mv /file1/*    /file2
   # 例子: 将file1中所有txt文件移到file2中
   mv /file1/*.txt /file2
   ````
