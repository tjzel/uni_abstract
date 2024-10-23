#include "Canvas.hpp"
#include "HalfSquareTriangle.hpp"
#include "QuarterSquareTriangle.hpp"
#include "Rectangle.hpp"
#include "Square.hpp"
#include <iostream>

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
}
