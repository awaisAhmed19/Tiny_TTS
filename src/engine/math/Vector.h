#pragma once
#include "Constants.h"
#include "types.h"
#include <cmath>
#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <memory>
#include <new>
#include <utility>
namespace engine {

namespace math {
/*
============================================================
VECTOR CORE
============================================================

[x] size()
[X] resize()
[X] reserve()
[X] capacity()
[X] empty()

[X] data()
[X] operator[]()
[X] at()

[X] begin()
[X] end()
[X] cbegin()
[X] cend()

============================================================
CONSTRUCTION / OWNERSHIP
============================================================

[X] default constructor
[X] size constructor
[X] value constructor
[X] initializer-list constructor

[X] copy constructor
[X] move constructor
[X] copy assignment
[X] move assignment

============================================================
FILL / INITIALIZATION
============================================================

[ ] fill()
[ ] zero()
[ ] ones()

[ ] randomUniform()
[ ] randomNormal()

============================================================
ELEMENT-WISE ARITHMETIC
============================================================

[ ] add()
[ ] subtract()
[ ] multiply()
[ ] divide()

[ ] operator+()
[ ] operator-()
[ ] operator*()       // scalar
[ ] operator/()       // scalar

[ ] operator+=()
[ ] operator-=()
[ ] operator*=()
[ ] operator/=()

============================================================
ELEMENT-WISE MATH
============================================================

[ ] abs()
[ ] exp()
[ ] log()
[ ] sqrt()
[ ] pow()

[ ] sin()
[ ] cos()
[ ] tanh()

[ ] floor()
[ ] ceil()
[ ] round()

[ ] clamp()
[ ] min()
[ ] max()

============================================================
REDUCTIONS
============================================================

[ ] sum()
[ ] mean()

[ ] minElement()
[ ] maxElement()

[ ] argmin()
[ ] argmax()

[ ] magnitudeSquared()
[ ] magnitude()

[ ] dot()

============================================================
NORMALIZATION
============================================================

[ ] normalized()

[ ] variance()
[ ] standardDeviation()

[ ] normalize()
[ ] standardize()

============================================================
COMPARISON
============================================================

[ ] isZero()
[ ] isNormalized()
[ ] nearEqual()

[ ] operator==()
[ ] operator!=()

============================================================
ML-SPECIFIC
============================================================

[ ] hadamardProduct()
[ ] outerProduct()

[ ] softmax()
[ ] logSoftmax()

[ ] oneHot()

============================================================
UTILITY
============================================================

[ ] map()
[ ] transform()

[ ] copy()
[ ] slice()
[ ] concat()

[ ] reverse()

============================================================
RANDOM / INITIALIZATION
============================================================

[ ] randomUniform()
[ ] randomNormal()
[ ] randomXavier()
[ ] randomHe()
*/
template <typename T> struct Vector {
  size_t m_capacity;
  size_t m_size;

  T *mp_data;
  ~Vector() {
    for (size_t i = 0; i < m_size; ++i) {
      (mp_data + i)->~T();
    }

    ::operator delete(mp_data);
  }
  Vector() : m_capacity(0), m_size(0), mp_data(nullptr) {}
  // ============================================================
  // VECTOR CORE
  // ============================================================
  //
  // [x] size()
  size_t size() const { return m_size; }
  // [ ] resize()
  void resize(size_t new_size) {
    if (new_size < m_size) {
      // Destroy elements being removed
      for (size_t i = new_size; i < m_size; ++i) {
        (mp_data + i)->~T();
      }
      m_size = new_size;
      return;
    }
    if (new_size > m_capacity) {
      reserve(new_size);
    }
    for (size_t i = m_size; i < new_size; ++i) {
      new (mp_data + i) T();
    }

    m_size = new_size;
  }
  void resize(size_t new_size, const T &value) {
    if (new_size < m_size) {
      for (size_t i = new_size; i < m_size; ++i) {
        (mp_data + i)->~T();
      }

      m_size = new_size;
      return;
    }

    if (new_size > m_capacity) {
      reserve(new_size);
    }
    for (size_t i = m_size; i < new_size; ++i) {
      new (mp_data + i) T(value);
    }
    m_size = new_size;
  }
  // [ ] reserve()

  void reserve(size_t new_capacity) {
    if (new_capacity <= m_capacity) {
      return;
    }

    T *new_data = static_cast<T *>(::operator new(new_capacity * sizeof(T)));

    size_t constructed = 0;

    try {
      for (; constructed < m_size; ++constructed) {
        new (new_data + constructed)
            T(std::move_if_noexcept(mp_data[constructed]));
      }
    } catch (...) {
      for (size_t i = 0; i < constructed; ++i) {
        (new_data + i)->~T();
      }

      ::operator delete(new_data);
      throw;
    }

    // Destroy old objects
    for (size_t i = 0; i < m_size; ++i) {
      (mp_data + i)->~T();
    }

    ::operator delete(mp_data);

    mp_data = new_data;
    m_capacity = new_capacity;
  }
  // [ ] capacity()
  size_t capacity() const { return m_capacity; }
  // [ ] empty()
  bool empty() const { return m_size == 0; }
  //
  // [ ] data()
  T *data() const { return mp_data; }
  // [ ] operator[]()
  T &operator[](size_t i) { return mp_data[i]; }

  const T &operator[](size_t i) const { return mp_data[i]; }
  // [ ] at()
  T &at(size_t i) {
    if (i >= m_size) {
      throw std::out_of_range("Vector::at()");
    }

    return mp_data[i];
  }
  const T &at(size_t i) const {
    if (i >= m_size) {
      throw std::out_of_range("Vector::at()");
    }

    return mp_data[i];
  }
  // [ ] begin()
  T *begin() { return mp_data; }
  // [ ] end()
  T *end() { return mp_data + m_size; }
  // [ ] cbegin()
  const T *cbegin() const { return mp_data; }
  // [ ] cend()
  const T *cend() const { return mp_data + m_size; }

  // ============================================================
  // CONSTRUCTION / OWNERSHIP
  // ============================================================
  //
  // [ ] size constructor
  Vector(const size_t size) : m_capacity(size), m_size(size), mp_data(nullptr) {
    resize(size);
  }
  // [ ] value constructor
  Vector(const size_t size, const T &value)
      : m_capacity(size), m_size(size), mp_data(nullptr) {
    resize(size, value);
  }
  // [ ] initializer-list constructor
  Vector(std::initializer_list<T> values)
      : m_capacity(values.size()), m_size(values.size()), mp_data(nullptr) {
    if (m_size == 0)
      return;

    mp_data = static_cast<T *>(::operator new(m_size * sizeof(T)));

    size_t constructed = 0;

    try {
      for (const T &value : values) {
        new (mp_data + constructed) T(value);
        ++constructed;
      }
    } catch (...) {
      for (size_t i = 0; i < constructed; ++i) {
        (mp_data + i)->~T();
      }

      ::operator delete(mp_data);
      throw;
    }
  }
  // [ ] copy constructor
  Vector(const Vector &other)
      : m_capacity(other.m_size), m_size(other.m_size), mp_data(nullptr) {
    if (m_size == 0)
      return;

    mp_data = static_cast<T *>(::operator new(m_capacity * sizeof(T)));

    size_t constructed = 0;

    try {
      for (; constructed < m_size; ++constructed) {
        new (mp_data + constructed) T(other.mp_data[constructed]);
      }
    } catch (...) {
      for (size_t i = 0; i < constructed; ++i) {
        (mp_data + i)->~T();
      }

      ::operator delete(mp_data);
      throw;
    }
  }
  // [ ] move constructor
  Vector(Vector &&other) noexcept
      : m_capacity(other.m_capacity), m_size(other.m_size),
        mp_data(other.mp_data) {
    other.m_capacity = 0;
    other.m_size = 0;
    other.mp_data = nullptr;
  }
  void swap(Vector &other) noexcept {
    std::swap(m_capacity, other.m_capacity);
    std::swap(m_size, other.m_size);
    std::swap(mp_data, other.mp_data);
  }
  // [ ] copy assignment
  Vector &operator=(const Vector &other) {
    if (this == &other)
      return *this;

    Vector temp(other);
    swap(temp);
    return *this;
  }
  // [ ] move assignment
  Vector &operator=(Vector &&other) noexcept {
    if (this == &other)
      return *this;

    // Destroy our current objects
    for (size_t i = 0; i < m_size; ++i) {
      (mp_data + i)->~T();
    }

    ::operator delete(mp_data);

    // Steal other's resources
    mp_data = other.mp_data;
    m_size = other.m_size;
    m_capacity = other.m_capacity;

    // Leave other valid
    other.mp_data = nullptr;
    other.m_size = 0;
    other.m_capacity = 0;

    return *this;
  }
  void clear() {
    for (size_t i = 0; i < m_size; ++i) {
      (mp_data + i)->~T();
    }

    m_size = 0;
  }

  void pop_back() {
    if (m_size == 0) {
      return;
    }

    --m_size;
    (mp_data + m_size)->~T();
  }

  void push_back(const T &value) {
    if (m_size == m_capacity) {
      reserve(m_capacity == 0 ? 1 : m_capacity * 2);
    }

    new (mp_data + m_size) T(value);
    ++m_size;
  }
  void push_back(T &&value) {
    if (m_size == m_capacity) {
      reserve(m_capacity == 0 ? 1 : m_capacity * 2);
    }

    new (mp_data + m_size) T(std::move(value));
    ++m_size;
  }
  template <typename... Args> T &emplace_back(Args &&...args) {
    if (m_size == m_capacity) {
      reserve(m_capacity == 0 ? 1 : m_capacity * 2);
    }

    T *element = mp_data + m_size;

    new (element) T(std::forward<Args>(args)...);

    ++m_size;

    return *element;
  }
};
}; // namespace math
}; // namespace engine
