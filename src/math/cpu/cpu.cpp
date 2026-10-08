#include <math/src/runtime/backend.h>
namespace ml {
void cpu_add(const math::Tensor &a, const math::Tensor &b, math::Tensor &out);
void cpu_sub(const math::Tensor &a, const math::Tensor &b, math::Tensor &out);
void cpu_mul(const math::Tensor &a, const math::Tensor &b, math::Tensor &out);
void cpu_div(const math::Tensor &a, const math::Tensor &b, math::Tensor &out);
void cpu_matmul(const math::Tensor &a, const math::Tensor &b,
                math::Tensor &out);
Backend cpu_backend = {cpu_add, cpu_sub, cpu_mul, cpu_div,
                       cpu_matmul}; // namespace ml
}; // namespace ml
