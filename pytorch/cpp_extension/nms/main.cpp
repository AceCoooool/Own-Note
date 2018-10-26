#include <torch/torch.h>
#include "nms_cpu.h"
#include <iostream>

using namespace std;

int main() {
    at::Tensor boxes = at::tensor({1.0, 1.0, 3.0, 3.0, 2.0, 2.0, 3.0, 3.0, 1.5, 1.5, 3.5, 3.5}).reshape({3, 4});
    at::Tensor scores = at::tensor({0.99, 0.6, 0.8}).reshape({3, 1});
    float threshold = 0.5;
    std::cout << nms_cpu(boxes, scores, threshold) << std::endl;
}