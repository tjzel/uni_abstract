// TODO: Remove forward declarations.
#ifndef FIGURE_HPP
#define FIGURE_HPP

#include "Canvas.hpp"
#include "Line.hpp"
#include "Point.hpp"

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
