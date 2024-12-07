#include <concepts>
#include <iostream>
#include <list>
#include <string>
#include <vector>

/***
 * Zaimplementować koncepty
 * (i) `Container` który wymaga:
 *   - atrybutu `value_type`
 *   - atrybutu `iterator`
 *   - metody `begin`
 *   - metody `end`
 *   - metody `insert(iterator, value_type)`
 * (ii) `Predicate` który wymaga, aby funkcja przyjmowała typ T,
 *      a zwracała coś co można interpretować jako bool
 * (iii) oraz metodę `filterContainer`, która zwraca nowy kontener z elementami
 *       które spełniają predykat.
 */

template <typename TContainer>
concept Container = requires(TContainer c) {
  typename TContainer::value_type;
  typename TContainer::iterator;
  { c.begin() } -> std::same_as<typename TContainer::iterator>;
  { c.end() } -> std::same_as<typename TContainer::iterator>;
  { c.insert(c.begin(), typename TContainer::value_type{}) };
};

template <typename TPredicate, typename TArg>
concept Predicate = requires(TPredicate predicate, TArg arg) {
  { predicate(arg) } -> std::convertible_to<bool>;
};

template <Container TContainer, Predicate<typename TContainer::value_type> TPredicate>
TContainer filterContainer(const TContainer &container, TPredicate predicate) {
  TContainer result{};
  for (const auto &element : container) {
    if (predicate(element)) {
      result.insert(result.end(), element);
    }
  }
  return result;
}

int main() {
  std::vector<int> numbers = {1, 2, 3, 4, 5, 6};
  auto isEven = [](int x) { return x % 2 == 0; };
  auto evenNumbers = filterContainer(numbers, isEven);
  std::cout << "Even numbers: ";
  for (int n : evenNumbers) {
    std::cout << n << " ";
  }
  std::cout << "\n";

  std::list<std::string> words = {"apple", "banana", "cherry", "date"};
  auto startsWithB = [](const std::string &word) { return word.starts_with("b"); };
  auto bWords = filterContainer(words, startsWithB);
  std::cout << "Words starting with 'b': ";
  for (const auto &word : bWords) {
    std::cout << word << " ";
  }
  std::cout << "\n";

  return 0;
}
