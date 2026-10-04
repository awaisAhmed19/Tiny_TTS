#include "../src/engine/math/matrix.h"
#include "../src/engine/math/vector.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

using engine::math::Matrix;
using engine::math::Vector;

namespace {

int tests_run = 0;
int tests_failed = 0;

void check(bool condition, const std::string &message) {
  ++tests_run;

  if (!condition) {
    ++tests_failed;
    std::cerr << "[FAIL] " << message << '\n';
  } else {
    std::cout << "[PASS] " << message << '\n';
  }
}

template <typename T> bool near(T a, T b, T epsilon = static_cast<T>(1e-6)) {
  return std::abs(a - b) <= epsilon;
}

// ============================================================
// VECTOR
// ============================================================

void testVectorConstruction() {
  Vector<double> v{1.0, 2.0, 3.0};

  check(v.size() == 3, "Vector size");
  check(v[0] == 1.0, "Vector element [0]");
  check(v[1] == 2.0, "Vector element [1]");
  check(v[2] == 3.0, "Vector element [2]");
}

void testVectorResize() {
  Vector<int> v{1, 2, 3};

  v.resize(5);

  check(v.size() == 5, "Vector resize grows");

  v[3] = 4;
  v[4] = 5;

  check(v[3] == 4, "Vector resized element [3]");
  check(v[4] == 5, "Vector resized element [4]");

  v.resize(2);

  check(v.size() == 2, "Vector resize shrinks");
  check(v[0] == 1, "Vector preserved element [0]");
  check(v[1] == 2, "Vector preserved element [1]");
}

void testVectorFill() {
  Vector<int> v(4);

  v.fill(7);

  check(v[0] == 7, "Vector fill [0]");
  check(v[1] == 7, "Vector fill [1]");
  check(v[2] == 7, "Vector fill [2]");
  check(v[3] == 7, "Vector fill [3]");

  v.zero();

  check(v.isZero(), "Vector zero");

  v.ones();

  check(v[0] == 1 && v[1] == 1 && v[2] == 1 && v[3] == 1, "Vector ones");
}

void testVectorArithmetic() {
  Vector<double> a{1.0, 2.0, 3.0};
  Vector<double> b{4.0, 5.0, 6.0};

  auto add = a + b;
  auto sub = b - a;
  auto mul = a * b;
  auto div = b / a;

  check(add.nearEqual(Vector<double>{5.0, 7.0, 9.0}), "Vector addition");

  check(sub.nearEqual(Vector<double>{3.0, 3.0, 3.0}), "Vector subtraction");

  check(mul.nearEqual(Vector<double>{4.0, 10.0, 18.0}),
        "Vector element-wise multiplication");

  check(div.nearEqual(Vector<double>{4.0, 2.5, 2.0}),
        "Vector element-wise division");
}

void testVectorScalarArithmetic() {
  Vector<double> v{1.0, 2.0, 3.0};

  check((v + 2.0).nearEqual(Vector<double>{3.0, 4.0, 5.0}), "Vector + scalar");

  check((v - 1.0).nearEqual(Vector<double>{0.0, 1.0, 2.0}), "Vector - scalar");

  check((v * 2.0).nearEqual(Vector<double>{2.0, 4.0, 6.0}), "Vector * scalar");

  check((v / 2.0).nearEqual(Vector<double>{0.5, 1.0, 1.5}), "Vector / scalar");
}

void testVectorMath() {
  Vector<double> a{3.0, 4.0};

  check(near(a.magnitudeSquared(), 25.0), "Vector magnitude squared");

  check(near(a.magnitude(), 5.0), "Vector magnitude");

  Vector<double> b{1.0, 2.0};

  check(near(a.dot(b), 11.0), "Vector dot product");
}

void testVectorNormalize() {
  Vector<double> v{3.0, 4.0};

  auto normalized = v.normalized();

  check(near(normalized.magnitude(), 1.0), "Vector normalized magnitude");

  check(near(normalized[0], 0.6), "Vector normalized [0]");

  check(near(normalized[1], 0.8), "Vector normalized [1]");
}

void testVectorStatistics() {
  Vector<double> v{1.0, 2.0, 3.0, 4.0};

  check(near(v.sum(), 10.0), "Vector sum");

  check(near(v.mean(), 2.5), "Vector mean");

  check(v.min() == 1.0, "Vector min");

  check(v.max() == 4.0, "Vector max");

  check(v.argmin() == 0, "Vector argmin");

  check(v.argmax() == 3, "Vector argmax");
}

void testVectorHadamard() {
  Vector<double> a{1.0, 2.0, 3.0};
  Vector<double> b{4.0, 5.0, 6.0};

  auto result = a.hadamard_product(b);

  check(result.nearEqual(Vector<double>{4.0, 10.0, 18.0}),
        "Vector Hadamard product");
}

void testVectorSoftmax() {
  Vector<double> v{1.0, 2.0, 3.0};

  auto result = v.softmax();

  check(near(result.sum(), 1.0), "Vector softmax sums to one");

  check(result[0] < result[1] && result[1] < result[2],
        "Vector softmax ordering");
}

void testVectorOneHot() {
  auto v = Vector<double>::one_hot(5, 2);

  check(v.size() == 5, "Vector one-hot size");

  check(v[0] == 0.0 && v[1] == 0.0 && v[2] == 1.0 && v[3] == 0.0 && v[4] == 0.0,
        "Vector one-hot values");
}

// ============================================================
// MATRIX
// ============================================================

void testMatrixConstruction() {
  Matrix<double> m(2, 3);

  check(m.rows() == 2, "Matrix rows");
  check(m.cols() == 3, "Matrix cols");
  check(m.size() == 6, "Matrix size");
}

void testMatrixElementAccess() {
  Matrix<int> m(2, 3);

  m[0][0] = 1;
  m[0][1] = 2;
  m[0][2] = 3;
  m[1][0] = 4;
  m[1][1] = 5;
  m[1][2] = 6;

  check(m(0, 0) == 1, "Matrix operator()");

  check(m.at(1, 2) == 6, "Matrix at()");

  check(m[1][1] == 5, "Matrix operator[][]");
}

void testMatrixZero() {
  Matrix<int> m(2, 2);

  m[0][0] = 1;
  m[0][1] = 2;
  m[1][0] = 3;
  m[1][1] = 4;

  m.zero();

  check(m.is_zero(), "Matrix zero");
}

void testMatrixIdentity() {
  Matrix<double> m(3, 3);

  m.identity();

  Matrix<double> expected(3, 3, {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0});

  check(m.nearEqual(expected), "Matrix identity");

  check(m.is_identity(), "Matrix is_identity");
}

void testMatrixDiagonal() {
  Matrix<double> m(3, 3);

  m.diagonal(5.0);

  Matrix<double> expected(3, 3, {5.0, 0.0, 0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 5.0});

  check(m.nearEqual(expected), "Matrix diagonal");
}

void testMatrixArithmetic() {
  Matrix<double> a(2, 2, {1.0, 2.0, 3.0, 4.0});

  Matrix<double> b(2, 2, {5.0, 6.0, 7.0, 8.0});

  check((a + b).nearEqual(Matrix<double>(2, 2, {6.0, 8.0, 10.0, 12.0})),
        "Matrix addition");

  check((b - a).nearEqual(Matrix<double>(2, 2, {4.0, 4.0, 4.0, 4.0})),
        "Matrix subtraction");
}

void testMatrixMultiplication() {
  Matrix<double> a(2, 3, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0});

