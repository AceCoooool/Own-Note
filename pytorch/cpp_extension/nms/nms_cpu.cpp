#include "nms_cpu.h"

/*
 * 1. 下面大部分操作都可以查看python版本的文档: https://pytorch.org/docs/master/tensors.html
 * 2. dets: Nx4;  scores: Nx1
 * 3. 返回的是 K (被选择的框框的下标)
 * */
template<typename scalar_t>
at::Tensor nms_cpu_kernel(const at::Tensor &dets, const at::Tensor &scores, const float threshold) {
    // 检查是否为cpu tensor类型
    AT_ASSERTM(!dets.type().is_cuda(), "dets must be a CPU tensor");
    AT_ASSERTM(!scores.type().is_cuda(), "scores must be a CPU tensor");
    AT_ASSERTM(dets.type() == scores.type(), "dets should have the same type as scores");
    // 如果dets为空的情况
    if (dets.numel() == 0) {
        return at::empty({0}, dets.options().dtype(at::kLong).device(at::kCPU));
    }

    auto x1_t = dets.select(1, 0).contiguous();  // select(dim, index)
    auto y1_t = dets.select(1, 1).contiguous();
    auto x2_t = dets.select(1, 2).contiguous();
    auto y2_t = dets.select(1, 3).contiguous();

    // 计算面积：N
    at::Tensor areas_t = (x2_t - x1_t + 1) * (y2_t - y1_t + 1);
    // 根据score降序排序: sort(dim, descending=False) -> (Tensor, LongTensor) 返回的是tuple类型
    auto order_t = std::get<1>(scores.sort(0, /* descending=*/true));

    // suppressed_t: 标记, 代表该对象是否"被抑制"(即该box不再取了)
    auto ndets = dets.size(0);
    at::Tensor suppressed_t = at::zeros({ndets}, dets.options().dtype(at::kByte).device(at::kCPU));

    // 返回指向这些Variable空间的指针
    auto suppressed = suppressed_t.data<uint8_t>();
    auto order = order_t.data<int64_t>();
    auto x1 = x1_t.data<scalar_t>();
    auto y1 = y1_t.data<scalar_t>();
    auto x2 = x2_t.data<scalar_t>();
    auto y2 = y2_t.data<scalar_t>();
    auto areas = areas_t.data<scalar_t>();

    // 执行nms算法
    for (int64_t _i = 0; _i < ndets; _i++) {
        auto i = order[_i];       // 被选中的框框
        if (suppressed[i] == 1)
            continue;
        auto ix1 = x1[i];
        auto iy1 = y1[i];
        auto ix2 = x2[i];
        auto iy2 = y2[i];
        auto iarea = areas[i];

        // 将IoU大于threshold的所有框框抑制掉
        for (int64_t _j = _i + 1; _j < ndets; _j++) {
            auto j = order[_j];
            if (suppressed[j] == 1)
                continue;
            auto xx1 = std::max(ix1, x1[j]);
            auto yy1 = std::max(iy1, y1[j]);
            auto xx2 = std::min(ix2, x2[j]);
            auto yy2 = std::min(iy2, y2[j]);

            auto w = std::max(static_cast<scalar_t>(0), xx2 - xx1 + 1);
            auto h = std::max(static_cast<scalar_t>(0), yy2 - yy1 + 1);
            auto inter = w * h;
            auto ovr = inter / (iarea + areas[j] - inter);
            if (ovr >= threshold)
                suppressed[j] = 1;
        }
    }
    // 返回suppressed_t没被抑制对应的位置 K (squeeze(1)主要是因为nonzero返回的是Kx1)
    return at::nonzero(suppressed_t == 0).squeeze(1);
}

at::Tensor nms_cpu(const at::Tensor &dets,
                   const at::Tensor &scores,
                   const float threshold) {
    at::Tensor result;
    // 自动类型调度(即将scalar_t指定为dets的类型, 并检查是否为float或double类型) --- 反正写的时候照抄吧
    // 具体宏见: https://github.com/pytorch/pytorch/blob/master/aten/src/ATen/Dispatch.h
    AT_DISPATCH_FLOATING_TYPES(dets.type(), "nms", [&] {
        result = nms_cpu_kernel<scalar_t>(dets, scores, threshold);
    });
    return result;
}