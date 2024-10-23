#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstdint>
using uint_ = uint16_t;
#define method virtual auto

#endif // TYPES_HPP

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
#ifndef LINE_HPP
#define LINE_HPP

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
#ifndef CANVAS_HPP
#define CANVAS_HPP

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
#ifndef FIGURE_HPP
#define FIGURE_HPP

class Figure {
public:
  virtual ~Figure() = default;
  method draw(Canvas &canvas) -> void = 0;
  method set_height(uint height) -> void = 0;
  method get_height() -> uint = 0;
  method set_width(uint height) -> void = 0;
  method get_width() -> uint = 0;
  method get_area() -> double = 0;
  method set_color(char color) -> void { _color = color; };
  method get_color() -> char { return _color; };
  method get_position() -> Point { return get_translation().asTranslation(); };
  method move(Line translation) -> void { _translation += translation; }
  method set_translation(Line translation) -> void { _translation = translation; };
  method get_translation() -> Line { return _translation; };

private:
  char _color = '?';
  Line _translation = {};
};

#endif // FIGURE_HPP
#ifndef SQUARE_HPP
#define SQUARE_HPP

class Square : public Figure {
public:
  auto draw(Canvas &canvas) -> void override {
    auto topLeft = get_position();
    auto topRight = topLeft + Point{.y = _width};
    auto topSide = Line(topLeft, topRight);

    auto bottomRight = topRight + Point{.x = _width};
    auto rightSide = Line(topRight, bottomRight);

    auto bottomLeft = bottomRight + Point{.y = -_width};
    auto bottomSide = Line(bottomRight, bottomLeft);

    auto leftSide = Line(bottomLeft, topLeft);

    canvas.drawLine(topSide, get_color());
    canvas.drawLine(rightSide, get_color());
    canvas.drawLine(bottomSide, get_color());
    canvas.drawLine(leftSide, get_color());

    canvas.flush();
  };
  auto set_width(uint width) -> void override { _width = width; };
  auto get_width() -> uint override { return _width; };
  auto set_height(uint height) -> void override { set_width(height); };
  auto get_height() -> uint override { return get_width(); };
  auto get_area() -> double override { return _width * _width; };

private:
  uint_ _width = {};
};

#endif // SQUARE_HPP
#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

class Rectangle : public Figure {
public:
  auto draw(Canvas &canvas) -> void override {
    auto topLeft = get_position();
    auto topRight = topLeft + Point{.y = _width};
    auto topSide = Line(topLeft, topRight);

    auto bottomRight = topRight + Point{.x = _height};
    auto rightSide = Line(topRight, bottomRight);

    auto bottomLeft = bottomRight + Point{.y = -_width};
    auto bottomSide = Line(bottomRight, bottomLeft);

    auto leftSide = Line(bottomLeft, topLeft);

    canvas.drawLine(topSide, get_color());
    canvas.drawLine(rightSide, get_color());
    canvas.drawLine(bottomSide, get_color());
    canvas.drawLine(leftSide, get_color());

    canvas.flush();
  };
  auto set_width(uint width) -> void override { _width = width; };
  auto get_width() -> uint override { return _width; };
  auto set_height(uint height) -> void override { _height = height; };
  auto get_height() -> uint override { return _height; };
  auto get_area() -> double override { return _width * _height; };

private:
  uint_ _width = {};
  uint_ _height = {};
};

#endif // RECTANGLE_HPP
#ifndef HALFSQUARETRIANGLE_HPP
#define HALFSQUARETRIANGLE_HPP

class HalfSquareTriangle : public Figure {
public:
  auto draw(Canvas &canvas) -> void override {
    auto topLeft = get_position();

    auto bottomRight = topLeft + Point{.x = _width, .y = _width};
    auto rightSide = Line(topLeft, bottomRight);

    auto bottomLeft = bottomRight + Point{.y = -_width};
    auto bottomSide = Line(bottomRight, bottomLeft);

    auto leftSide = Line(bottomLeft, topLeft);

    canvas.drawLine(rightSide, get_color());
    canvas.drawLine(bottomSide, get_color());
    canvas.drawLine(leftSide, get_color());

    canvas.flush();
  };
  auto set_width(uint width) -> void override { _width = width; };
  auto get_width() -> uint override { return _width; };
  auto set_height(uint height) -> void override { set_width(height); };
  auto get_height() -> uint override { return get_width(); };
  auto get_area() -> double override { return static_cast<double>(_width * _width) / 2; };

private:
  uint_ _width = {};
};

