#ifndef QUARTERSQUARETRIANGLE_HPP
#define QUARTERSQUARETRIANGLE_HPP

#include "Canvas.hpp"
#include "Figure.hpp"

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
