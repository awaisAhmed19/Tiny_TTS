#pragma once
#include "Constants.h"
#include "types.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <memory>
#include <new>
#include <random>
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

[X] fill()
[X] zero()
[X] ones()

[X] randomUniform()
[X] randomNormal()

============================================================
ELEMENT-WISE ARITHMETIC
============================================================

[X] add()
[X] subtract()
[X] multiply()
[X] divide()

[X] operator+()
[X] operator-()
[X] operator*()       // scalar
[X] operator/()       // scalar

[X] operator+=()
[X] operator-=()
[X] operator*=()
[X] operator/=()

============================================================
ELEMENT-WISE MATH
============================================================

[X] abs()
[X] exp()
[X] log()
[X] sqrt()
[X] pow()

[X] sin()
[X] cos()
[X] tanh()

[X] floor()
[X] ceil()
[X] round()

[X] clamp()
[X] min()
[X] max()

============================================================
REDUCTIONS
============================================================

[X] sum()
[X] mean()

[X] minElement()
[X] maxElement()

[X] argmin()
[X] argmax()

[X] magnitudeSquared()
[X] magnitude()

[X] dot()

============================================================
NORMALIZATION
============================================================

[X] normalized()

[X] variance()
[X] standardDeviation()

[X] normalize()
[X] standardize()

============================================================
COMPARISON
============================================================

[X] isZero()
[X] isNormalized()
[X] nearEqual()

[X] operator==()
[X] operator!=()

============================================================
ML-SPECIFIC
============================================================

[X] hadamardProduct()
[] outerProduct()

[X] softmax()
[X] logSoftmax()

[X] oneHot()

============================================================
UTILITY
============================================================

[ ] map()
[ ] transform()

[X] copy()
[X] slice()
[X] concat()

