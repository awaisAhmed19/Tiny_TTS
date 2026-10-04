#pragma once

#include <cstddef>
#include <initializer_list>
#include <vector>

namespace tinytts {

class Shape {
public:
  Shape() = default;

  Shape(std::initializer_list<size_t> dimensions) : m_dims(dimensions) {}

  size_t ndim() const noexcept { return m_dims.size(); }

  size_t operator[](size_t index) const { return m_dims[index]; }

  size_t size() const noexcept {
    size_t result = 1;

    for (size_t dim : m_dims)
      result *= dim;

    return result;
  }

private:
  std::vector<size_t> m_dims;
};

} // namespace tinytts
