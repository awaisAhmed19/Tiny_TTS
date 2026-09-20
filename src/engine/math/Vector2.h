#pragma once
#include "Constants.h"
#include "types.h"
#include <cmath>
namespace engine {

namespace math {
/*
[x] size()
[x] magnitudesquared()
[x] magnitude()
[x] normalized()
[x] dot()
[x] lerp()
[x] clamp()
[x] min()
[x] max()
[x] abs()
[x] floor()
[x] ceil()
[x] round()
[x] iszero()
[x] isnormalized()
[x] distancesquared()
[x] distance()
[x] hadamardproduct()
[x] projectonto()
[x] rejectfrom()

// dsp / numerical math
[ ] sum()
[ ] mean()
[ ] variance()
[ ] standarddeviation()
[ ] minelement()
[ ] maxelement()
[ ] argmin()
[ ] argmax()

// vector products
[ ] crossproduct()       // vector3
[ ] tripleproduct()      // vector3
[ ] outerproduct()

// contact / geometry math
[ ] reflect()
[ ] refract()
[ ] perpendicular()      // vector2
[ ] rotate()             // vector2
[ ] rotatearound()       // vector2
*/
struct Vector2 {
  f32 x = 0.f;
  f32 y = 0.f;

  constexpr Vector2() = default;
  constexpr Vector2(f32 _x, f32 _y) : x(_x), y(_y) {}

  f32 &operator[](int i) { return ((&x)[i]); }
  const f32 &operator[](int i) const { return ((&x)[i]); }

  inline Vector2 operator-(const Vector2 &v) const {
    return Vector2(x - v.x, y - v.y);
  }
  inline Vector2 operator-() const { return Vector2(-x, -y); }
  Vector2 operator+(const Vector2 &v) const {
    return Vector2(x + v.x, y + v.y);
  }
  inline Vector2 operator*(const Vector2 &v) const {
    return Vector2(x * v.x, y * v.y);
  }
  inline Vector2 operator/(const Vector2 &v) const {
    RT_ASSERT(!v.isZero(),
              "Division by zero in math::Vector2::this->operator/; Hint : "
              "check your input vector u/ v <- ");
    return Vector2(x / v.x, y / v.y);
  }
  inline friend Vector2 operator*(f32 factor, const Vector2 &u) {
    return Vector2(factor * u.x, factor * u.y);
  }

  inline friend Vector2 operator-(f32 factor, const Vector2 &u) {
    return Vector2(factor - u.x, factor - u.y);
  }

  inline friend Vector2 operator+(f32 factor, const Vector2 &u) {
    return Vector2(factor + u.x, factor + u.y);
  }

  inline friend Vector2 operator/(f32 factor, const Vector2 &u) {
    return Vector2(factor / u.x, factor / u.y);
  }

  inline Vector2 &operator-=(const Vector2 &v) {
    x -= v.x;
    y -= v.y;
    return *this;
  }
  inline Vector2 &operator+=(const Vector2 &v) {
    x += v.x;
    y += v.y;
    return *this;
  }
  inline Vector2 &operator*=(const Vector2 &v) {
    x *= v.x;
    y *= v.y;
    return *this;
  }
  inline Vector2 operator/=(const Vector2 &v) {
    RT_ASSERT(!v.isZero(),
              "Division by zero in math::Vector2::this->operator/; Hint : "
              "check your input vector u/ v <- ");
    x /= v.x;
    y /= v.y;
    return *this;
  }
  inline bool operator==(const Vector2 &v) const {
    return (x == v.x && y == v.y);
  }
  inline bool operator!=(const Vector2 &v) const {
    return (x != v.x || y != v.y);
  }
  inline void invert() {
    x = -x;
    y = -y;
  }

  inline Vector2 operator*(const f32 scalar) const {
    return (Vector2(x * scalar, y * scalar));
  }

  inline Vector2 operator/(const f32 scalar) const {
    RT_ASSERT(
        scalar != 0,
        "Division by zero in math::Vector2::this->operator/scalar; Hint : "
        "check your input scalar <- ");

    return (Vector2(x / scalar, y / scalar));
  }
  inline Vector2 operator+(const f32 scalar) const {
    return (Vector2(x + scalar, y + scalar));
  }
  inline Vector2 operator-(const f32 scalar) const {
    return (Vector2(x - scalar, y - scalar));
  }

  inline Vector2 &operator*=(f32 scalar) {
    x *= scalar;
    y *= scalar;
    return (*this);
  }
  inline Vector2 &operator+=(f32 scalar) {
    x += scalar;
    y += scalar;
    return (*this);
  }
  inline Vector2 &operator-=(f32 scalar) {
    x -= scalar;
    y -= scalar;
    return (*this);
  }
  inline Vector2 &operator/=(f32 scalar) {
    RT_ASSERT(std::abs(scalar) > eps, "Division by zero");
    scalar = 1.f / scalar;
    x *= scalar;
    y *= scalar;
    return (*this);
  }

  inline bool isZero() const {
    return (std::abs(x) < eps && std::abs(y) < eps);
  }