  Matrix<double> b(3, 2, {7.0, 8.0, 9.0, 10.0, 11.0, 12.0});

  auto result = a * b;

  Matrix<double> expected(2, 2, {58.0, 64.0, 139.0, 154.0});

  check(result.nearEqual(expected), "Matrix multiplication");
}

void testMatrixTranspose() {
  Matrix<double> m(2, 3, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0});

  auto result = m.transpose();

  Matrix<double> expected(3, 2, {1.0, 4.0, 2.0, 5.0, 3.0, 6.0});

  check(result.nearEqual(expected), "Matrix transpose");
}

void testMatrixProperties() {
  Matrix<double> identity(3, 3);
  identity.identity();

  check(identity.is_square(), "Matrix is_square");

  check(identity.is_identity(), "Matrix identity property");

  Matrix<double> symmetric(2, 2, {1.0, 2.0, 2.0, 3.0});

  check(symmetric.is_symmetric(), "Matrix is_symmetric");
}

void testMatrixDeterminant() {
  Matrix<double> m(2, 2, {1.0, 2.0, 3.0, 4.0});

  check(near(m.determinant(), -2.0), "2x2 determinant");

  Matrix<double> m3(3, 3, {6.0, 1.0, 1.0, 4.0, -2.0, 5.0, 2.0, 8.0, 7.0});

  check(near(m3.determinant(), -306.0), "3x3 determinant");

  Matrix<double> singular(3, 3, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0});

  check(near(singular.determinant(), 0.0), "Singular matrix determinant");
}

