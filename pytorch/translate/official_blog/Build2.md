# Build System (2)

> 这个系列翻译自：[PyTorch Internals Part II - The Build System](http://pytorch.org/2017/06/27/Internals2.html)

让我们接着[Build System (1)](./Build1.md)来介绍

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

## Part II

### ① Build ATen部分和第三方库（原本是Torch核心部分）

让我们先看看`setup.py`中的`install`命令（继承自`setuptools.command.install.install`的类）

```python
class install(setuptools.command.install.install):

    def run(self):
        if not self.skip_build:
            self.run_command('build_deps')   # 运行build_deps "Command"
        setuptools.command.install.install.run(self)  # 这条命令会进一步执行build这个类
```

可见，上述命令首先做的是调用`build_deps`的命令（对应的也是一个类）。因此，我们来看看`build_deps`这个命令里面的`run`函数的内容：

```python
def run(self):
        # Check if you remembered to check out submodules
        def check_file(f):
            if not os.path.exists(f):
                print("Could not find {}".format(f))
                print("Did you run 'git submodule update --init'?")
                sys.exit(1)
        # 检查几个文件夹是否存在
        check_file(os.path.join(lib_path, "gloo", "CMakeLists.txt"))
        check_file(os.path.join(lib_path, "nanopb", "CMakeLists.txt"))
        check_file(os.path.join(lib_path, "pybind11", "CMakeLists.txt"))

        libs = []
        if WITH_NCCL and not WITH_SYSTEM_NCCL:
            libs += ['nccl']
        libs += ['ATen', 'nanopb']
        if IS_WINDOWS:
            libs += ['libshm_windows']
        else:
            libs += ['libshm']
        if WITH_DISTRIBUTED:
            if sys.platform.startswith('linux'):
                libs += ['gloo']
            libs += ['THD']
        build_libs(libs)  # 对ATen, nanopb, libshm这3个库进行build (如果是cpu情况)
```

郁闷的你可能发现其又调用了另一个函数`build_libs()`，那么我们再来看看这个函数究竟起什么作用：

```python
def build_libs(libs):
    for lib in libs:
        assert lib in dep_libs, 'invalid lib: {}'.format(lib)
    if IS_WINDOWS:
        build_libs_cmd = ['torch\\lib\\build_libs.bat']
    else:
        build_libs_cmd = ['bash', 'torch/lib/build_libs.sh'] 
    my_env = os.environ.copy()
    my_env["PYTORCH_PYTHON"] = sys.executable
    if not IS_WINDOWS:
        if WITH_NINJA:
            my_env["CMAKE_GENERATOR"] = '-GNinja'
            my_env["CMAKE_INSTALL"] = 'ninja install'
        else:
            my_env['CMAKE_GENERATOR'] = ''
            my_env['CMAKE_INSTALL'] = 'make install'
    if WITH_SYSTEM_NCCL:
        my_env["NCCL_ROOT_DIR"] = NCCL_ROOT_DIR
    if WITH_CUDA:
        my_env["CUDA_BIN_PATH"] = CUDA_HOME
        build_libs_cmd += ['--with-cuda']
    if WITH_NNPACK:
        build_libs_cmd += ['--with-nnpack']
    if WITH_CUDNN:
        my_env["CUDNN_LIB_DIR"] = CUDNN_LIB_DIR
        my_env["CUDNN_LIBRARY"] = CUDNN_LIBRARY
        my_env["CUDNN_INCLUDE_DIR"] = CUDNN_INCLUDE_DIR
    # 这一步等价于执行bash build_libs.sh ATen nanopb libshm (CPU情况)
    if subprocess.call(build_libs_cmd + libs, env=my_env) != 0:
        sys.exit(1)
	# 这一步进行cwrapper过程: 将THNN和THCUNN"变为"CPython的格式
    if 'ATen' in libs:
        from tools.nnwrap import generate_wrappers as generate_nn_wrappers
        generate_nn_wrappers()
```

上面整个过程中最终的一条语句就是`subprocess.call(build_libs_cmd + libs, env=my_env)`，相当于在命令行执行了`bash build_libs.sh ATen nanopb libshm`，而这又牵扯到了`torch/lib/build_libs.sh`这个Bash脚本，关于这个脚本的一些注释可以参考[build_libs.sh注释版](./code/build_libs.sh)。总归这个脚本主要的作用就是对`ATen, nanpopb, libshm`这三个文件夹(说明：除了ATen文件夹之外，其他文件夹都在此`lib`目录下面。GPU版本还有一些其他文件夹)进行make（就是我们常见的采用CMake方式的make操作），然后将这些make后的动态库和`THNN.h, THCUNN.h`复制到`lib`目录下，以及将全部动态库和头文件等全部信息复制到`lib/tmp_install`（这个文件夹是Bash脚本中新建的）中。

让我们来对比一下上述脚本执行前后的`lib`文件夹：

```shell
# 脚本执行前
lib $ ls
build_libs.bat  gloo    libshm_windows  nccl      README.md
build_libs.sh   libshm  nanopb          pybind11  THD
# 脚本执行后
lib $ ls
build           libgloo.a             libshm.so       THCUNN.h
build_libs.bat  libgloo_builder.a     libshm_windows  THD
build_libs.sh   libgloo_cuda.a        libTHD.a        THNN.h
gloo            libnccl.so            nanopb          tmp_install
include         libnccl.so.1          nccl            torch_shm_manager
libATen.so      libprotobuf-nanopb.a  pybind11
libATen.so.1    libshm                README.md
```

可以发现，在`lib`目录下多了很多额外的东西：

- 每个库的共享库(即`.so`文件) --- 如nccl库对应的`libnccl.so`
- THNN和THCUNN的头文件 --- `THNN.h`和`THCUNN.h`
- `build`和`tmp_install`文件夹
- `torch_shm_manager`可执行文件 --- 与共享内存管理相关

让我们来看看`tmp_install`里面的内容：

```shell
lib $ ls tmp_install/
bin  include  lib  share  THD_deps.txt
```

`tmp_install`看起来像是一个标准的安装文件：包含binaries，头文件(include)和库文件(lib)。举个例子： `tmp_install/include/TH`中包含所有`TH`的头文件，`tmp_install/lib/`中包含`libTH.so.1`文件(新版本中此动态库包含在`libATen.so.1`中)

你可能好奇我们为什么需要这个文件夹呢？这主要是用来编译那些相互依赖的库。例如，`THC`库依赖`TH`库和其头文件，在脚本里面则是通过`cmake`里面的参数体现：

```shell
# 其中的install_dir就是tmp_install
cmake ...
	-DTH_INCLUDE_PATH="$INSTALL_DIR/include" \
	-DTH_LIB_PATH="$INSTALL_DIR/lib" \
```

我们可以通过查看我们build的`THC`库来验证上述说法（新版本里面`THC`等库都并到了`ATen`中了）：

```shell
lib $ ldd libTHC.so.1
	...
	libTH.so.1 => /home/killeent/github/pytorch/torch/lib/tmp_install/lib/./libTH.so.1 (0x00007f84478b7000)
```

在`build_libs.sh`中对include和library路径采用的方式有些混乱，但是能够完成任务。最后，让我们来看看这个脚本的最后几行：

```shell
rm -rf "$INSTALL_DIR/lib/cmake"
rm -rf "$INSTALL_DIR/lib/python"
cp "$INSTALL_DIR/lib"/* .
if [ -d "$INSTALL_DIR/lib64/" ]; then
    cp "$INSTALL_DIR/lib64"/* .
fi
cp ../../aten/src/THNN/generic/THNN.h .
cp ../../aten/src/THCUNN/generic/THCUNN.h .
cp -r "$INSTALL_DIR/include" .
if [ -d "$INSTALL_DIR/bin/" ]; then
    cp "$INSTALL_DIR/bin/"/* .
fi
```

可以看到，这几行的作用是删除一些“无用”的文件和复制所有的东西到上层目录`torch/lib`目录中去（后续会解释这么做的原因）

执行完脚本之后，我们会发现`build_libs`函数里面还有一小段我们还没探索呢：

```python
if 'ATen' in libs:
  from tools.nnwrap import generate_wrappers as generate_nn_wrappers
  generate_nn_wrappers()   # 执行类似cwrap操作
```

回顾一下在“上一个系列”中我们是如何将底层库（类似`TH`等）“捆绑”到PyTorch中的：利用cwrap工具（Python函数）将我们手动“编写”的`YAML`文件解析成CPython兼容的函数形式。但是，由于`THNN`和`THCUNN`这两个库相对比较简单，我们没有必要手动“编写”`YAML`文件再解析，而是直接利用wrap工具（即上面的`generate_nn_wrappers`）来自动生成cwrap定义文件和最终与CPython兼容的C++代码。

我们将`THNN.h`和`THCUNN.h`这两个头文件复制到`torch/lib`目录下面也是因为`generate_nn_wrappers()`函数默认认为这两个文件的目录在此。其实`generate_nn_wrappers()`这个函数主要做下述几件事：

1. 解析头文件，并生成YAML格式的函数声明，并将它们写入到`.cwrap`文件中
2. 调用cwrap工具解析这些生成的`.cwrap`文件，生成对应的`C++`文件

运行完`generate_nn_wrappers()`之后，我们会发现`torch/csrc`中多了一个`nn`文件夹，来看看里面有什么：

```shell
nn $ ls
THCUNN.cpp  THCUNN.cwrap  THNN.cpp  THNN.cwrap  type_checks.h
```

让我们再来看下自动生成的`.cwrap`里面的一条内容：

```yaml
TH_API void THNN_FloatAbs_updateOutput(void*, THFloatTensor*, THFloatTensor*);
[[
  name: FloatAbs_updateOutput
  return: void
  cname: THNN_FloatAbs_updateOutput
  arguments:
    - void* state
    - THFloatTensor* input
    - THFloatTensor* output
]]
```

以及其对应的`.cpp`内容：

```c++
TH_API void THNN_FloatAbs_updateOutput(void*, THFloatTensor*, THFloatTensor*);

PyObject * FloatAbs_updateOutput(PyObject *_unused, PyObject *args)
{
  HANDLE_TH_ERRORS
  int __argcount = args ? PyTuple_Size(args) : 0;
    
    if (__argcount == 3 &&
          THPUtils_checkLong(PyTuple_GET_ITEM(args, 0)) &&
          THNN_FloatTensor_Check(PyTuple_GET_ITEM(args, 1)) &&
          THNN_FloatTensor_Check(PyTuple_GET_ITEM(args, 2))) {
      

      void* arg_state = (void*)THPUtils_unpackLong(PyTuple_GET_ITEM(args, 0));
      THFloatTensor* arg_input = THNN_FloatTensor_Unpack(PyTuple_GET_ITEM(args, 1));
      THFloatTensor* arg_output = THNN_FloatTensor_Unpack(PyTuple_GET_ITEM(args, 2));
      
      PyThreadState *_save = NULL;
      try {
        Py_UNBLOCK_THREADS;
        THNN_FloatAbs_updateOutput(arg_state, arg_input, arg_output);
        Py_BLOCK_THREADS;
        Py_RETURN_NONE;
      } catch (...) {
        if (_save) {
          Py_BLOCK_THREADS;
        }
        throw;
      }
    
  } else {
    THPUtils_invalidArguments(args, NULL, "FloatAbs_updateOutput", 1, "(int state, torch.FloatTensor input, torch.FloatTensor output)");
    return NULL;
  }
  END_HANDLE_TH_ERRORS
}
```

上述过程主要是为了将底层代码转化为CPython兼容的形式。

### ② Build纯Python模块

在`setup.py`中执行完`build_deps()`里面的内容，回到`install()`里面中，可以发现只剩下一条语句了：

```python
setuptools.command.install.install.run(self)
```

这条语句会调用`build`这个类

```python
class build(distutils.command.build.build):
    sub_commands = [
                       ('build_deps', lambda self: True),
                   ] + distutils.command.build.build.sub_commands # 调用其他的Command
```

然后会调用好几个其他`Command`类的。首先调用的就是`build_py()`命令---即这一部分的内容（build纯Python模块）。这一部分会处理参数中的`packages`这一项。

我们的`packages`是利用`find_packages()`来寻找的：

```python
packages = find_packages(exclude=('tools', 'tools.*',))
# 找到的packages如下所示---所有包含__init__.py的文件夹
['torch', 'torch.testing', 'torch.utils', 'torch.distributions', 'torch.distributed', 'torch.nn', 'torch.onnx', 'torch.optim', 'torch.backends', 'torch.cuda', 'torch.multiprocessing', 'torch.legacy', 'torch._thnn', 'torch.sparse', 'torch.for_onnx', 'torch.autograd', 'torch.contrib', 'torch.jit', 'torch.utils.backcompat', 'torch.utils.serialization', 'torch.utils.trainer', 'torch.utils.ffi', 'torch.utils.data', 'torch.utils.trainer.plugins', 'torch.nn.utils', 'torch.nn.modules', 'torch.nn.parallel', 'torch.nn.backends', 'torch.nn._functions', 'torch.nn._functions.thnn', 'torch.backends.cudnn', 'torch.legacy.nn', 'torch.legacy.optim', 'torch.autograd._functions']
```

我们可以发现，`find_package`会搜索`torch`目录下面所有的包含`__init__.py`文件夹

使用`setuptools`进行building时，会在`setup.py`同级目录下面创建一个build文件夹（用来保存building结果）。由于PyTorch库既有纯Python模块又有扩展模块，因此有必要保留操作系统和Python版本信息。来看看生成的build文件夹：

```shell
pytorch $ ls build
lib.linux-x86_64-3.6  temp.linux-x86_64-3.6
```

“后缀”说明此次编译的操作系统环境为`linux-x86_64`，采用的Python版本为3.6。在lib目录下面包含整个库，而temp文件夹包含所有build过程生成的文件（这个文件夹并不会影响安装）

由于纯Python模块内全都是Python代码，无需进行“编译”，因此`build_py`过程就是简单的将利用`find_packages`找到的所有内容复制到`build/`中去即可。因此，该过程执行的内容类似下面语句

```shell
copying torch/autograd/_functions/blas.py -> build/lib.linux-x86_64-3.6/torch/autograd/_functions
```

在[Build1](./Build1.md)中我们也提到了`setup()`参数项里面的`package_data`（需要被安装的额外的文件---一般指头文件和动态库），因此也需要将这些内容全部复制到`build/`文件夹里面。该过程执行的内容如下所示：

```shell
copying torch/lib/libATen.so.1 -> build/lib.linux-x86_64-3.6/torch/lib
...
copying torch/lib/include/THC/generic/THCTensor.h -> build/lib.linux-x86_64-3.6/torch/lib/include/THC/generic
```

### ③ Build扩展模块

Build完核心库和纯Python库之后，我们需要build扩展模块。PyTorch的后端可以认为是CPython写的C++代码（当然是通过几个cwrap工具来自动将“纯C++/C”转换为CPython形式），而对这一部分的build是最麻烦的，占据了`setup.py`文件里面的大部分内容（即其中的大部分代码是为了build这一部分而写的）。

在执行完`build_py`之后，会接着执行`build_ext`部分，来看看这个命令里面`run`的内容：

```python
def run(self):

	# Print build options
    # ...省略了一系列检查的代码
	generate_code(ninja_global)
    # ...省略了一堆"windows"和"ninja"相关的代码
    # It's an old-style class in Python 2.7...
    setuptools.command.build_ext.build_ext.run(self)
```

我们可以发现其中很重要的一句就是`generate_code(ninja_global)`，因此让我们进入`generate_code.py`这一部分再看看：

```python
def generate_code(ninja_global=None):
    # if ninja is enabled, we just register this file as something
    # ninja will need to call if needed
    if ninja_global is not None:
        return generate_code_ninja(ninja_global)
    # cwrap depends on pyyaml, so we can't import it earlier
    root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    sys.path.insert(0, root)
    from tools.cwrap import cwrap
    from tools.cwrap.plugins.THPPlugin import THPPlugin
    from tools.cwrap.plugins.ArgcountSortPlugin import ArgcountSortPlugin
    from tools.cwrap.plugins.AutoGPU import AutoGPU
    from tools.cwrap.plugins.BoolOption import BoolOption
    from tools.cwrap.plugins.KwargsPlugin import KwargsPlugin
    from tools.cwrap.plugins.NullableArguments import NullableArguments
    from tools.cwrap.plugins.WrapDim import WrapDim
    from tools.cwrap.plugins.AssertNDim import AssertNDim
    from tools.cwrap.plugins.Broadcast import Broadcast
    from tools.cwrap.plugins.ProcessorSpecificPlugin import ProcessorSpecificPlugin
    from tools.autograd.gen_autograd import gen_autograd
    from tools.jit.gen_jit_dispatch import gen_jit_dispatch
    thp_plugin = THPPlugin()

    cwrap('torch/csrc/generic/TensorMethods.cwrap', plugins=[
        ProcessorSpecificPlugin(), BoolOption(), thp_plugin,
        AutoGPU(condition='IS_CUDA'), ArgcountSortPlugin(), KwargsPlugin(),
        AssertNDim(), WrapDim(), Broadcast()
    ])
    # Build ATen based Variable classes
    autograd_gen_dir = 'torch/csrc/autograd/generated'
    jit_gen_dir = 'torch/csrc/jit/generated'
    for d in (autograd_gen_dir, jit_gen_dir):
        if not os.path.exists(d):
            os.mkdir(d)
    gen_autograd('torch/lib/tmp_install/share/ATen/Declarations.yaml', autograd_gen_dir)
    gen_jit_dispatch('torch/lib/tmp_install/share/ATen/Declarations.yaml',jit_gen_dir)
```

回顾一下之前提到到自动生成C++（如`THNN`库）。这里就是将`TH`，`THC`，`CuDNN`全部”捆绑“在一起。我们使用YAML形式的声明文件`TensorMethods.cwrap`来生成C++文件（兼容CPython，可以理解为CPython的壳，调用写的纯C++代码），而这正是PyTorch代码”高效“的缘由。以`zero_`函数为例：

```yaml
# YAML形式的声明
[[
  name: zero_
  cname: zero
  return: self
  arguments:
    - THTensor* self
]]
```

生成的C++代码：

```c++
PyObject * THPTensor_(zero_)(PyObject *self, PyObject *args, PyObject *kwargs) {
	...
	THTensor_(zero)(LIBRARY_STATE arg_self);
	...
}
```

在上一个系列中，我们讲述了这些函数是如何和Tensor类型联系起来的，因此在这个系列里面就不展开讲述了。对于build过程，知道这些C++文件是在扩展模块进行build之前已经生成就足够了（因为在扩展模块的编译阶段需要用到这些文件）。

**扩展模块在build时的一些不同**

之前的模块只需列出模块和包，然后setuptools会帮你找到正确的的文件。但在build扩展模块时，你需要自己指定扩展模块的名字，源文件以及一些编译/链接的需求（编译包含的文件，连接的库等等）

这也是为什么在`setup.py`文件中，大量的代码用来”说明“如何build这些扩展模块。在这一部分，我们会发现之前的脚本`build_libs.sh`中一些操作的意义。例如，在build核心代码时创建的`tmp_install`文件夹 --- 在`setup.py`代码中我们会使用到这个文件夹，并将其中那些包含头文件的文件夹包含进来（`tmp_install/include`里面的一些文件夹）

```python
tmp_install_path = lib_path + "/tmp_install"
include_dirs += [
    cwd,
    os.path.join(cwd, "torch", "csrc"),
    lib_path + "/pybind11/include",
    tmp_install_path + "/include",
    tmp_install_path + "/include/TH",
    tmp_install_path + "/include/THNN",
    tmp_install_path + "/include/ATen",
]
```

类似的，之前的脚本会将`.so`(shared object libraries)文件复制到`torch/lib`文件夹中，我们会在`setup.py`中直接用到这些动态链接库：

```python
# lib_path is torch/lib
ATEN_LIB = os.path.join(lib_path, 'libATen.so.1')
THD_LIB = os.path.join(lib_path, 'libTHD.a')
NCCL_LIB = os.path.join(lib_path, 'libnccl.so.1')
```

来看看是如何build我们主要的`torch._C`扩展模块：

```python
C = Extension("torch._C",
              libraries=main_libraries,
              sources=main_sources,
              language='c++',
              extra_compile_args=main_compile_args + extra_compile_args,
              include_dirs=include_dirs,
              library_dirs=library_dirs,
              extra_link_args=extra_link_args + main_link_args + [make_relative_rpath('lib')],
              )
```

- `main_libraries`：我们需要链接的所有库（CPU版本对应`shm`，GPU版本还有额外的`cudart, nvToolsExt, cudnn`）。其中的`shm`是PyTorch的共享内存管理库。需要注意的是，类似`TH`等库并没有在此处

- `main_source`：组成PyTorch后端的所有C++文件（与CPython兼容---可以看成那些`ATen.so`的马甲）

- `main_compile_args+extra_compile_args`：编译时的参数。例如，我们可能需要加上debug的flags在Debug模式下面

- `include_dirs`：包含头文件的所有文件夹。这也反映了之前脚本`build_libs.sh`的一些”贡献“---例如，我们可以直接在`torch/lib/tmp_install/include/TH`中找到所有与`TH`相关的头文件

- `library_dirs`：在链接阶段用到的所有共享库(`.so`文件)。例如此`library_dirs`包含`torch/lib`这个文件夹，就包含了那些我们复制到此目录下的所有`.so`文件

- `extra_link_args + main_link_args + [make_relative_rpath('lib')]`：创建扩展时链接目标文件时用到。在PyTorch中，包含更多常规选项，类似链接`libstdc++`等。此外，还有一个非常关键的部分：我们在这里链接我们的后端库`ATen`等！

  ```python
  ATEN_LIB = os.path.join(lib_path, 'libATen.so.1')
  NANOPB_STATIC_LIB = os.path.join(lib_path, 'protobuf-nanopb.lib')
  main_link_args = [ATEN_LIB, NANOPB_STATIC_LIB]
  ```

你可能好奇我们为什么不将这些后端库放到`main_libraries`中，而是放到这个`extra_link_args`中来。主要的原因是为了避免与Torch库安装时的冲突，因为在Lua Torch库安装时，通常会设置`LD_LIBRARY_PATH`等，从而使得我们可能错误地将PyTorch链接到Torch的后端库。

除了`torch._C`扩展库之外，这里还有几个其他的扩展库来使得PyTorch变得更好。这几个扩展库的build方式和`torch._C`是类似的，就不展开讲了。如果build成功了，我们可以进入安装阶段了。

### ④ 安装

在building完成了，安装是非常简单的。我们只需要将`build/lib.linux-x86_64-3.6`里的全部内容复制到指定的安装文件夹下就好了。回顾一下在第一部分[Build 1](./Build1.md)中我们最终得到的库在特定python的`site_packages`中，因此在安装阶段，我们看到的内容就类似下面的：

```shell
running install_lib
creating /home/killeent/local/miniconda2/envs/p3/lib/python3.6/site-packages/torch
copying build/lib.linux-x86_64-3.6/torch/_C.cpython-36m-x86_64-linux-gnu.so -> /home/killeent/local/miniconda2/envs/p3/lib/python3.6/site-packages/torch
copying build/lib.linux-x86_64-3.6/torch/_dl.cpython-36m-x86_64-linux-gnu.so -> /home/killeent/local/miniconda2/envs/p3/lib/python3.6/site-packages/torch
creating /home/killeent/local/miniconda2/envs/p3/lib/python3.6/site-packages/torch/_thnn
copying build/lib.linux-x86_64-3.6/torch/_thnn/_THNN.cpython-36m-x86_64-linux-gnu.so -> /home/killeent/local/miniconda2/envs/p3/lib/python3.6/site-packages/torch/_thnn
copying build/lib.linux-x86_64-3.6/torch/_thnn/_THCUNN.cpython-36m-x86_64-linux-gnu.so -> /home/killeent/local/miniconda2/envs/p3/lib/python3.6/site-packages/torch/_thnn
```

其实已经全部完成了，当我们执行`import torch`时，就会在`site_packages`中寻找到这个库。

再来稍微看下是如何找到这个库的。首先，Python会在特定的目录下搜索库：

```shell
# note we are now in my home directory
(p3) killeent@devgpu047:~$ python
Python 3.6.1 |Continuum Analytics, Inc.| (default, Mar 22 2017, 19:54:23)
[GCC 4.4.7 20120313 (Red Hat 4.4.7-1)] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> import sys
>>> sys.path
['', '/home/killeent/local/miniconda2/envs/p3/lib/python36.zip', '/home/killeent/local/miniconda2/envs/p3/lib/python3.6', '/home/killeent/local/miniconda2/envs/p3/lib/python3.6/lib-dynload', '/home/killeent/.local/lib/python3.6/site-packages',
# 这里就是具体的包所在的路径
'/home/killeent/local/miniconda2/envs/p3/lib/python3.6/site-packages',   '/home/killeent/github/pytorch', '/home/killeent/local/miniconda2/envs/p3/lib/python3.6/site-packages/setuptools-27.2.0-py3.6.egg']
```

可以发现，`site-packages`出现在Python的搜索路径中。来进一步看下`torch`模块所在的路径：

```shell
>>> import torch
>>> import inspect
>>> inspect.getfile(torch)
'/home/killeent/local/miniconda2/envs/p3/lib/python3.6/site-packages/torch/__init__.py'
```

我们的`torch`库确实出现在了`site-packages`中。～当然代表安装成功了！

**说明**：Python在`sys.path`中的空字符串`''`代表的是当前工作路径，且是第一个搜索的路径。因此如果我们在pytorch目录下面运行python，就会导致我们调用”当前目录“里面的pytorch，而不是在site-package中的。

