#include <iostream>

/***
 * Zadanie wymaga C++ 23!
 * Zaimplementować klasę Matrix, która będzie szablonem z następującymi parametrami/metodami:
 * T - typ danych przechowywanych w macierzy
 * X - liczba wierszy macierzy
 * Y - liczba kolumn macierzy (domyślnie równa X)
 * Konstruktor domyślny, który inicjuje macierz "zerami"
 * Operator [] do pobierania i ustawiania elementów macierzy
 * Operator + do dodawania macierzy o tych samych rozmiarach
 * Operator * do mnożenia macierzy,
 *     gdzie liczba kolumn 1. macierzy = liczbie wierszy 2. macierzy
 *     oraz liczba wierszy 1. macierzy = liczba kolumn 2. macierzy
 * (*) Pełnoprawny operator * do mnożenia macierzy.
 * Zaimplementuj metodę display(), która wyświetli zawartość macierzy
 ***/

template <typename T, std::size_t X, std::size_t Y = X> class Matrix {
public:
  Matrix() { _data.fill({}); }

  T &operator[](std::size_t x, std::size_t y) { return _data.at(x).at(y); };

  Matrix operator+(Matrix<T, X, Y> &other) {
    Matrix<T, X, Y> result;
    for (std::size_t i = 0; i < X; i++) {
      for (std::size_t j = 0; j < Y; j++) {
        result[i, j] = _data.at(i).at(j) + other[i, j];
      }
    }

    return result;
  };

  Matrix operator*(Matrix<T, Y, X> &other) {
    Matrix<T, X, Y> result;
    for (std::size_t i = 0; i < X; i++) {
      for (std::size_t j = 0; j < Y; j++) {
        result[i, j] = _data.at(i).at(j) * other[j, i];
      }
    }
    return result;
  };

  void display() {
    for (const auto &row : _data) {
      for (const auto &element : row) {
        std::cout << element << " ";
      }
      std::cout << '\n';
    }
  }

private:
  std::array<std::array<T, Y>, X> _data;
  static_assert(X > 0, "X must be greater than 0");
  static_assert(Y > 0, "Y must be greater than 0");
};

int main() {
  const std::size_t x = 3; // musi być const!
  const std::size_t y = 4;

  Matrix<int, x> mat1;
  Matrix<int, x, x> mat2;
  Matrix<int, x, y> mat3;

  for (int i = 0; i < x; i++) {
    for (int j = 0; j < x; j++) {
      mat1[i, j] = i * x + j;
      mat2[i, j] = (i * x + j) * 2;
    }
  }

  for (int i = 0; i < x; i++) {
    for (int j = 0; j < y; j++) {
      mat3[i, j] = i * x + j;
    }
  }

  std::cout << "Matrix 1:" << std::endl;
  mat1.display();

  std::cout << "Matrix 2:" << std::endl;
  mat2.display();

  std::cout << "Matrix 3:" << std::endl;
  mat3.display();

  Matrix<int, 3> sum = mat1 + mat2;
  std::cout << "Sum of matrices:" << std::endl;
  sum.display();

  // Matrix<int, 3, 4> forbidden = mat1 + mat3;

  // (mat1 * mat3).display();

  return 0;
}