void testMatrixInverse() {
  Matrix<double> m(2, 2, {4.0, 7.0, 2.0, 6.0});

  auto inv = m.inverse();

  Matrix<double> expected(2, 2, {0.6, -0.7, -0.2, 0.4});

  check(inv.nearEqual(expected), "Matrix inverse");

  auto identity = m * inv;

  Matrix<double> expectedIdentity(2, 2);
  expectedIdentity.identity();

  check(identity.nearEqual(expectedIdentity),
        "Matrix inverse verification A * A^-1");
}

// void testMatrixCofactor() {
//   Matrix<double> m(3, 3, {1.0, 2.0, 3.0, 0.0, 4.0, 5.0, 1.0, 0.0, 6.0});
//
//   auto result = m.cofactor();
//
//   Matrix<double> expected(3, 3,
//                           {24.0, 5.0, -4.0, -12.0, 3.0, 2.0, -2.0,
//                           -5.0, 4.0});
//
//   check(result.nearEqual(expected), "Matrix cofactor");
// }
//
// void testMatrixAdjugate() {
//   Matrix<double> m(2, 2, {1.0, 2.0, 3.0, 4.0});
//
//   auto result = m.adjugate();
//
//   Matrix<double> expected(2, 2, {4.0, -2.0, -3.0, 1.0});
//
//   check(result.nearEqual(expected), "Matrix adjugate");
// }

void testMatrixVectorMultiplication() {
  Matrix<double> m(2, 3, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0});

  Vector<double> v{10.0, 20.0, 30.0};

  auto result = m.multiplyVector(v);

  check(result.size() == 2, "Matrix-vector result size");

  check(near(result[0], 140.0), "Matrix-vector multiplication [0]");

  check(near(result[1], 320.0), "Matrix-vector multiplication [1]");
}

void testMatrixErrors() {
  Matrix<double> nonsquare(2, 3);

  bool caught = false;

  try {
    nonsquare.identity();
  } catch (const std::invalid_argument &) {
    caught = true;
  }

  check(caught, "Matrix identity rejects non-square matrix");

  caught = false;

  try {
    nonsquare.determinant();
  } catch (const std::invalid_argument &) {
    caught = true;
  }

  check(caught, "Matrix determinant rejects non-square matrix");
}

} // namespace

int main() {

  std::cout << "==============================\n";
  std::cout << " TinyTTS Math Tests\n";
  std::cout << "==============================\n\n";

  // Vector
  testVectorConstruction();
  testVectorResize();
  testVectorFill();
  testVectorArithmetic();
  testVectorScalarArithmetic();
  testVectorMath();
  testVectorNormalize();
  testVectorStatistics();
  testVectorHadamard();
  testVectorSoftmax();
  testVectorOneHot();

  // Matrix
  testMatrixConstruction();
  testMatrixElementAccess();
  testMatrixZero();
  testMatrixIdentity();
  testMatrixDiagonal();
  testMatrixArithmetic();
  testMatrixMultiplication();
  testMatrixTranspose();
  testMatrixProperties();
  testMatrixDeterminant();
  testMatrixInverse();
  // testMatrixCofactor();
  // testMatrixAdjugate();
  testMatrixVectorMultiplication();
  testMatrixErrors();

  std::cout << "\n==============================\n";
  std::cout << " Tests: " << tests_run << '\n';
  std::cout << " Failed: " << tests_failed << '\n';
  std::cout << "==============================\n";

  return tests_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
