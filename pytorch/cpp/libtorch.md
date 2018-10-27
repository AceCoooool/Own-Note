# 这里主要记录用的时候一些"注意事项"

1. `<torch/torch.h>` v.s. `<ATen/ATen.h>`：命名空间`at`包含在命名空间`torch`下面。但是采用两种方式定义的变量是有区别的

   ```cpp
   at::Tensor tensor1 = at::ones({2});   // 对应的是CPUFloatType{2}类型
   torch::Tensor tensor2 = torch::ones({2});  // 对应的是Variable[CPUFloatType]{4}类型
   ```

   - `tensor2`对应的是`Variable`（具备梯度存储空间），而`tensor1`对应的是`THTensor`类型。所以如果自己定义一些操作时，请用`at::Tensor`而不是用`torch::Tensor`。（这点在自己写底层的前向与反向传播时尤其注意）。

   - 而采用`torch::Tensor`的情况类似作者给出的例子：

     ```python
     # Python
     import torch

     model = torch.nn.Linear(5, 1)
     optimizer = torch.optim.SGD(model.parameters(), lr=0.1)
     prediction = model.forward(torch.randn(3, 5))
     loss = torch.nn.functional.mse_loss(prediction, torch.ones(3, 1))
     loss.backward()
     optimizer.step()
     ```

     等价的CPP方式

     ```cpp
     // C++
     #include <torch/torch.h>

     torch::nn::Linear model(5, 1);
     torch::optim::SGD optimizer(model->parameters(), /*lr=*/0.1);
     torch::Tensor prediction = model->forward(torch::randn({3, 5}));
     auto loss = torch::mse_loss(prediction, torch::ones({3, 1}));
     loss.backward();
     optimizer.step();
     ```

2. 在cuda程序（`.cu`）中，存储在`gpu`上面的数据是不允许使用`cout`来输出查看的，所以请使用`printf`进行查看。

   > 原因请看：[Using thrust with printf / cout](https://stackoverflow.com/questions/36855469/using-thrust-with-printf-cout) --- 说白了就是NVIDIA并没有实现c++的I/O

3. 目前存在的一个bug，采用cmake3.12会在运行的时候出错（而CLion18.02自带的就是cmake3.12）。这个bug真算是神坑

