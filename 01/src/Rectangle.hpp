#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include "Canvas.hpp"
#include "Figure.hpp"

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
