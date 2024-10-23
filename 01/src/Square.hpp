#ifndef SQUARE_HPP
#define SQUARE_HPP

#include "Canvas.hpp"
#include "Figure.hpp"

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
