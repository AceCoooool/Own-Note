# Build System (3)

> 这个系列翻译自：[PyTorch Internals Part II - The Build System](http://pytorch.org/2017/06/27/Internals2.html)

让我们接着[Build System (1)](./Build1.md), [Build System (2)](./Build2.md)来介绍

目录：

Part I：站在全局的角度

- 整体概述
- site_package

Part II：各模块的build

- Build ATen部分和第三方库
- Build纯Python模块
- Build扩展模块
- 安装

Part III：站在开发者的角度

- Setuptools开发者模式
- 依赖库
- 第三方库
- 相关但未提到的内容

## Part III

由于PyTorch整个安装过程是非常耗时的。而在对PyTorch实际开发过程中，我们可能仅仅只是对其中的某个子部分进行修改，那么只要重新build这部分自然是最理想的选择。

### ① Setuptools开发者模式

要实现上述任务，主要是借助了`setuptools`的`develop`命令。关于该`develop`命令，[文档](https://setuptools.readthedocs.io/en/latest/setuptools.html#development-mode)里介绍如下：

> 这个命令允许你在自己的某个“staging area”部署项目，且能被Python通过`import`导入。这是由于这种部署方式，使得你在项目里面修改的代码能够立即在这个“staging area”里生效，而无需重新build和install。

但是这究竟是如何工作的呢？假设我们在pytorch目录下面运行`python setup.py build develop`。则`build`命令先被执行，从而使得那些依赖的库(如`TH`等---可以理解为Part II中的①)和扩展模块(Part II中的③)进行building。但是，如果我们看下python的site-packages，我们会发现只有`torch.egg-link`：

```shell
(p3) killeent@devgpu047:site-packages$ ls -la torch*
-rw-r--r--. 1 killeent users 31 Jun 27 08:02 torch.egg-link
```

我们可以查看下`torch.egg-link`的内容，其实就是简单的对pytorch文件夹的引用：

```shell
(p3) killeent@devgpu047:site-packages$ cat torch.egg-link
/home/killeent/github/pytorch
```

如果我们返回到pytorch文件夹，我们会看到有一个名为`torch.egg-info`的文件夹：

```shell
(p3) killeent@devgpu047:pytorch (master)$ ls -la torch.egg-info/
total 28
drwxr-xr-x.  2 killeent users  4096 Jun 27 08:09 .
drwxr-xr-x. 10 killeent users  4096 Jun 27 08:01 ..
-rw-r--r--.  1 killeent users     1 Jun 27 08:01 dependency_links.txt
-rw-r--r--.  1 killeent users   255 Jun 27 08:01 PKG-INFO
-rw-r--r--.  1 killeent users     7 Jun 27 08:01 requires.txt
-rw-r--r--.  1 killeent users 16080 Jun 27 08:01 SOURCES.txt
-rw-r--r--.  1 killeent users    12 Jun 27 08:01 top_level.txt
```

这个文件夹包含了pytorch项目的所有元信息。例如，`requires.txt`罗列了所有pytorch的依赖库：

```shell
(p3) killeent@devgpu047:pytorch (master)$ cat torch.egg-info/requires.txt
pyyaml numpy
```

无需关心太多细节，你可以认为`develop`模式允许我们将pytorch项目本身视为在site-packages里面（类似linux里面的link），因此我们能够直接导入并使用该项目作为第三方库：

```shell
(p3) killeent@devgpu047:~$ python
Python 3.6.1 |Continuum Analytics, Inc.| (default, Mar 22 2017, 19:54:23)
[GCC 4.4.7 20120313 (Red Hat 4.4.7-1)] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> import torch
>>> torch.__file__
'/home/killeent/github/pytorch/torch/__init__.py'
```

我们有如下结论：

- 如果我们改变pytorch项目里面的python代码，则这种改变会被直接捕获。我们无需运行任何命令，python解释器能够”看到“这些变化（即修改纯python代码，无需任何多余操作）
- 如果我们修改了扩展模块中的C++源代码，则我们需要重新运行`develop`命令来重新build扩展项目

这样我们就能够方便地开发和测试我们的pytorch代码库了。

### ② 依赖库

如果我们需要”修改“依赖的库（如`TH`等），我们可以通过`build_deps`命令直接重新build这些依赖库，该命令会自动调用`build_libs.sh`脚本来重新build我们的库，并将生成的库复制到正确的地方。如果我们使用setuptools的`develop`模式，我们将会使用在pytorch项目本地已build的扩展模块。由于我们已经对编译的扩展库的共享库指定了路径，因此这些改变将会被捕获到（即使用修改了依赖库）：

```shell
# we are using the local extension
(p3) killeent@devgpu047:~$ python
Python 3.6.1 |Continuum Analytics, Inc.| (default, Mar 22 2017, 19:54:23)
[GCC 4.4.7 20120313 (Red Hat 4.4.7-1)] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> import torch
>>> torch._C.__file__
'/home/killeent/github/pytorch/torch/_C.cpython-36m-x86_64-linux-gnu.so'

# it references the local shared object library we just re-built
(p3) killeent@devgpu047:~$ ldd /home/killeent/github/pytorch/torch/_C.cpython-36m-x86_64-linux-gnu.so
# ...
libTH.so.1 => /home/killeent/github/pytorch/torch/lib/libTH.so.1 (0x00007f543d0e2000)
# ...
```

正是因为这样，我们无需整体全部重新编译就可以测试我们的更改了

### ③ 第三方库

pytorch依赖一些第三方库，最常用的方式是通过Anaconda来安装它们。例如，我们可以通过下述方式利用`mkl`这个第三方库：

```shell
# installed to miniconda2/envs/p3/lib/libmkl_intel_lp64.so
conda install mkl
```

只要我们将此库的目录包含在我们的`$CMAKE_PREFIX_PATH`，我们就能够在编译时成功找到该库：

```shell
# in the site-packages dir
(p3) killeent@devgpu047:torch$ ldd _C.cpython-36m-x86_64-linux-gnu.so
# ...
libmkl_intel_lp64.so => /home/killeent/local/miniconda2/envs/p3/lib/libmkl_intel_lp64.so (0x00007f3450bba000)
# ...
```

### ④ 相关但未提到的内容

- 如何使用`ccache`来加快build的速度
- pytorch上层目录里的`__init__.py`是如何处理模块的初始化导入和如何将各种不同的模块和扩展库”统筹兼顾“
- CMake机制 --- 后端库是如何通过CMake进行build的