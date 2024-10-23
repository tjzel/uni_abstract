#ifndef HALFSQUARETRIANGLE_HPP
#define HALFSQUARETRIANGLE_HPP

#include "Canvas.hpp"
#include "Figure.hpp"

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