  inline Vector2 lerp(const Vector2 &v, f32 t) const {
    return Vector2(x + ((v.x - x) * t), y + ((v.y - y) * t));
  }

  inline Vector2 slerp(const Vector2 &v, f32 t) const {
    f32 theta = std::acos(std::clamp(dot(v), -1.f, 1.f));
    if (theta < 0.001)
      return lerp(v, t);
    f32 sintheta = std::sin(theta);
    f32 ratioA = std::sin((1 - t) * theta) / sintheta;
    f32 ratioB = std::sin(t * theta) / sintheta;

    return {x * ratioA + v.x * ratioB, y * ratioA + v.y * ratioB};
  }
  inline f32 magnitude() const {
    return std::sqrt((this->x * this->x) + (this->y * this->y));
  }
  inline f32 magnitudeSquared() const { return x * x + y * y; }

  inline float length() const { return magnitude(); }
  inline Vector2 clampMagnitude(const f32 max_length) const {
    if (this->length() > max_length) {
      return (*this / this->length()) * max_length;
    }
    return *this;
  }

  inline Vector2 cclamp(const f32 min_x, const f32 min_y, const f32 max_x,
                        const f32 max_y) const {
    return Vector2(std::clamp(x, min_x, max_x), std::clamp(y, min_y, max_y));
  }

  inline Vector2 normalized() const {
    float mag = this->magnitude();
    RT_ASSERT(mag > eps, "cannot normalize zero vector");
    return {x / mag, y / mag};
  }

  inline void normalize() {
    f32 mag = this->magnitude();
    RT_ASSERT(mag > eps, "Cannot normalize zero vector");
    this->x /= mag;
    this->y /= mag;
  }
  inline f32 crossProduct(const Vector2 &v) const {
    return (this->x * v.y - this->y * v.x);
  }
  inline f32 dot(const Vector2 &v) const {
    return (this->x * v.x) + (this->y * v.y);
  }
  inline f32 angle(const Vector2 &v) const {
    f32 c = ((this->dot(v)) / (this->magnitude() * v.magnitude()));
    c = std::clamp(c, -1.f, 1.f);
    return std::acos(c);
  }

  inline bool isNormalized() const {
    return std::abs(this->magnitudeSquared() - 1.f) < eps;
  }

  inline f32 distance(const Vector2 &v) const {
    f32 dx = x - v.x;
    f32 dy = y - v.y;
    return std::sqrt(dx * dx + dy * dy);
  }

  inline f32 distanceSquared(const Vector2 &v) const {
    f32 dx = x - v.x;
    f32 dy = y - v.y;
    return dx * dx + dy * dy;
  }

  inline f32 scalarProjection(const Vector2 &v) const {
    return (this->dot(v)) / v.magnitude();
  }
  inline Vector2 hadamardProduct(const Vector2 &v) const {
    return {x * v.x, y * v.y};
  }
  inline Vector2 vectorProjection(const Vector2 &v) const {
    return v * (this->dot(v) / (v.magnitude() * v.magnitude()));
  }
  inline Vector2 rejectFrom(const Vector2 &v) const {
    return *this - this->vectorProjection(v);
  }
  // rotate around a point
  Vector2 rotateAround(const Vector2 &center, float angle) const {
    return (*this - center).rotate(angle) + center;
  }
  Vector2 reflect(const Vector2 &normal) const {
    return *this - normal * (2.f * dot(normal));
  }
  bool nearEqual(const Vector2 &v) const {
    return std::abs(x - v.x) < eps && std::abs(y - v.y) < eps;
  }
  float maxComponent() const { return std::max(x, y); }
  float minComponent() const { return std::min(x, y); }
  inline Vector2 perpendicular() const { return Vector2(-y, x); }
  inline Vector2 perpendicularCW() const { return Vector2(y, -x); }
  inline Vector2 rotate(const f32 angle) const {
    f32 r = angle * Pi / 180.f;
    return Vector2(x * std::cos(r) - y * std::sin(r),
                   x * std::sin(r) + y * std::cos(r));
  }
};

inline Vector2 max(const Vector2 &u, const Vector2 &v) {
  return Vector2(std::max(u.x, v.x), std::max(u.y, v.y));
}
inline Vector2 min(const Vector2 &u, const Vector2 &v) {
  return Vector2(std::min(u.x, v.x), std::min(u.y, v.y));
}
inline Vector2 maxlength(const Vector2 &u, const Vector2 &v) {
  return u.magnitudeSquared() > v.magnitudeSquared() ? u : v;
}
inline Vector2 minlength(const Vector2 &u, const Vector2 &v) {
  return u.magnitudeSquared() < v.magnitudeSquared() ? u : v;
}

inline Vector2 abs(const Vector2 &u) {
  return Vector2(std::abs(u.x), std::abs(u.y));
}

inline Vector2 ceil(const Vector2 &u) {
  return Vector2(std::ceil(u.x), std::ceil(u.y));
}
inline Vector2 floor(const Vector2 &u) {
  return Vector2(std::floor(u.x), std::floor(u.y));
}
inline Vector2 round(const Vector2 &u) {
  return Vector2(std::round(u.x), std::round(u.y));
}
}; // namespace math
}; // namespace engine
