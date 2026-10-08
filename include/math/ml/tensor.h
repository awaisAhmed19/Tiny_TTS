#pragma once
#include <stdint.h>
// ============================================================
// TENSOR
// ============================================================
//
// Construction
// [ ] zeros()
// [ ] ones()
// [ ] full()
// [ ] fromVector()
//
// Properties
// [ ] shape()
// [ ] rank()
// [ ] size()
// [ ] data()
// [ ] empty()
//
// Indexing
// [ ] at()
// [ ] operator[]()
//
// Shape manipulation
// [ ] reshape()
// [ ] flatten()
// [ ] squeeze()
// [ ] unsqueeze()
// [ ] transpose()
// [ ] permute()
//
// Element-wise operations
// [ ] add()
// [ ] subtract()
// [ ] multiply()
// [ ] divide()
// [ ] hadamardProduct()
//
// Scalar operations
// [ ] addScalar()
// [ ] subtractScalar()
// [ ] multiplyScalar()
// [ ] divideScalar()
//
// Reductions
// [ ] sum()
// [ ] mean()
// [ ] variance()
// [ ] standardDeviation()
// [ ] min()
// [ ] max()
// [ ] argmin()
// [ ] argmax()
//
// Broadcasting
// [ ] broadcastTo()
// [ ] canBroadcast()
//
// Linear algebra
// [ ] matmul()
// [ ] dot()
// [ ] outerProduct()
//
// Numerical functions
// [ ] abs()
// [ ] exp()
// [ ] log()
// [ ] sqrt()
// [ ] pow()
// [ ] clamp()
//
// Neural-network operations
// [ ] softmax()
// [ ] logSoftmax()
//
// Memory
// [ ] clone()
// [ ] fill()
// [ ] contiguous()
//
// Comparison
// [ ] isZero()
// [ ] nearEqual()
//
// Debugging
// [ ] print()
#include "../src/core/buffer.h"
inline constexpr size_t MAX_DIM = 8;
namespace math {

struct Tensor {
  Buffer *buffer;
  Datatype dtype;
  size_t ndim;
  size_t shape[MAX_DIM];
  size_t stride[MAX_DIM];
  size_t numel;
  size_t offset;
};

}; // namespace math
