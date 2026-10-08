#pragma once
#include "../../ml/tensor.h"
namespace ml {

struct Backend {
  void (*add)(const math::Tensor &, const math::Tensor &, math::Tensor &);
  void (*sub)(const math::Tensor &, const math::Tensor &, math::Tensor &);
  void (*mul)(const math::Tensor &, const math::Tensor &, math::Tensor &);
  void (*div)(const math::Tensor &, const math::Tensor &, math::Tensor &);
  void (*matmul)(const math::Tensor &, const math::Tensor &, math::Tensor &);
};
extern Backend cpu_backend;
}; // namespace ml
