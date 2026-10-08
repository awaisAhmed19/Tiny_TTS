#include <math/src/runtime/backend.h>
namespace ml {
void cuda_add(const math::Tensor &a, const math::Tensor &b, math::Tensor &out);
void cuda_sub(const math::Tensor &a, const math::Tensor &b, math::Tensor &out);
void cuda_mul(const math::Tensor &a, const math::Tensor &b, math::Tensor &out);
void cuda_div(const math::Tensor &a, const math::Tensor &b, math::Tensor &out);
void cuda_matmul(const math::Tensor &a, const math::Tensor &b,
                 math::Tensor &out);
Backend cuda_backend = {cuda_add, cuda_sub, cuda_mul, cuda_div,
                        cuda_matmul}; // namespace ml
}; // namespace ml
