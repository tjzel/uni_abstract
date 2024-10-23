#ifndef LINE_HPP
#define LINE_HPP

#include "Point.hpp"
#include <cstdlib>

class Line {
public:
  auto set_begin(Point begin) -> void { _begin = begin; };
  auto set_start(Point begin) -> void { set_begin(begin); };
  auto get_begin() -> Point { return _begin; };
  auto set_end(Point end) -> void { _end = end; };
  auto set_stop(Point end) -> void { set_end(end); };
  auto get_end() -> Point { return _end; };
  auto is_point() -> bool { return _begin == _end; };
  auto operator+=(const Line &translation) -> Line & {
    _begin += translation._begin;
    _end += translation._end;
    return *this;
  };
  auto asTranslation() -> Point { return {.x = _end.x - _begin.x, .y = _end.y - _begin.y}; }
  auto getDistanceTo(Point point) -> double {
    Point vector = asTranslation();
    Point tangentVector = {.x = -vector.y, .y = vector.x};
    Point distanceVector = {.x = point.x - _begin.x, .y = point.y - _begin.y};
    double distance = distanceVector * tangentVector;
    distance /= tangentVector.length();
    distance = std::fabs(distance);
    return distance;
  }

  Line() = default;
  Line(Point begin, Point end) : _begin(begin), _end(end) {};

private:
  Point _begin = {};
  Point _end = {};
};

#endif // LINE_HPP
