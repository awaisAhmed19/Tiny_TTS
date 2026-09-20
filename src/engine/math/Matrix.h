#pragma once
#include "types.h"
#include <array>
#include <cmath>
#include <stdexcept>
#include <type_traits>

// Generic fixed-size matrix template, column-vector convention (M * v).
// Use this for anything outside the hot-path 2x2/3x3/4x4 specializations
// above (e.g. NxN Jacobians, generic linear-algebra utilities, batch solves).
// Matrix2/Matrix3/Matrix4 remain the hand-optimized types for engine hot paths;
// this template is not meant to replace them, only to cover the general case.
namespace engine {
namespace math {

template <typename T, int Rows, int Cols> struct Matrix {
  static_assert(Rows > 0 && Cols > 0, "Matrix dimensions must be positive");
  static_assert(std::is_arithmetic_v<T>, "Matrix requires an arithmetic type");

  std::array<std::array<T, Cols>, Rows> data{};

  constexpr Matrix() = default;

  explicit Matrix(T fillValue) {
    for (int i = 0; i < Rows; ++i)
      for (int j = 0; j < Cols; ++j)
        data[i][j] = fillValue;
  }

  Matrix(std::initializer_list<std::initializer_list<T>> rows) {
    int i = 0;
    for (const auto &row : rows) {
      int j = 0;
      for (const auto &val : row) {
        if (i < Rows && j < Cols)
          data[i][j] = val;
        ++j;
      }
      ++i;
    }
  }

  T *operator[](int row) {
    if (row < 0 || row >= Rows)
      throw std::out_of_range("Matrix row index out of range");
    return data[row].data();
  }
  const T *operator[](int row) const {
    if (row < 0 || row >= Rows)
      throw std::out_of_range("Matrix row index out of range");
    return data[row].data();
  }

  bool operator==(const Matrix &m) const {
    for (int i = 0; i < Rows; ++i)
      for (int j = 0; j < Cols; ++j)
        if (data[i][j] != m.data[i][j])
          return false;
    return true;
  }
  bool operator!=(const Matrix &m) const { return !(*this == m); }

  Matrix operator-() const {
    Matrix result;
    for (int i = 0; i < Rows; ++i)
      for (int j = 0; j < Cols; ++j)
        result.data[i][j] = -data[i][j];
    return result;
  }

  Matrix operator+(const Matrix &m) const {
    Matrix result;
    for (int i = 0; i < Rows; ++i)
      for (int j = 0; j < Cols; ++j)
        result.data[i][j] = data[i][j] + m.data[i][j];
    return result;
  }
  Matrix operator-(const Matrix &m) const {
    Matrix result;
    for (int i = 0; i < Rows; ++i)
      for (int j = 0; j < Cols; ++j)
        result.data[i][j] = data[i][j] - m.data[i][j];
    return result;
  }
  Matrix &operator+=(const Matrix &m) {
    for (int i = 0; i < Rows; ++i)
      for (int j = 0; j < Cols; ++j)
        data[i][j] += m.data[i][j];
    return *this;
  }
  Matrix &operator-=(const Matrix &m) {
    for (int i = 0; i < Rows; ++i)
      for (int j = 0; j < Cols; ++j)
        data[i][j] -= m.data[i][j];
    return *this;
  }

  Matrix operator*(T scalar) const {
    Matrix result;
    for (int i = 0; i < Rows; ++i)
      for (int j = 0; j < Cols; ++j)
        result.data[i][j] = data[i][j] * scalar;
    return result;
  }
  Matrix &operator*=(T scalar) {
    for (int i = 0; i < Rows; ++i)
      for (int j = 0; j < Cols; ++j)
        data[i][j] *= scalar;
    return *this;
  }
  Matrix operator/(T scalar) const {
    RT_ASSERT(std::abs(scalar) > static_cast<T>(1e-6), "Scalar cannot be zero");
    Matrix result;
    for (int i = 0; i < Rows; ++i)
      for (int j = 0; j < Cols; ++j)
        result.data[i][j] = data[i][j] / scalar;
    return result;
  }
  Matrix &operator/=(T scalar) {
    RT_ASSERT(std::abs(scalar) > static_cast<T>(1e-6), "Scalar cannot be zero");
    for (int i = 0; i < Rows; ++i)
      for (int j = 0; j < Cols; ++j)
        data[i][j] /= scalar;
    return *this;
  }
  friend Matrix operator*(T s, const Matrix &m) { return m * s; }

  // Matrix-matrix multiply: (Rows x Cols) * (Cols x OtherCols) -> (Rows x
  // OtherCols)
  template <int OtherCols>
  Matrix<T, Rows, OtherCols>
  operator*(const Matrix<T, Cols, OtherCols> &rhs) const {
    Matrix<T, Rows, OtherCols> result;
    for (int i = 0; i < Rows; ++i) {
      for (int j = 0; j < OtherCols; ++j) {
        T sum = static_cast<T>(0);
        for (int k = 0; k < Cols; ++k)
          sum += data[i][k] * rhs.data[k][j];
        result.data[i][j] = sum;
      }
    }
    return result;
  }

  // Matrix-vector multiply: (Rows x Cols) * (Cols x 1) -> (Rows x 1),
  // column-vector convention.
  std::array<T, Rows> operator*(const std::array<T, Cols> &v) const {
    std::array<T, Rows> result{};
    for (int i = 0; i < Rows; ++i) {
      T sum = static_cast<T>(0);
      for (int j = 0; j < Cols; ++j)
        sum += data[i][j] * v[j];
      result[i] = sum;
    }
    return result;
  }

  [[nodiscard]] Matrix<T, Cols, Rows> transposed() const {
    Matrix<T, Cols, Rows> result;
    for (int i = 0; i < Rows; ++i)
      for (int j = 0; j < Cols; ++j)
        result.data[j][i] = data[i][j];
    return result;
  }

  template <int R = Rows, int C = Cols, typename = std::enable_if_t<R == C>>
  static Matrix identity() {
    Matrix result;
    for (int i = 0; i < Rows; ++i)
      result.data[i][i] = static_cast<T>(1);
    return result;
  }

  static Matrix zero() { return Matrix(); }

  template <int R = Rows, int C = Cols, typename = std::enable_if_t<R == C>>
  [[nodiscard]] T trace() const {
    T t = static_cast<T>(0);
    for (int i = 0; i < Rows; ++i)
      t += data[i][i];
    return t;
  }

  // Determinant via Laplace expansion. Correct for any size but O(n!) —
  // intended for small/general use (NxN utilities), NOT hot paths.
  // Use the dedicated Matrix2/Matrix3/Matrix4 determinant() for engine hot
  // paths.
  template <int R = Rows, int C = Cols, typename = std::enable_if_t<R == C>>
  [[nodiscard]] T determinant() const {
    return determinantImpl(data, Rows);
  }

private:
  static T determinantImpl(const std::array<std::array<T, Cols>, Rows> &m,
                           int n) {
    if (n == 1)
      return m[0][0];
    if (n == 2)
      return m[0][0] * m[1][1] - m[0][1] * m[1][0];

    T det = static_cast<T>(0);
    T sign = static_cast<T>(1);
    for (int col = 0; col < n; ++col) {
      std::array<std::array<T, Cols>, Rows> minor{};
      for (int i = 1; i < n; ++i) {
        int mj = 0;
        for (int j = 0; j < n; ++j) {
          if (j == col)
            continue;
          minor[i - 1][mj++] = m[i][j];
        }
      }
      det += sign * m[0][col] * determinantImpl(minor, n - 1);
      sign = -sign;
    }
    return det;
  }
};

}; // namespace math
}; // namespace engine