#endif // HALFSQUARETRIANGLE_HPP
#ifndef QUARTERSQUARETRIANGLE_HPP
#define QUARTERSQUARETRIANGLE_HPP

class QuarterSquareTriangle : public Figure {
public:
  auto draw(Canvas &canvas) -> void override {
    auto topLeft = get_position();
    auto topRight = topLeft + Point{.y = _width};
    auto topSide = Line(topLeft, topRight);

    auto bottomMiddle = topLeft + Point{.x = _width / 2, .y = _width / 2};
    auto rightSide = Line(topRight, bottomMiddle);

    auto leftSide = Line(bottomMiddle, topLeft);

    canvas.drawLine(topSide, get_color());
    canvas.drawLine(rightSide, get_color());
    canvas.drawLine(leftSide, get_color());

    canvas.flush();
  };
  auto set_width(uint width) -> void override { _width = width; };
  auto get_width() -> uint override { return _width; };
  auto set_height(uint height) -> void override { set_width(height); };
  auto get_height() -> uint override { return get_width(); };
  auto get_area() -> double override { return static_cast<double>(_width * _width) / 4; };

private:
  uint_ _width = {};
};

#endif // QUARTERSQUARETRIANGLE_HPP

int main() {
  const auto canvasWidth = 8;

  Square square;
  Canvas squareCanvas{canvasWidth, canvasWidth};
  square.set_width(4);
  square.draw(squareCanvas);
  std::cout << "\n";

  Rectangle rectangle;
  Canvas rectangleCanvas{canvasWidth, canvasWidth};
  rectangle.set_width(2);
  rectangle.set_height(4);
  rectangle.draw(rectangleCanvas);
  std::cout << "\n";

  QuarterSquareTriangle quarterSquareTriangle;
  Canvas quarterSquareTriangleCanvas{canvasWidth, canvasWidth};
  quarterSquareTriangle.set_width(4);
  quarterSquareTriangle.draw(quarterSquareTriangleCanvas);
  std::cout << "\n";

  HalfSquareTriangle halfSquareTriangle;
  Canvas halfSquareTriangleCanvas{canvasWidth, canvasWidth};
  halfSquareTriangle.set_width(4);
  halfSquareTriangle.draw(halfSquareTriangleCanvas);
  std::cout << "\n";

  Canvas squareMoveCanvas{canvasWidth, canvasWidth};
  Square squareMove;
  squareMove.set_width(4);
  squareMove.move({{.x = 0, .y = 0}, {.x = 2, .y = 2}});
  squareMove.draw(squareMoveCanvas);
  std::cout << "\n";

  Canvas rectangleMoveCanvas{canvasWidth, canvasWidth};
  Rectangle rectangleMove;
  rectangleMove.set_width(2);
  rectangleMove.set_height(4);
  rectangleMove.move({{.x = 0, .y = 0}, {.x = 2, .y = 2}});
  rectangleMove.draw(rectangleMoveCanvas);
  std::cout << "\n";

  Canvas quarterSquareTriangleMoveCanvas{canvasWidth, canvasWidth};
  QuarterSquareTriangle quarterSquareTriangleMove;
  quarterSquareTriangleMove.set_width(4);
  quarterSquareTriangleMove.move({{.x = 0, .y = 0}, {.x = 2, .y = 2}});
  quarterSquareTriangleMove.draw(quarterSquareTriangleMoveCanvas);
  std::cout << "\n";

  Canvas halfSquareTriangleMoveCanvas{canvasWidth, canvasWidth};
  HalfSquareTriangle halfSquareTriangleMove;
  halfSquareTriangleMove.set_width(4);
  halfSquareTriangleMove.move({{.x = 0, .y = 0}, {.x = 2, .y = 2}});
  halfSquareTriangleMove.draw(halfSquareTriangleMoveCanvas);
  std::cout << "\n";

  Canvas overlapCanvas{canvasWidth, canvasWidth};
  square.draw(overlapCanvas);
  std::cout << "\n";
  halfSquareTriangleMove.draw(overlapCanvas);
  std::cout << "\n";

  return 0;
};