[X] reverse()

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

  bool operator==(Vector &other) const {
    for (size_t i = 0; i < m_size; ++i) {
      if (mp_data[i] != other[i]) {
        return false;
      }
    }
    return true;
  }
  // [ ] operator!=()
  bool operator!=(Vector &other) const {
    for (size_t i = 0; i < m_size; ++i) {
      if (mp_data[i] != other[i]) {
        return true;
      }
    }
    return false;
  }

  bool is_zero() const {
    for (size_t i = 0; i < m_size; ++i) {
      if (mp_data[i] != 0 || mp_data[i] != 0.0f) {
        return false;
      }
    }
    return true;
  }

  bool nearEqual(const Vector &other, T epsilon) const {
    if (m_size != other.m_size)
      return false;

    for (size_t i = 0; i < m_size; ++i) {
      if (std::abs(mp_data[i] - other.mp_data[i]) > epsilon)
        return false;
    }

    return true;
  }

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
  Vector(size_t size) : m_capacity(0), m_size(0), mp_data(nullptr) {
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

  //[]Fill
  void fill(const T value) {
    for (size_t i = 0; i < m_size; ++i) {
      mp_data[i] = value;
    }
  }

  void zero() { fill(T{}); }

  void ones() { fill(T{1}); }
  void randomUniform(T a, T b) {
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(static_cast<float>(a),
                                                       static_cast<float>(b));

    for (size_t i = 0; i < m_size; ++i) {
      mp_data[i] = distribution(generator);
    }
  }

  void randomNormal(float mean, float std_dev) {
    if (std_dev <= 0)
      throw std::runtime_error("standard deviation must be positive");

    std::random_device rd{};
    std::mt19937 gen{rd()};

    std::normal_distribution<float> d{mean, std_dev};

    for (size_t n{}; n < m_size; ++n) {
      mp_data[n] = d(gen);
    }
  }

  Vector operator+(const Vector &other) {
    if (m_size != other.m_size)
      throw std::invalid_argument("vector sizes must match");
    Vector output(m_size);
    for (size_t i = 0; i < m_size; ++i) {
      output.mp_data[i] = mp_data[i] + other.mp_data[i];
    }
    return output;
  }

  Vector operator*(const Vector &other) {
    if (m_size != other.m_size)
      throw std::invalid_argument("vector sizes must match");
    Vector output(m_size);
    for (size_t i = 0; i < m_size; ++i) {
      output.mp_data[i] = mp_data[i] * other.mp_data[i];
    }
    return output;
  }

  Vector operator-(const Vector &other) {
    if (m_size != other.m_size)
      throw std::invalid_argument("vector sizes must match");
    Vector output(m_size);
    for (size_t i = 0; i < m_size; ++i) {
      output.mp_data[i] = mp_data[i] - other.mp_data[i];
    }
    return output;
  }

  Vector operator/(const Vector &other) {
    if (m_size != other.m_size)
      throw std::invalid_argument("vector sizes must match");
    Vector output(m_size);

    for (size_t i = 0; i < m_size; ++i) {
      if (other.mp_data[i] == 0)
        throw std::runtime_error("division by zero");
      output.mp_data[i] = mp_data[i] / other.mp_data[i];
    }
    return output;
  }
  Vector operator+(const T other) {
    Vector output(m_size);
    for (size_t i = 0; i < m_size; ++i) {
      output.mp_data[i] = mp_data[i] + other;
    }
    return output;
  }

  Vector operator*(const T other) {
    Vector output(m_size);
    for (size_t i = 0; i < m_size; ++i) {
      output.mp_data[i] = mp_data[i] * other;
    }
    return output;
  }

  Vector operator-(const T other) {
    Vector output(m_size);
    for (size_t i = 0; i < m_size; ++i) {
      output.mp_data[i] = mp_data[i] - other;
    }
    return output;
  }

  Vector operator/(const T other) {
    if (other == 0)
      throw std::invalid_argument("vector sizes must match");
    Vector output(m_size);
    for (size_t i = 0; i < m_size; ++i) {
      output.mp_data[i] = mp_data[i] / other;
    }
    return output;
  }
  Vector &operator+=(const Vector &other) {
    if (m_size != other.m_size)
      throw std::invalid_argument("vector sizes must match");
    for (size_t i = 0; i < m_size; ++i) {
      mp_data[i] += other.mp_data[i];
    }
    return *this;
  }

  Vector &operator*=(const Vector &other) {
    if (m_size != other.m_size)
      throw std::invalid_argument("vector sizes must match");
    for (size_t i = 0; i < m_size; ++i) {
      mp_data[i] *= other.mp_data[i];
    }
    return *this;
  }

  Vector &operator-=(const Vector &other) {
    if (m_size != other.m_size)
      throw std::invalid_argument("vector sizes must match");
    for (size_t i = 0; i < m_size; ++i) {
      mp_data[i] -= other.mp_data[i];
    }
    return *this;
  }

  Vector &operator/=(const Vector &other) {
    if (m_size != other.m_size)
      throw std::invalid_argument("vector sizes must match");

    for (size_t i = 0; i < m_size; ++i) {
      if (other.mp_data[i] == 0)
        throw std::runtime_error("division by zero");
      mp_data[i] /= other.mp_data[i];
    }
    return *this;
  }
  Vector &operator+=(const T other) {
    Vector output(m_size);
    for (size_t i = 0; i < m_size; ++i) {
      mp_data[i] += other;
    }
    return *this;
  }

  Vector &operator*=(const T other) {
    for (size_t i = 0; i < m_size; ++i) {
      mp_data[i] *= other;
    }
    return *this;
  }

  Vector &operator-=(const T other) {
    for (size_t i = 0; i < m_size; ++i) {
      mp_data[i] -= other;
    }
    return *this;
  }

  Vector &operator/=(const T other) {
    if (other == 0)
      throw std::invalid_argument("vector sizes must match");
    for (size_t i = 0; i < m_size; ++i) {
      mp_data[i] /= other;
    }
    return *this;
  }

  void abs() {
    for (size_t i = 0; i < m_size; ++i) {
      mp_data[i] = std::abs(mp_data[i]);
    }
  }
  void exp() {
    for (size_t i = 0; i < m_size; ++i) {
      mp_data[i] = std::exp(mp_data[i]);
    }
  }
  void pow(const T value) {
    for (size_t i = 0; i < m_size; ++i) {
      mp_data[i] = std::pow(mp_data[i], value);
    }
  }
  void log() {
    for (size_t i = 0; i < m_size; ++i) {
      mp_data[i] = std::log(mp_data[i]);
    }
  }
  void sqrt() {
    for (size_t i = 0; i < m_size; ++i) {
      mp_data[i] = std::sqrt(mp_data[i]);
    }
  }

  void sin() {
    for (size_t i = 0; i < m_size; ++i) {
      mp_data[i] = std::sin(mp_data[i]);
    }
  }
  void cos() {
    for (size_t i = 0; i < m_size; ++i) {
      mp_data[i] = std::cos(mp_data[i]);
    }
  }
  void tanh() {
    for (size_t i = 0; i < m_size; ++i) {
      mp_data[i] = std::tanh(mp_data[i]);
    }
  }
  void floor() {
    for (size_t i = 0; i < m_size; ++i) {
      mp_data[i] = std::floor(mp_data[i]);
    }
  }
  void ceil() {
    for (size_t i = 0; i < m_size; ++i) {
      mp_data[i] = std::ceil(mp_data[i]);
    }
  }
  void round() {
    for (size_t i = 0; i < m_size; ++i) {
      mp_data[i] = std::round(mp_data[i]);
    }
  }
  void clamp(const T min, const T max) {
    for (size_t i = 0; i < m_size; ++i) {
      mp_data[i] = std::clamp(mp_data[i], min, max);
    }
  }

  T max() const {
    if (m_size == 0)
      throw std::runtime_error("max() called on empty vector");

    T max_out = mp_data[0];

    for (size_t i = 1; i < m_size; ++i)
      max_out = std::max(max_out, mp_data[i]);

    return max_out;
  }

  T min() const {
    if (m_size == 0)
      throw std::runtime_error("min() called on empty vector");

    T min_out = mp_data[0];

    for (size_t i = 1; i < m_size; ++i)
      min_out = std::min(min_out, mp_data[i]);

    return min_out;
  }

  T sum() {
    if (m_size == 0)
      throw std::runtime_error("min() called on empty vector");
    T sum_ = 0;
    for (size_t i = 0; i < m_size; ++i)
      sum_ += mp_data[i];

    return sum_;
  }

  T mean() {
    if (m_size == 0)
      throw std::runtime_error("min() called on empty vector");
    T mean_ = 0, sum_ = 0;
    for (size_t i = 0; i < m_size; ++i)
      sum_ += mp_data[i];

    return sum_ / m_size;
  }

  T minElement() { return min(); }
  T maxElement() { return max(); }

  T argmin() { return min(); }
  T argmax() { return max(); }

  T magnitudeSquared() const {
    T result{};
    for (size_t i = 0; i < m_size; ++i)
      result += mp_data[i] * mp_data[i];

    return result;
  }

  T magnitude() const { return std::sqrt(magnitudeSquared()); }

  T dot(const Vector<T> &other) {
    if (m_size != other.m_size) {
      throw std::runtime_error("size of the vectors must be equal");
    }
    T dot_prod = 0;
    for (size_t i = 0; i < m_size; ++i) {
      dot_prod += mp_data[i] * other.mp_data[i];
    }
    return dot_prod;
  }

  Vector normalized() const {
    T mag = magnitude();
    if (mag == T{})
      return *this;
    Vector res(*this);
    for (size_t i = 0; i < m_size; ++i) {
      res.mp_data[i] /= mag;
    }
    return res;
  }

  T variance() {
    T mu = mean();
    T n = static_cast<T>(m_size);
    T sum = 0, _sum = 0;

    for (size_t i = 0; i < m_size; ++i) {
      _sum = mp_data[i] - mu;
      sum += _sum * _sum;
    }

    return sum / n;
  }

  T std_dev() { return std::sqrt(variance()); }

  void normalize() {
    T mag = magnitude();

    if (mag == T{})
      return;

    for (size_t i = 0; i < m_size; ++i)
      mp_data[i] /= mag;
  }

  void standardize() {
    T mu = mean();
    T sd = std_dev();

    if (sd == T{})
      return;

    for (size_t i = 0; i < m_size; ++i)
      mp_data[i] = (mp_data[i] - mu) / sd;
  }

  Vector hadamard_product(const Vector &other) const {
    if (m_size != other.m_size)
      throw std::runtime_error("size mismatch");

    Vector res(*this);

    for (size_t i = 0; i < m_size; ++i)
      res[i] = mp_data[i] * other[i];

    return res;
  }

  Vector softmax() const {
    T max_value = max();
    T denom = T{};

    for (size_t i = 0; i < m_size; ++i)
      denom += std::exp(mp_data[i] - max_value);

    Vector res(*this);

    for (size_t i = 0; i < m_size; ++i)
      res[i] = std::exp(mp_data[i] - max_value) / denom;

    return res;
  }

  Vector logsoftmax() const {
    T max_value = max();
    T sum = T{};

    for (size_t i = 0; i < m_size; ++i)
      sum += std::exp(mp_data[i] - max_value);

    T log_sum_exp = std::log(sum);

    Vector res(*this);

    for (size_t i = 0; i < m_size; ++i)
      res[i] = (mp_data[i] - max_value) - log_sum_exp;

    return res;
  }

  static Vector one_hot(const size_t size, const size_t k) {
    Vector res(size);

    for (size_t i = 0; i < size; ++i) {
      if (i == k) {
        res[i] = 1;
      } else {
        res[i] = 0;
      }
    }
    return res;
  }

  Vector concat(Vector &other) {
    size_t new_size = m_size + other.m_size;
    Vector res(new_size);
    for (size_t i = 0; i < m_size; ++i) {
      res[i] = mp_data[i];
    }
    for (size_t i = m_size; i < new_size; ++i) {
      res[i] = other.mp_data[i];
    }
    return res;
  }
  Vector reverse() {
    Vector res(*this);
    for (size_t i = 0; i < m_size; ++i) {
      res[i] = mp_data[m_size - i - 1];
    }
    return res;
  }
  Vector copy() {
    Vector res(*this);
    return res;
  }
};
}; // namespace math
}; // namespace engine
