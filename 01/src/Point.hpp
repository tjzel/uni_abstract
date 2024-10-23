#ifndef POINT_HPP
#define POINT_HPP

#include <cmath>

struct Point {
  int x = {};
  int y = {};

  auto operator+=(const Point &other) -> Point & {
    x += other.x;
    y += other.y;
    return *this;
  }

  auto length() -> double { return std::sqrt(x * x + y * y); }
};

inline auto operator==(const Point &left, const Point &right) -> bool { return left.x == right.x && left.y == right.y; }

inline auto operator+(Point &left, Point right) -> Point { return {.x = left.x + right.x, .y = left.y + right.y}; }

inline auto operator*(Point &left, Point &right) -> int { return left.x * right.x + left.y * right.y; }

#endif // POINT_HPP
