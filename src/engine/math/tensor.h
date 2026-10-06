#pragma once
#include "../core/device.h"
#include "shape.h"
#include <stdint.h>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <vector>
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

template <typename T> struct tensor {
  T *m_data;
  using dtype = T;

  std::vector<int16_t> m_shape;
  std::vector<int16_t> m_stride;

  Device device;

  static constexpr bool check_valid_type() {
    return std::is_floating_point_v<T> ||
           std::is_same_v<T, int>; //||
                                   // TODO:  std::is_same_v<T, complex>;
  }
};
}; // namespace math
}; // namespace engine
