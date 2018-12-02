# Build System (1)

> 这个系列翻译自：[PyTorch Internals Part II - The Build System](http://pytorch.org/2017/06/27/Internals2.html)

在第一个系列中，主要介绍了如何生成能够被python解释器“理解”的`torch.Tensor`对象。而在这个系列中，主要探索如何PyTorch是如何进行build的。PyTorch的代码主要可以分成下述四种不同的“组件”：

- Torch的核心库代码：TH, THC, THNN, THCUNN  （主要来源于[Torch框架](https://github.com/torch/torch7)---C代码为主）
- 供应商库：CuDNN, NCCL（均与GPU相关，后者主要用于分布式系统）
- Python的扩展库（这部分和第一个系列联系最密切）
- 额外的第三方库：NumPy, MKL, LAPACK（后面两者主要与CPU相关）

这个系列主要探索一条简单的命令`python setup.py install`是如何使得我们能够在我们的python代码中使用`import torch`这个site-package（其实就是如何将这些内容全部“组装”成可用的site-package）

这个系列整体上可用分为两个部分：第一部分主要站在一个终端用户的角度来解释build过程（即，我们是如何将前面提到的四种组件合起来进行build的），第二部分主要站在开发者的角度，提供一些建议来帮助更快速地“按需”进行build。（第一部分为主）。

该系列主要分为以下几个部分（从整体到局部）：

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

> 说明：
>
> 1. 下述大多数情况下的package指的是最终在site-package中的那个torch包
> 2. 有些内容是以自己电脑下安装的情况，而有些则是直接使用原作者文章里的内容（使用自己安装的情况的部分主要是因为有些内容在新版本里面已经有些不同了）

## Part I

### ① 整体概览

Python可以使用[Setuptools](https://setuptools.readthedocs.io/en/latest/index.html)这个第三方库来进行build其他库（PyTorch也正是借助此库来进行build的）。关于build的全部内容均在`setup.py`文件中，而该文件中又主要是通过`setup()`这个“入口”函数来进行的，因此不妨让我们先看看这个“入口”函数：

```python
cmdclass = {
    'build': build,
    'build_py': build_py,
    'build_ext': build_ext,
    'build_deps': build_deps,
    'build_module': build_module,
    'develop': develop,
    'install': install,
    'clean': clean,
}

setup(name="torch", version=version,  # meta data
         description="Tensors and Dynamic neural networks in Python with strong GPU acceleration",
         # package contents
         ext_modules=extensions,   # 扩展模块---一般指底层模块
         cmdclass=cmdclass,        # 对应各种不同的"安装行为"
         packages=packages,        # 包---纯python代码
         package_data={'torch': [     # 需要被安装的额外的文件---一般指头文件和动态库
              'lib/*.so*', 'lib/*.dylib*', 'lib/*.dll', 'lib/*.lib',
              'lib/torch_shm_manager',
              'lib/*.h',
              'lib/include/TH/*.h', 'lib/include/TH/generic/*.h',
              'lib/include/THC/*.h', 'lib/include/THC/generic/*.h',
              'lib/include/ATen/*.h',
         ]},
         install_requires=['pyyaml', 'numpy'],   # 预先需安装的库
         )
```

这个“入口”函数全都是输入参数，而这些输入参数按功能又可以分为两类：

1. 元数据：包含一些基础信息，如库的名字，版本，描述等等
2. package内容：这部分见下述描述

我们主要的关注点在第2类上面，让我们来逐个分析：

- `ext_modules`：扩展模块，主要是指那些采用“low-level”语言来实现python功能的部分。这个参数对应所有的扩展模块（以列表的形式，包括最核心的`torch._C`这个模块）--- 每个模块主要利用Extension来进行build。主要包含`torch._C, torch._dl, torch._thnn._THNN `（GPU版本还包括`torch._nvrtc, torch._thnn._THCUNN`两个模块）
- `cmdclass`：在命令行中运行`setup.py`文件时，还需要指定具体的“命令”（可以理解为参数），从而使得build按照指定的命令来进行build和安装。而每一个“命令“在`setup.py`中有其具体的实现。例如：`install`”命令“对应builds和安装包
- `packages`：该项目中使用到的packages。是指那些纯python代码的部分 --- 可以理解为最终产生的”第三方库“里面的所有python代码
- `package_data`：package中必须包含的一些额外文件：主要包含一些头文件和一些动态库文件等等（例如C代码make产生的一些.so文件）
- `install_requires`：build PyTorch库所依赖的一些第三方库，如`pyyaml`和`numpy`库 --- 即在build之前要先安装好了这几个库

后续内容，我们将更详细的介绍这些部分（主要是指`ext_modules, packages, package_data`）。但在介绍这些之前，不妨让我们先来看看最终产生的package是什么样的。

### ② site_package

在Python中，第三方包默认安装此Python对应的`lib/<version>/site_packages`目录下面。例如，我是在`pyenv`下面的[Miniconda](https://conda.io/miniconda.html)中安装PyTorch库，因此我的所有包安装的路径如下所示：

```shell
/home/ace/.pyenv/versions/miniconda3-4.3.30/lib/python3.6/site-packages
```

不妨让我们看下`torch`包里面究竟有些什么呢（安装的PyTorch第三方库对应`torch`包）：

```shell
:torch $ls
autograd  onnx  backends  optim  _C.cpython-36m-x86_64-linux-gnu.so  __pycache__  contrib  random.py  cuda  serialization.py  distributed  _six.py  distributions.py  sparse  
_dl.cpython-36m-x86_64-linux-gnu.so  _storage_docs.py  for_onnx  storage.py  functional.py  _tensor_docs.py  __init__.py  tensor.py  jit  _tensor_str.py  legacy  _thnn  lib  _torch_docs.py
multiprocessing  utils  nn  _utils.py  _nvrtc.cpython-36m-x86_64-linux-gnu.so  version.py
```

上述这些”内容“包含了所有我们所需要的，具体可以分为下述三类：

- 所有的”纯“packages（对应①中的`packages`）--- 即所有纯Python代码

- 扩展库 --- 指的是`_C.xxx.so`，`_dl.xxx.so`，`_nvrtc.xxx.so`（当然还有`_thnn`文件夹下面的`_THNN.xxx.so`和`_THCUNN.xxx.so`）等

- package_data：对应`lib/`文件夹里面的所有内容 --- `.so`文件和`.h`文件

  ```shell
  torch $ ls lib/
  include                     libnccl.so.1  libTHCUNN.so.1  THCUNN.h
  libATen.so.1                libshm.so     libTHNN.so.1    THNN.h
  libcudnn-a2b758a6.so.7.0.3  libTHC.so.1   libTH.so.1      torch_shm_manager
  libnccl.so                  libTHCS.so.1  libTHS.so.1
  ```

Python解释器在`import`过程中会搜索`site_packages`来寻找是否有对应的第三方库。例如，当我们在Python代码中`import torch`时，会在`site_packages`中寻找到`torch`这个库，然后初始化并导入。更多关于`import`机制等内容可以参考[modules](https://docs.python.org/3/tutorial/modules.html)。

