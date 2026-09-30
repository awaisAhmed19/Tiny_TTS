#pragma once
#include "Vector.h"
#include "types.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <vector>

// ============================================================
// MATRIX
// ============================================================
//
// // Construction
// [X] identity()
// [X] zero()
// [X] diagonal()
//
// Properties
// [X] rows()
// [X] cols()
// [X] size()
//
// Element access
// [X] at()
// [X] operator()
//
// Arithmetic
// [X] operator+()
// [X] operator-()
// [X] operator*()
// [X] operator/
//
// Matrix operations
// [X] transpose()
// [X] determinant()
// [X] inverse()
//
// Matrix properties
// [X] is_square()
// [X] is_symmetric()
// [X] is_identity()
// [X] is_zero()
//
//  Comparison
// [X] nearEqual()
//
//   Linear algebra
// [ ] trace()
// [ ] rank()
// [X] cofactor()
// [X] adjugate()
//
//   Decompositions
// [ ] LU()
// [ ] QR()
// [ ] SVD()
// [ ] eigenvalues()
// [ ] eigenvectors()
//
// /  Vector operations
// [ ] multiplyVector()
//
// /  Utility
// [ ] row()
// [ ] column()
namespace engine {
namespace math {
template <typename T> struct Matrix {
  size_t m_rows{};
  size_t m_cols{};
  std::vector<T> m_data;

  Matrix() = default;

  Matrix(size_t rows, size_t cols)
      : m_rows(rows), m_cols(cols), m_data(rows * cols) {}

  Matrix(size_t rows, size_t cols, const std::vector<T> &data)
      : m_rows(rows), m_cols(cols), m_data(data) {

    if (data.size() != rows * cols)
      throw std::invalid_argument("matrix data size does not match dimensions");
  }

  Matrix(const Matrix &) = default;
  Matrix(Matrix &&) noexcept = default;

  Matrix &operator=(const Matrix &) = default;
  Matrix &operator=(Matrix &&) noexcept = default;

  size_t rows() const { return m_rows; }

  size_t cols() const { return m_cols; }

  size_t size() const { return m_data.size(); }

  T *data() { return m_data.data(); }

  const T *data() const { return m_data.data(); }

  void zero() { std::fill(m_data.begin(), m_data.end(), T{}); }

  void identity() {
    if (m_rows != m_cols)
      throw std::invalid_argument("identity() requires a square matrix");

    zero();

    for (size_t i = 0; i < m_rows; ++i)
      m_data[i * m_cols + i] = T{1};
  }

  void diagonal(T value) {
    if (m_rows != m_cols)
      throw std::invalid_argument("diagonal() requires a square matrix");

    zero();

    for (size_t i = 0; i < m_rows; ++i)
      m_data[i * m_cols + i] = value;
  }

  T &at(size_t row, size_t col) {
    if (row >= m_rows || col >= m_cols)
      throw std::out_of_range("Matrix::at()");

    return m_data[row * m_cols + col];
  }

  const T &at(size_t row, size_t col) const {
    if (row >= m_rows || col >= m_cols)
      throw std::out_of_range("Matrix::at()");

    return m_data[row * m_cols + col];
  }

  T &operator()(size_t row, size_t col) { return m_data[row * m_cols + col]; }

  const T &operator()(size_t row, size_t col) const {
    return m_data[row * m_cols + col];
  }

  T *operator[](size_t row) { return m_data.data() + row * m_cols; }

  const T *operator[](size_t row) const { return m_data.data() + row * m_cols; }

  Matrix operator+(T value) const {
    Matrix res(*this);

    for (size_t i = 0; i < m_data.size(); ++i)
      res.m_data[i] = m_data[i] + value;

    return res;
  }
  Matrix operator-(T value) const {
    Matrix res(*this);

    for (size_t i = 0; i < m_data.size(); ++i)
      res.m_data[i] = m_data[i] - value;

    return res;
  }
  Matrix operator*(const T value) const {
    Matrix res(*this);

    for (size_t i = 0; i < m_data.size(); ++i)
      res.m_data[i] = m_data[i] * value;

    return res;
  }
  Matrix operator/(const T value) const {
    if (value == T{}) {
      throw std::invalid_argument("division by zero");
    }
    Matrix res(*this);

    for (size_t i = 0; i < m_data.size(); ++i)
      res.m_data[i] = m_data[i] / value;

    return res;
  }

  Matrix &operator+=(T value) {
    for (auto &x : m_data)
      x += value;

    return *this;
  }
  Matrix &operator-=(T value) {
    for (auto &x : m_data)
      x -= value;

    return *this;
  }

  Matrix &operator*=(T value) {
    for (auto &x : m_data)
      x *= value;

    return *this;
  }

  Matrix &operator/=(T value) {
    if (value == T{})
      throw std::invalid_argument("division by zero");

    for (auto &x : m_data)
      x /= value;

    return *this;
  }
  Matrix operator+(const Matrix &other) const {
    if (m_rows != other.m_rows || m_cols != other.m_cols)
      throw std::invalid_argument("matrix dimensions do not match");

    Matrix res(*this);

    for (size_t i = 0; i < m_data.size(); ++i)
      res.m_data[i] += other.m_data[i];

    return res;
  }
  Matrix operator-(const Matrix &other) const {
    if (m_rows != other.m_rows || m_cols != other.m_cols)
      throw std::invalid_argument("matrix dimensions do not match");

    Matrix res(*this);

    for (size_t i = 0; i < m_data.size(); ++i)
      res.m_data[i] -= other.m_data[i];

    return res;
  }
  Matrix operator*(const Matrix &other) const {
    if (m_cols != other.m_rows)
      throw std::invalid_argument(
          "matrix dimensions are incompatible for multiplication");

    Matrix res(m_rows, other.m_cols);

    for (size_t i = 0; i < m_rows; ++i) {
      for (size_t k = 0; k < m_cols; ++k) {
        const T a = (*this)(i, k);

        for (size_t j = 0; j < other.m_cols; ++j) {
          res(i, j) += a * other(k, j);
        }
      }
    }

    return res;
  }

  bool is_square() const { return m_rows == m_cols; }
  T determinant() const {
    if (!is_square())
      throw std::invalid_argument("determinant requires a square matrix");

    if (m_rows == 0)
      throw std::invalid_argument("determinant of empty matrix is undefined");

    return determinantImpl(*this);
  }

  Matrix transpose() const {
    Matrix res(m_cols, m_rows);

    for (size_t i = 0; i < m_rows; ++i)
      for (size_t j = 0; j < m_cols; ++j)
        res(j, i) = (*this)(i, j);

    return res;
  }

  bool is_identity() const {
    if (!is_square())
      return false;

    for (size_t i = 0; i < m_rows; ++i) {
      for (size_t j = 0; j < m_cols; ++j) {
        const T expected = (i == j) ? static_cast<T>(1) : static_cast<T>(0);

        if ((*this)(i, j) != expected)
          return false;
      }
    }

    return true;
  }
  bool is_symmetric() const {
    if (!is_square())
      return false;

    for (size_t i = 0; i < m_rows; ++i) {
      for (size_t j = i + 1; j < m_cols; ++j) {
        if ((*this)(i, j) != (*this)(j, i))
          return false;
      }
    }

    return true;
  }
  bool is_zero() const {
    for (const T &value : m_data) {
      if (value != static_cast<T>(0))
        return false;
    }

    return true;
  }
  bool nearEqual(const Matrix &other,
                 T absTol = std::numeric_limits<T>::epsilon() * 16,
                 T relTol = std::numeric_limits<T>::epsilon() * 16) const {
    static_assert(std::is_floating_point_v<T>,
                  "nearEqual() requires a floating-point element type");

    if (m_rows != other.m_rows || m_cols != other.m_cols)
      return false;

    for (size_t i = 0; i < m_data.size(); ++i) {
      const T a = m_data[i];
      const T b = other.m_data[i];

      const T diff = std::abs(a - b);
      const T scale = std::max(std::abs(a), std::abs(b));

      // Negated <= so NaN never counts as "near".
      if (!(diff <= absTol + relTol * scale))
        return false;
    }

    return true;
  }
  std::optional<Matrix> tryInverse() const {
    static_assert(std::is_floating_point_v<T>,
                  "inverse() requires a floating-point element type");

    if (!is_square())
      throw std::invalid_argument("inverse requires a square matrix");

    if (m_rows == 0)
      throw std::invalid_argument("inverse of empty matrix is undefined");

    const size_t n = m_rows;

    Matrix a(*this); // working copy, reduced to identity
    Matrix inv(n, n);
    inv.identity(); // ends up as A^-1

    // Singularity tolerance scaled to the magnitude of the input.
    T maxAbs = T{};
    for (const T &x : m_data)
      maxAbs = std::max(maxAbs, std::abs(x));

    const T tol =
        std::numeric_limits<T>::epsilon() * static_cast<T>(n) * maxAbs;

    for (size_t col = 0; col < n; ++col) {
      // Pick the row with the largest |entry| in this column.
      size_t pivot = col;
      T best = std::abs(a(col, col));

      for (size_t r = col + 1; r < n; ++r) {
        const T v = std::abs(a(r, col));
        if (v > best) {
          best = v;
          pivot = r;
        }
      }

      if (best <= tol)
        return std::nullopt;

      a.swapRows(pivot, col);
      inv.swapRows(pivot, col);

      // Normalise the pivot row.
      const T invPivot = T{1} / a(col, col);
      for (size_t j = 0; j < n; ++j) {
        a(col, j) *= invPivot;
        inv(col, j) *= invPivot;
      }

      // Eliminate this column from every other row.
      for (size_t r = 0; r < n; ++r) {
        if (r == col)
          continue;

        const T f = a(r, col);
        if (f == T{})
          continue;

        for (size_t j = 0; j < n; ++j) {
          a(r, j) -= f * a(col, j);
          inv(r, j) -= f * inv(col, j);
        }
      }
    }

    return inv;
  }

  // Throwing convenience wrapper.
  Matrix inverse() const {
    if (auto r = tryInverse())
      return std::move(*r);

    throw std::domain_error("matrix is singular and cannot be inverted");
  }
  Vector<T> multiplyVector(const Vector<T> &v) const {
    if (v.size() != m_cols)
      throw std::invalid_argument("matrix columns must match vector size");

    Vector<T> result(m_rows);

    for (size_t i = 0; i < m_rows; ++i) {
      T sum = static_cast<T>(0);

      for (size_t j = 0; j < m_cols; ++j) {
        sum += (*this)[i][j] * v[j];
      }

      result[i] = sum;
    }

    return result;
  }

private:
  T determinantImpl(const Matrix &m) const {
    const size_t n = m.rows();

    if (n == 1)
      return m[0][0];

    if (n == 2)
      return m[0][0] * m[1][1] - m[0][1] * m[1][0];

    T det = static_cast<T>(0);

    for (size_t col = 0; col < n; ++col) {
      Matrix minor(n - 1, n - 1);

      for (size_t i = 1; i < n; ++i) {
        size_t minor_col = 0;

        for (size_t j = 0; j < n; ++j) {
          if (j == col)
            continue;

          minor[i - 1][minor_col++] = m[i][j];
        }
      }

      const T sign = (col % 2 == 0) ? static_cast<T>(1) : static_cast<T>(-1);

      det += sign * m[0][col] * determinantImpl(minor);
    }

    return det;
  }

  void swapRows(size_t r1, size_t r2) {
    if (r1 == r2)
      return;

    const auto cols = static_cast<std::ptrdiff_t>(m_cols);
    auto first1 = m_data.begin() + static_cast<std::ptrdiff_t>(r1) * cols;
    auto first2 = m_data.begin() + static_cast<std::ptrdiff_t>(r2) * cols;

    std::swap_ranges(first1, first1 + cols, first2);
  }
};
}; // namespace math
}; // namespace engine
