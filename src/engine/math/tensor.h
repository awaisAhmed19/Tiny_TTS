#pragma once
#include "../core/device.h"
#include "shape.h"
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
namespace engine {
namespace math {
using namespace tinytts;
template <typename T> struct Tensor {
public:
  using value_type = T;

  Tensor() = default;

  Tensor(Shape shape, Device device = Device::CPU);

  ~Tensor();

  Tensor(const Tensor &) = delete;
  Tensor &operator=(const Tensor &) = delete;

  Tensor(Tensor &&) noexcept;
  Tensor &operator=(Tensor &&) noexcept;

  T *data() noexcept;
  const T *data() const noexcept;

  const Shape &shape() const noexcept;

  size_t size() const noexcept;

  Device device() const noexcept;

private:
  T *m_data = nullptr;
  Shape m_shape;
  Device m_device = Device::CPU;
};
}; // namespace math
}; // namespace engine
