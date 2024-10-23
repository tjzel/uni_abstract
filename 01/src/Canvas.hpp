#ifndef CANVAS_HPP
#define CANVAS_HPP

#include "Line.hpp"
#include "Point.hpp"
#include "Types.hpp"
#include <cstdlib>
#include <iostream>
#include <vector>

class Canvas {
public:
  auto drawLine(Line line, char color) -> void {
    const auto begin = line.get_begin();
    const auto end = line.get_end();

    const auto xDirection = _getDirection(begin.x, end.x);
    const auto yDirection = _getDirection(begin.y, end.y);

    auto position = line.get_begin();
    if (position.x < 0) {
      position.x = 0;
    }
    if (position.y < 0) {
      position.y = 0;
    }
    while (_isInRange(position.x, position.y, begin, end)) {
      _paint(position.x, position.y, color);
      const Point xCandidate = {.x = position.x + xDirection, .y = position.y};
      const Point yCandidate = {.x = position.x, .y = position.y + yDirection};
      const Point xyCandidate = {.x = position.x + xDirection, .y = position.y + yDirection};

      const auto badPointDistance = 999;

      auto xCandidateDistance = line.getDistanceTo(xCandidate);
      if (xCandidate == position) {
        xCandidateDistance = badPointDistance;
      }
      auto yCandidateDistance = line.getDistanceTo(yCandidate);
      if (yCandidate == position) {
        yCandidateDistance = badPointDistance;
      }
      auto xyCandidateDistance = line.getDistanceTo(xyCandidate);
      if (xyCandidate == position) {
        xyCandidateDistance = badPointDistance;
      }

      position = _getPositionCandidate(xCandidate, xCandidateDistance, yCandidate, yCandidateDistance, xyCandidate,
                                       xyCandidateDistance);
    }
  };

  auto flush() -> void {
    for (const auto &row : _canvas) {
      for (auto symbol : row) {
        std::cout << symbol;
      }
      std::cout << '\n';
    }
  }

  Canvas(uint width, uint height) : _width(width), _height(height) {}

private:
  auto _paint(uint_ x, uint_ y, char color) -> void { _canvas[x][y] = color; }

  auto _isInRange(uint_ x, uint_ y, Point begin, Point end) -> bool {
    return (((begin.x <= x && x <= end.x) || (end.x <= x && x <= begin.x)) &&
            ((begin.y <= y && y <= end.y) || (end.y <= y && y <= begin.y)));
  }

  auto _getDirection(int begin, int end) -> int {
    const auto partial = end - begin;
    return partial > 0 ? 1 : partial < 0 ? -1 : 0;
  }

  Point _getPositionCandidate(Point xCandidate, double xCandidateDistance, Point yCandidate, double yCandidateDistance,
                              Point xyCandidate, double xyCandidateDistance) {
    return (xCandidateDistance <= yCandidateDistance ? (xCandidateDistance <= xyCandidateDistance ? xCandidate : xyCandidate)
                                                     : (yCandidateDistance <= xyCandidateDistance ? yCandidate : xyCandidate));
  }

  static uint defaultCanvasWidth;

  uint _width = defaultCanvasWidth;
  uint _height = defaultCanvasWidth;
  std::vector<std::vector<char>> _canvas{_height, std::vector<char>(_width, '.')};
};

uint Canvas::defaultCanvasWidth = 16; // NOLINT

#endif // CANVAS_HPP
