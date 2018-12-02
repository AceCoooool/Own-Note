# torch._C Package (1)

> 这个系列翻译自：[A Tour of PyTorch Internals (Part I)](http://pytorch.org/2017/05/11/Internals.html)

这个系列主要讲解如何将PyTorch中的基本元素Tensor被Python“认可”（即能够加入到Python解释器中），从而能够在Python中直接使用。这个系列主要围绕下述四个问题展开：

1. PyTorch是如何扩展Python的解释器
2. PyTorch是如何封装那些实际定义了Tensor的属性和方法的C库（可以理解为如何封装底层TH）
3. PyTorch是如何封装与Tensor相关的那些方法的
4. PyTorch是如何将各个组件合起来构成成一个可运行的包

该系列主要分为以下几个部分：

Part I：扩展Python解释器

- torch._C新模块
- THPTensor类型

Part II：通用解析

- 通用编译 (Part 1)
- 通用编译 (Part 2)

Part III：模块对象和类型方法

- 新类型中添加方法(1)
- 新类型中添加方法 (2)

Part IV: 总结

## Part I: 扩展Python解释器

PyTorch与python交互，主要通过`torch`这个包（即`import torch`）。此处主要考虑基于C写的python扩展模块（CPython风格），称为`._C`模块（python 2中通过`init_C()`，python 3中通过`PyInit__C()`来创建`._C`扩展模块）。正是这个模块使得我们能够定义python的内置对象类型（例如`Tensor`类型）以及调用C/C++写的函数。

### ① torch._C新模块

首先，让我们来看看如何向Python中添加新模块。这部分主要参考[newtypes](https://docs.python.org/3.7/extending/newtypes.html)，你可以想成有固定的模板形式。下述通过一个例子来介绍“这种模板”：

```c
#include <Python.h>
// (1). 新数据类型
typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
} noddy_NoddyObject;
// (2). 新数据类型对应的类型对象
static PyTypeObject noddy_NoddyType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "noddy.Noddy",             /* tp_name */
    sizeof(noddy_NoddyObject), /* tp_basicsize */
    ...,                       /* 省略了一大堆 */
    Py_TPFLAGS_DEFAULT,        /* tp_flags */
    "Noddy objects",           /* tp_doc */
    ...,                       /* 省略了一大堆 */
    PyType_GenericNew,         /* tp_new */
};
// (3). 创建一个包含新module所有信息的结构
static PyModuleDef noddymodule = {
    PyModuleDef_HEAD_INIT,   // PyModuleDef_Base: 这个一般不变
    "noddy",                 // m_name: 模块名字
    "Example module that creates an extension type.", // m_doc
    -1,      // m_size: 一般设置为-1
    NULL, NULL, NULL, NULL, NULL    // m_methods, mslots, ...
};
// (4). 创建新module并返回
PyMODINIT_FUNC
PyInit_noddy(void)
{
    PyObject* m;

    if (PyType_Ready(&noddy_NoddyType) < 0)
        return NULL;

    m = PyModule_Create(&noddymodule);  // 新建module
    if (m == NULL)
        return NULL;

    Py_INCREF(&noddy_NoddyType);
    // 往module里面加新数据类型---这样才能赋予新module"生命"
    PyModule_AddObject(m, "Noddy", (PyObject *)&noddy_NoddyType); 
    return m;
}
```

说明：(1)(2)后续会进一步介绍，暂时想成放进module里面的新数据类型，(3)可以视为module的”元信息“ ，(4)是这一部分的关键，利用(1)(2)(3)这些”组件“，来产生一个module

根据上述所讲述的模板，来看看``torch._C`这个模块是如何定义的。这个`._C`模块定义在[torch/csrc/Module.cpp](https://github.com/pytorch/pytorch/blob/master/torch/csrc/Module.cpp)中。通过`init_C()`（对应python 2）或者`PyInit__C()`（对应python 3）函数来创建扩展模块。

这个模块“途经”许多不同的`__init()`函数来向模块添加不同的对象以及类型等等(比如`IntTensor`，`FloatTensor`类型等等)（其中`#define ASSERT_TRUE(cmd) if (!(cmd)) return NULL`）

```c
static PyObject* initModule() {
  HANDLE_TH_ERRORS
  THInferNumThreads();
#define ASSERT_TRUE(cmd) if (!(cmd)) return NULL
  THPUtils_addPyMethodDefs(methods, TorchMethods);
  THPUtils_addPyMethodDefs(methods, DataLoaderMethods);
  THPUtils_addPyMethodDefs(methods, torch::autograd::python_functions());
  ...;  // 省略了一堆

// 对应(3)
#if PY_MAJOR_VERSION == 2
  ASSERT_TRUE(module = Py_InitModule("torch._C", methods.data()));
#else
  static struct PyModuleDef torchmodule = {
     PyModuleDef_HEAD_INIT,
     "torch._C",
     NULL,
     -1,
     methods.data() 
  };
  ASSERT_TRUE(module = PyModule_Create(&torchmodule));  // 通过PyModuleDef创建一个PyObject* 对象
  ASSERT_TRUE(THPDoubleTensor_init(module));   // 可以理解为类似(1)(2)新类型加入模块
  ASSERT_TRUE(THPFloatTensor_init(module));
  ASSERT_TRUE(THPHalfTensor_init(module));
  ...;   // 省略了一堆
  return module;
  END_HANDLE_TH_ERRORS
}
// 对应(4)
#if PY_MAJOR_VERSION == 2
PyMODINIT_FUNC init_C()
#else
PyMODINIT_FUNC PyInit__C()
#endif
{
#if PY_MAJOR_VERSION == 2
  initModule();
#else
  return initModule();   // 这部分最终会返回一个PyObject*对象
#endif
```

> 后续的不同类型都是通过`PyModule_AddObject`加入到此module中来。几个函数详见: [PyModuleDef](https://docs.python.org/3/c-api/module.html#c.PyModuleDef)，[PyModule_Create](https://docs.python.org/3/c-api/module.html#c.PyModule_Create)，[PyModule_AddObject](https://docs.python.org/3/c-api/module.html#c.PyModule_AddObject)

### ②THPTensor类型

应该没有忘记在①中我们提到了往新模块中加入新的数据类型（模块模板中的(1)(2)）（CPython形式的数据类型其实也可以看成有固定的模板形式）。这一小节就主要讲解如何定义python能够“采用”的新类型，以及如何创建通用的`THPTensor`类型。

在Python的运行阶段，所有的Python对象均被视为`PyObject *`变量（可以理解为所有Python对象的“基础类型”(base type)）。每个Python类型均包含一个`refcount`和一个指向对象的“类型对象”(type object)的指针，这个“类型对象”决定了这个对象的各种属性。例如，这个“类型对象”包含此类型的各种方法以及要调用的C函数。

**补充**：这部分详细的内容可以参考[cpython/include/object.h](https://github.com/python/cpython/blob/master/Include/object.h) (或者[python源码-对象](http://www.wklken.me/posts/2014/08/05/python-source-object.html#pyobject_head))，下述仅仅给出其具体的定义：

```c
#define _PyObject_HEAD_EXTRA            \
    struct _object *_ob_next;           \
    struct _object *_ob_prev;

typedef struct _object {    // PyObject类
    _PyObject_HEAD_EXTRA    // 双链表结构, 和垃圾回收相关
    Py_ssize_t ob_refcnt;   // 引用计数, 跟python的内存管理机制有关(基于引用计数的垃圾回收机制)
    struct _typeobject *ob_type;  // 指向类型对象的指针
} PyObject;   
```

定义一种新类型一般可以总结为下述两点：

1. 创建一个类 (`struct`)：定义各种新对象所包含的内容
2. 对应此对象的“类型对象”（*）

创建类本身是非常简单的。Python内部，所有的浮点类型实际上是堆上的对象，python中的float类的定义如下所示(可以参考[cpyton/include/floatobject.h](https://github.com/python/cpython/blob/6f0eb93183519024cb360162bdd81b9faec97ba6/Include/floatobject.h) (或者[python源码-int](http://www.wklken.me/posts/2014/08/06/python-source-int.html)))：

```c
typedef struct {
    PyObject_HEAD    // 对应 PyObject ob_base --- 即PyObject的实例对象(上面的代码)
    double ob_fval;  
} PyFloatObject;
```

其中的`PyObject_HEAD`是一个包含对象引用计数和指向“类型对象”指针的宏(其实在python 3.7中直接就是PyObject实例的宏)。因此，从上述定义不难发现，要实现float，只有浮点数自身(`ob_fval`)需要“指明”（其实还有指向”类型“对象的指针）。

现在，让我们定义`THPTensor`类型(在[csrc/generic/Tensor.h](https://github.com/pytorch/pytorch/blob/master/torch/csrc/generic/Tensor.h)中)：

```c
struct THPTensor {
    PyObject_HEAD
    THTensor *cdata;   // 指向底层的THTensor类型
};
```

是不是非常简单？我们仅仅通过一个指针`cdata`就将底层的`TH`给封装起来了。

最重要的部分是定义一个新类型的“类型对象”。下述也先以内置类型float的“类型对象”为例(详见[Objects/floatobject.c](https://github.com/python/cpython/blob/9b6c60cbce4ac45e8ccd7934babff465e9769509/Objects/floatobject.c))：

```c
PyTypeObject PyFloat_Type = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "float",
    sizeof(PyFloatObject),                      /* tp_basicsize */
    0,
    (destructor)float_dealloc,                  /* tp_dealloc */
    ...                                         /* 省略了一堆 */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   /* tp_flags */
    float_new__doc__,                           /* tp_doc */
    0,                                          /* tp_traverse */
    0,                                          /* tp_clear */
    float_richcompare,                          /* tp_richcompare */
    0,                                          /* tp_weaklistoffset */
    0,                                          /* tp_iter */
    0,                                          /* tp_iternext */
    float_methods,                              /* tp_methods */
    0,                                          /* tp_members */
    float_getset,                               /* tp_getset */
    ...                                         /* 省略了一堆 */
    float_new,                                  /* tp_new */
};
```

理解“类型对象”最简单的方式是将视为：定义了各种对象属性所对应的“绑定对象”。例如，`tp_basicsize`绑定到`sizeof(PyFloatObject)`。这也是为什么python能够在创建新的`PyFloatObject`对象(利用`PyObject_New()`)时知道应该分配多少内存空间。关于全部能够“绑定的对象”(full list of field)详见：[object.h](https://github.com/python/cpython/blob/9b6c60cbce4ac45e8ccd7934babff465e9769509/Include/object.h)中的`typedef struct _typeobject`里的内容

而关于`THPTensor`类型的“类型对象”则为`THPTensorType`，定义在[csrc/generic/Tensor.cpp](https://github.com/pytorch/pytorch/blob/master/torch/csrc/generic/Tensor.cpp)中。这个“类型对象”中定义了`THPTensor`类型的名字，大小，映射方法等等：

```c
PyTypeObject THPTensorType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "torch._C." THPTensorBaseStr,          /* tp_name */
  sizeof(THPTensor),                     /* tp_basicsize */
  0,                                     /* tp_itemsize */
  (destructor)THPTensor_(dealloc),       /* tp_dealloc */
  ...                                    /* 省略了一堆 */
  &THPTensor_(mappingmethods),           /* tp_as_mapping */
  ...                                    /* 省略了一堆 */
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   /* tp_flags */
  NULL,                                  /* tp_doc */
  ...                                    /* 省略了一堆 */
  THPTensor_(pynew),                     /* tp_new */
};
```

例如，其中`tp_new`函数对应创建对象（与初始化不同），在python层面上对应`__new()__`方法。而C实现是静态方法：传入待实例类型和其他参数，然后返回一个新创建的对象：

```c
static PyObject * THPTensor_(pynew)(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
  HANDLE_TH_ERRORS
  Py_ssize_t num_args = args ? PyTuple_Size(args) : 0;

  THPTensorPtr self((THPTensor *)type->tp_alloc(type, 0));  // 此句是核心
  // more code below: 主要针对传入的参数args等进行一系列判断来选择对应的初始化
}
```

上述函数第一件事是分配一个`THPTensor`对象，然后根据传入的args进行一系列的初始化。例如，通过一个`THPTensor`对象y来创建一个`THPTensor`对象x时，我们通过是通过调用底层`TH`中`THTensor_(newwithTensor)`来实现的。（关于`THPTensor_(pynew)`更详细的实现见源码）

另一个在`Tensor.cpp`中非常重要的实现为下标(indexing)。PyTorch中的Tensor支持Python的映射协议 (主要和上述的`THPTensor_(mappingmethods)`相关)，这允许我们使用下述操作：

```c
x = torch.Tensor(10).fill_(1)
y = x[3] // y == 1
x[4] = 2
// etc.
```

> 说明：tensor不仅仅只有一维的下标映射，也能进行更多维操作

我们能够采用`[]`形式的符号来定义python中的三种映射方法：详见[python mapping](https://docs.python.org/3.7/c-api/typeobj.html#c.PyMappingMethods)

最重要的方法则是`THPTensor_(getValue)`和`THPTensor_(setValue)`，描述如何对Tensor进行下标操作，如何返回新的Tensor或者Scalar，或者如何更新Tensor的内容。更好的理解这部分内容建议进一步细看这两者的具体实现。

