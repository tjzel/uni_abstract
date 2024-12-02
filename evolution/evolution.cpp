#include <algorithm>
#include <cassert>
#include <concepts>
#include <ctime>
#include <iostream>
#include <random>
#include <type_traits>
#include <vector>

/***
 * Przygotować prosty algorytm ewolucyjny dla populacji liczb, wektorów lub tablic.
 * Klasy wytycznych mają kontrolować działanie algorytmu.
 * Jako parametry szablonu algorytmu ewolucyjnego mogą zostać podane typy, szablony lub instancje policy,
 * według Państwa wyboru.
 * Celem jest, poza funkcjonalnym algorytmem, wygodna konfiguracja algorytmu, i na to będę zwracał szczególną uwagę!
 * Mogą Państwo dowolnie modyfikować kod, o ile uznają Państwo, że to poprawi użytkowość,
 * przy czym sama zasada działania algorytmu ma się nie zmienić.
 * Istotne zmiany API powinny zostać opisane i podane przykłady użycia.
 *
 * Klasy wytycznych dla inicjalizacji populacji:
 *  - RandomInitiationPolicy<Type, MIN, MAX>: wypełnia populację losowymi osobnikami z zakresu (MIN, MAX)
 *  - LinSpaceInitiationPolicy<Type, MIN, MAX>: wypełnia populację osobnikami z zakresu (MIN, MAX),
 *    tak aby osobniki były liniowo rozłożone pomiędzy MIN i MAX.
 * Klasy wytycznych dla mutacji populacji:
 *  - PercentageMutationPolicy<Type, CHANCE, INTENSITY>: mutuje z prawdopodobieństwem CHANCE
 *    i mnoży wartość osobnika przez liczbę z zakresu (1 - INTENSITY/100, 1 + INTENSITY/100).
 *  - AbsoluteMutationPolicy<Type, CHANCE, INTENSITY>: mutuje z prawdopodobieństwem CHANCE
 *    i dodaje do wartości osobnika losową wartość z zakresu (-INTENSITY, INTENSITY).
 * Klasy wytycznych dla krzyżowania populacji:
 *  - AverageCrossoverPolicy<Type, WEIGHT>: tworzy nowego osobnika jako średnią ważoną rodziców (wagi to WEIGHT i 1 - WEIGHT).
 *    W przypadku wektorów wagi powinny być wektorami o wartościach z zakresu (0, 1) i tej samej długości co Type.
 *  - RandomCrossoverPolicy<Type>: To samo co wyżej, ale waga jest losowa.
 * Klasy wytycznych dla selekcji:
 *  - RandomSelectionPolicy<Type>: wybiera dwoje rodziców w sposób losowy.
 *  - UniqueRandomSelectionPolicy<Type>: wybiera dwoje rodziców w sposób losowy, ale bez powtórzeń (każda para jest wybierana
 * dwukrotnie).
 *  - TargetSelectionPolicy<Type, FIRST, LAST>: wybiera dwoje rodziców w sposób losowy,
 *    ale w taki sposób, że cała populacja jest ułożona w rankingu od najlepszego do najgorszego.
 *    Osobnik pierwszy w rankingu ma mieć FIRST% szans, a ostatni LAST%. Reszta ma szanse malejące w sposób liniowy.
 * Klasy wytycznych dla zakończenia algorytmu:
 *  - MaxGenStopConditionPolicy<Type, PARAM>: przerywa algorytm po PARAM generacjach.
 *  - StableAvgStopConditionPolicy<Type, PARAM>: przerywa algorytm, jeżeli od poprzedniego sprawdzenia warunku średnia generacji
 *    nie zmieniła się o więcej niż PARAM.
 */

/***
Zdecydowałem zamienić predefiniowane struktury na koncepty. W moim zdaniu,
w kontekście tego zadania klasy abstrakcyjne mijają się z celem.

Zdecydowałem wspierać tylko wartości numeryczne oraz tablice stałej wielkości
(std::array) jako typy osobników. Wektory zmiennej długości w takim przypadku
nadmiernie komplikują kod i uniemożliwiają wygodne korzystanie z szablonów.

StopConditionPolicy zostało polem klasy, ze względu na to, że
`StableAvgStopConditionPolicy` zawiera stan.
 */

/* #region NumeralType */
template <typename TEntity, typename = void> struct NumeralType;

template <typename TEntity> struct NumeralType<TEntity, std::enable_if_t<std::is_arithmetic_v<TEntity>>> {
  using Type = TEntity;
};

template <typename TEntity>
struct NumeralType<TEntity,
                   std::enable_if_t<std::is_array_v<TEntity> && std::is_arithmetic_v<std::remove_all_extents_t<TEntity>>>> {
  using Type = std::remove_all_extents_t<TEntity>;
};

template <typename TEntity> struct NumeralType<TEntity, std::void_t<typename TEntity::value_type>> {
  using Type = typename TEntity::value_type;
};

template <typename TEntity> using NumeralType_t = typename NumeralType<TEntity>::Type;

static_assert(std::is_same_v<double, NumeralType_t<double>>);
static_assert(std::is_same_v<double, NumeralType_t<std::vector<double>>>);
static_assert(std::is_same_v<double, NumeralType_t<std::array<double, 0>>>);
/* #endregion */

/* #region Distribution */
template <typename TEntity> struct UniformDistribution {
  using Numeral = NumeralType_t<TEntity>;
  using Distribution = std::conditional_t<std::is_floating_point_v<Numeral>, std::uniform_real_distribution<Numeral>,
                                          std::uniform_int_distribution<Numeral>>;
  using Generator = std::mt19937;
};
/* #endregion */

/* #region PrintEntity */
template <typename TEntity, typename = void> struct PrintEntity;

template <typename TEntity> struct PrintEntity<TEntity, std::enable_if_t<std::is_arithmetic_v<TEntity>>> {
  static void print(const TEntity &entity) { std::cout << entity << " "; }
};

template <typename TEntity> struct PrintEntity<TEntity, std::enable_if_t<!std::is_arithmetic_v<TEntity>>> {
  static void print(const TEntity &entity) {
    for (const auto &element : entity) {
      std::cout << element << " ";
    }
    std::cout << "\n";
  }
};
/* #endregion */

/* #region InitiationPolicy */
template <typename TEntity, typename = void> struct InitializeEntity;

template <typename TEntity> struct InitializeEntity<TEntity, std::enable_if_t<std::is_arithmetic_v<TEntity>>> {
  inline static void initRandom(TEntity &individual, typename UniformDistribution<TEntity>::Generator &generator,
                                typename UniformDistribution<TEntity>::Distribution &distribution) {
    individual = distribution(generator);
  }

  inline static void initValue(TEntity &individual, NumeralType_t<TEntity> value) { individual = value; }
};

template <typename TEntity> struct InitializeEntity<TEntity, std::enable_if_t<!std::is_arithmetic_v<TEntity>>> {
  inline static void initRandom(TEntity &individual,
                                typename UniformDistribution<typename TEntity::value_type>::Generator &generator,
                                typename UniformDistribution<typename TEntity::value_type>::Distribution &distribution) {
    for (auto &element : individual) {
      element = distribution(generator);
    }
  }

  inline static void initValue(TEntity &individual, const typename TEntity::value_type &value) {
    for (auto &element : individual) {
      element = value;
    }
  }
};

template <typename TInitiationPolicy, typename TEntity>
concept InitiationPolicy = requires(std::vector<TEntity> &population, std::size_t populationSize) {
  { TInitiationPolicy::init(population, populationSize) } -> std::same_as<void>;
};

template <typename TEntity, NumeralType_t<TEntity> TMIN, NumeralType_t<TEntity> TMAX> struct RandomInitiationPolicy {
  static void init(std::vector<TEntity> &population, std::size_t populationSize) {
    assert(populationSize > 1);
    typename UniformDistribution<TEntity>::Generator generator{std::random_device{}()};
    typename UniformDistribution<TEntity>::Distribution distribution{TMIN, TMAX};
    population = std::vector<TEntity>(populationSize);
    for (auto &individual : population) {
      InitializeEntity<TEntity>::initRandom(individual, generator, distribution);
    }
  }
};

template <typename TEntity, NumeralType_t<TEntity> TMIN, NumeralType_t<TEntity> TMAX> struct LinSpaceInitiationPolicy {
  static void init(std::vector<TEntity> &population, std::size_t populationSize) {
    assert(populationSize > 1);
    const NumeralType_t<TEntity> step = (TMAX - TMIN) / (populationSize - 1);
    population = std::vector<TEntity>(populationSize);
    auto value = TMIN;
    for (auto &individual : population) {
      InitializeEntity<TEntity>::initValue(individual, value);
      value += step;
    }
  }
};

constexpr double TEST_MIN = 0;
constexpr double TEST_MAX = 10;
static_assert(InitiationPolicy<RandomInitiationPolicy<double, TEST_MIN, TEST_MAX>, double>);
static_assert(InitiationPolicy<LinSpaceInitiationPolicy<double, TEST_MIN, TEST_MAX>, double>);

/* #endregion */

/* #region MutationPolicy */
template <typename TEntity, typename = void> struct MutateEntity;

template <typename TEntity> struct MutateEntity<TEntity, std::enable_if_t<std::is_arithmetic_v<TEntity>>> {
  static void mutatePercentage(TEntity &entity, typename UniformDistribution<TEntity>::Generator &generator,
                               typename UniformDistribution<double>::Distribution &intensityDistribution) {
    entity *= intensityDistribution(generator);
  }

  static void mutateAbsolute(TEntity &entity, typename UniformDistribution<TEntity>::Generator &generator,
                             typename UniformDistribution<double>::Distribution &intensityDistribution) {
    entity += intensityDistribution(generator);
  }
};

template <typename TEntity> struct MutateEntity<TEntity, std::enable_if_t<!std::is_arithmetic_v<TEntity>>> {
  static void mutatePercentage(TEntity &entity,
                               typename UniformDistribution<typename TEntity::value_type>::Generator &generator,
                               typename UniformDistribution<double>::Distribution &intensityDistribution) {
    for (auto &element : entity) {
      element *= intensityDistribution(generator);
    }
  }

  static void mutateAbsolute(TEntity &entity, typename UniformDistribution<typename TEntity::value_type>::Generator &generator,
                             typename UniformDistribution<double>::Distribution &intensityDistribution) {
    for (auto &element : entity) {
      element += intensityDistribution(generator);
    }
  }
};

template <typename TMutationPolicy, typename TEntity>
concept MutationPolicy = requires(std::vector<TEntity> &population) {
  { TMutationPolicy::mutate(population) } -> std::same_as<void>;
};

template <typename TEntity, double TCHANCE, NumeralType_t<TEntity> TINTENSITY> struct PercentageMutationPolicy {
  static void mutate(std::vector<TEntity> &population) {
    typename UniformDistribution<double>::Generator generator{std::random_device{}()};
    typename UniformDistribution<double>::Distribution chanceDistribution{0, 1};
    typename UniformDistribution<double>::Distribution intensityDistribution{1 - TINTENSITY / 100.0, 1 + TINTENSITY / 100.0};
    for (TEntity &individual : population) {
      if (chanceDistribution(generator) >= TCHANCE) {
        MutateEntity<TEntity>::mutatePercentage(individual, generator, intensityDistribution);
      }
    }
  }
};

template <typename TEntity, double TCHANCE, NumeralType_t<TEntity> TINTENSITY> struct AbsoluteMutationPolicy {
  static void mutate(std::vector<TEntity> &population) {
    typename UniformDistribution<TEntity>::Generator generator{std::random_device{}()};
    typename UniformDistribution<TEntity>::Distribution chanceDistribution{0, 1};
    typename UniformDistribution<double>::Distribution intensityDistribution{-TINTENSITY, TINTENSITY};
    for (TEntity &individual : population) {
      if (chanceDistribution(generator) >= TCHANCE) {
        MutateEntity<TEntity>::mutateAbsolute(individual, generator, intensityDistribution);
      }
    }
  }
};

constexpr double TEST_CHANCE = 0.1;
constexpr double TEST_INTENSITY = 10;
static_assert(MutationPolicy<PercentageMutationPolicy<double, TEST_CHANCE, TEST_INTENSITY>, double>);
static_assert(MutationPolicy<AbsoluteMutationPolicy<double, TEST_CHANCE, TEST_INTENSITY>, double>);
/* #endregion */

/* #region CrossoverPolicy */
template <typename TEntity, typename = void> struct CrossoverEntity;

template <typename TEntity> struct CrossoverEntity<TEntity, std::enable_if_t<std::is_arithmetic_v<TEntity>>> {
  static inline TEntity crossover(const TEntity &parent1, const TEntity &parent2, double weight) {
    return (parent1 * weight + parent2 * (1 - weight));
  }
};

template <typename TEntity> struct CrossoverEntity<TEntity, std::enable_if_t<!std::is_arithmetic_v<TEntity>>> {
  static inline TEntity crossover(const TEntity &parent1, const TEntity &parent2, double weight) {
    TEntity offspring{};
    for (std::size_t i = 0; i < parent1.size(); ++i) {
      auto it1 = parent1.begin();
      auto it2 = parent2.begin();
      auto itOffspring = offspring.begin();
      while (it1 != parent1.end() && it2 != parent2.end() && itOffspring != offspring.end()) {
        *itOffspring = *it1 * weight + *it2 * (1 - weight);
        ++it1;
        ++it2;
        ++itOffspring;
      }
    }
    return offspring;
  }
};

template <typename TCrossoverPolicy, typename TEntity>
concept CrossoverPolicy = requires(TEntity parent1, TEntity parent2) {
  { TCrossoverPolicy::crossover(parent1, parent2) } -> std::same_as<TEntity>;
};

template <typename TEntity, double TWEIGHT> struct AverageCrossoverPolicy {
  static TEntity crossover(const TEntity &parent1, const TEntity &parent2) {
    assert(TWEIGHT >= 0 && TWEIGHT <= 1);
    return CrossoverEntity<TEntity>::crossover(parent1, parent2, TWEIGHT);
  }
};

template <typename TEntity> struct RandomCrossoverPolicy {
  static TEntity crossover(const TEntity &parent1, const TEntity &parent2) {
    typename UniformDistribution<double>::Generator generator{std::random_device{}()};
    typename UniformDistribution<double>::Distribution distribution{0, 1};
    const double weight = distribution(generator);
    return CrossoverEntity<TEntity>::crossover(parent1, parent2, weight);
  }
};

constexpr double TEST_WEIGHT = 0.5;
static_assert(CrossoverPolicy<AverageCrossoverPolicy<double, TEST_WEIGHT>, double>);
static_assert(CrossoverPolicy<RandomCrossoverPolicy<double>, double>);
/* #endregion */

/* #region SelectionPolicy */

template <typename TComparator, typename TEntity>
concept CompareEntities = requires(const TEntity &lhs, const TEntity &rhs) {
  { TComparator::compare(lhs, rhs) } -> std::same_as<bool>;
};

template <typename TEntity, typename = void> struct AbsoluteValueComparator;

template <typename TEntity> struct AbsoluteValueComparator<TEntity, std::enable_if_t<std::is_arithmetic_v<TEntity>>> {
  static bool compare(const TEntity &lhs, const TEntity &rhs) { return std::abs(lhs) > std::abs(rhs); }
};

template <typename TEntity> struct AbsoluteValueComparator<TEntity, std::enable_if_t<!std::is_arithmetic_v<TEntity>>> {
  static bool compare(const TEntity &lhs, const TEntity &rhs) {
    return std::abs(std::accumulate(std::begin(lhs), std::end(lhs), NumeralType_t<TEntity>{})) >
           std::abs(std::accumulate(std::begin(rhs), std::end(rhs), NumeralType_t<TEntity>{}));
  }
};

using CompareTestType = std::array<double, 3>;
static_assert(CompareEntities<AbsoluteValueComparator<CompareTestType>, CompareTestType>);

template <typename TEntity, CompareEntities<TEntity> TComparator> struct SortEntity {
  static void sort(std::vector<TEntity> &population) { std::sort(population.begin(), population.end(), TComparator::compare); }
};

template <typename TSelectionPolicy, typename TEntity>
concept SelectionPolicy = requires(std::vector<TEntity> &population) {
  { TSelectionPolicy::select(population) } -> std::same_as<std::pair<TEntity, TEntity>>;
};

template <typename TEntity> struct RandomSelectionPolicy {
  static std::pair<TEntity, TEntity> select(const std::vector<TEntity> &population) {
    typename UniformDistribution<std::size_t>::Generator generator{std::random_device{}()};
    typename UniformDistribution<std::size_t>::Distribution distribution{0, population.size() - 1};
    const TEntity &parent1 = population[distribution(generator)];
    const TEntity &parent2 = population[distribution(generator)];
    return {parent1, parent2};
  }
};

template <typename TEntity> struct UniqueRandomSelectionPolicy {
  static std::pair<TEntity, TEntity> select(const std::vector<TEntity> &population) {
    typename UniformDistribution<std::size_t>::Generator generator{std::random_device{}()};
    typename UniformDistribution<std::size_t>::Distribution distribution{0, population.size() - 1};
    // const TEntity &parent1 = population[distribution(generator)];
    const size_t parent1Index = distribution(generator);
    size_t parent2Index = parent1Index;
    while ((parent2Index = distribution(generator)) == parent1Index) {
    };
    return {population[parent1Index], population[parent2Index]};
  }
};

template <typename TEntity, double TFIRST, double TLAST, CompareEntities<TEntity> TComparator> struct TargetSelectionPolicy {
  static std::pair<TEntity, TEntity> select(std::vector<TEntity> &population) {
    assert(TFIRST >= 0 && TLAST >= 0 && TFIRST >= TLAST);
    assert(population.size() > 1);

    SortEntity<TEntity, TComparator>::sort(population);

    const double step = (TFIRST - TLAST) / (population.size() - 1);
    const double sumOfWeights = (TFIRST + TLAST) * population.size() / 2;
    typename UniformDistribution<double>::Generator generator{std::random_device{}()};
    typename UniformDistribution<double>::Distribution distribution{0, 1};

    double parent1Random = distribution(generator);
    double parent1Chance = TLAST / sumOfWeights;
    size_t parent1Index = 0;
    while (parent1Random > parent1Chance) {
      parent1Index++;
      parent1Chance += step * static_cast<double>(parent1Index);
    }

    double parent2Random = distribution(generator);
    double parent2Chance = TLAST / sumOfWeights;
    size_t parent2Index = 0;
    while (parent2Random > parent2Chance) {
      parent2Index++;
      parent2Chance += step * static_cast<double>(parent2Index);
    }

    return {population[parent1Index], population[parent2Index]};
  }
};

static_assert(SelectionPolicy<RandomSelectionPolicy<double>, double>);
static_assert(SelectionPolicy<UniqueRandomSelectionPolicy<double>, double>);
constexpr double TEST_FIRST = 10;
constexpr double TEST_LAST = 1;
static_assert(SelectionPolicy<TargetSelectionPolicy<double, TEST_FIRST, TEST_LAST, AbsoluteValueComparator<double>>, double>);
/* #endregion */

/* #region StopConditionPolicy */
template <typename TEntity, typename = void> struct EntityAverage;

template <typename TEntity> struct EntityAverage<TEntity, std::enable_if_t<std::is_arithmetic_v<TEntity>>> {
  static inline NumeralType_t<TEntity> average(const std::vector<TEntity> &population) {
    return std::accumulate(population.begin(), population.end(), 0) / population.size();
  }
};

template <typename TEntity> struct EntityAverage<TEntity, std::enable_if_t<!std::is_arithmetic_v<TEntity>>> {
  static inline NumeralType_t<TEntity> average(const std::vector<TEntity> &population) {
    NumeralType_t<TEntity> avg{};
    for (const auto &individual : population) {
      for (const auto &value : individual) {
        avg += value;
      }
    }
    avg /= population.size();
    return avg;
  }
};

template <typename TStopConditionPolicy, typename TNumeral>
concept StopConditionPolicy =
    requires(TStopConditionPolicy stopConditionPolicy, std::vector<TNumeral> &population, int generation) {
      { stopConditionPolicy.shouldStop(population, generation) } -> std::same_as<bool>;
    };

template <typename TEntity, uint TPARAM> struct MaxGenStopConditionPolicy {
  bool shouldStop(const std::vector<TEntity> & /*population*/, int generation) { return generation >= TPARAM; }
};

template <typename TEntity, NumeralType_t<TEntity> TPARAM> struct StableAvgStopConditionPolicy {
  bool shouldStop(const std::vector<TEntity> &population, int /*generation*/) {
    NumeralType_t<TEntity> avg = EntityAverage<TEntity>::average(population);
    if (firstCheck) {
      lastAvg = avg;
      firstCheck = false;
      return false;
    }
    if (std::abs(avg - lastAvg) > TPARAM) {
      lastAvg = avg;
      return false;
    }
    return true;
  }

private:
  NumeralType_t<TEntity> lastAvg{};
  bool firstCheck = true;
};

constexpr int TEST_PARAM = 10;
static_assert(StopConditionPolicy<MaxGenStopConditionPolicy<double, TEST_PARAM>, double>);
static_assert(StopConditionPolicy<StableAvgStopConditionPolicy<double, static_cast<double>(TEST_PARAM)>, double>);
/* #endregion */

/* #region EvolutionaryAlgorithm */

template <typename TEntity, InitiationPolicy<TEntity> TInitiationPolicy, MutationPolicy<TEntity> TMutationPolicy,
          CrossoverPolicy<TEntity> TCrossoverPolicy, SelectionPolicy<TEntity> TSelectionPolicy,
          StopConditionPolicy<TEntity> TStopConditionPolicy>
class EvolutionaryAlgorithm {
public:
  explicit EvolutionaryAlgorithm(int populationSize) : populationSize_(populationSize) {
    TInitiationPolicy::init(population_, populationSize);
  }

  void run() {
    int generation = 0;
    while (!stopConditionPolicy_.shouldStop(population_, generation)) {
      std::vector<TEntity> newPopulation;

      for (int i = 0; i < populationSize_; ++i) {
        auto [parent1, parent2] = TSelectionPolicy::select(population_);
        TEntity offspring = TCrossoverPolicy::crossover(parent1, parent2);
        newPopulation.push_back(offspring);
      }

      population_ = newPopulation;
      TMutationPolicy::mutate(population_);
      generation++;
    }

    std::cout << "Algorithm stopped after " << generation << " generations.\n";
    printPopulation();
  }

private:
  std::vector<TEntity> population_;
  int populationSize_;
  TStopConditionPolicy stopConditionPolicy_{};

  void printPopulation() const {
    for (const auto &individual : population_) {
      PrintEntity<TEntity>::print(individual);
    }
    std::cout << "\n";
  }
};
/* #endregion */

/* #region TestMethods */
void doubleEvolution() {
  constexpr auto minInit = 0.;
  constexpr auto maxInit = 2137.;
  constexpr auto mutationChance = 0.1;
  constexpr auto mutationIntensity = 10.;
  constexpr auto crossoverWeight = 0.3;
  constexpr auto generationLimit = 10;
  constexpr auto populationSize = 36;

  using Entity = double;

  EvolutionaryAlgorithm<Entity, RandomInitiationPolicy<Entity, minInit, maxInit>,
                        PercentageMutationPolicy<Entity, mutationChance, mutationIntensity>,
                        AverageCrossoverPolicy<Entity, crossoverWeight>, RandomSelectionPolicy<Entity>,
                        MaxGenStopConditionPolicy<Entity, generationLimit>>
      algorithm(populationSize);
  algorithm.run();
}

void vectorDoubleEvolution() {
  constexpr auto dimension = 4;
  constexpr auto minInit = 0.;
  constexpr auto maxInit = 2137.;
  constexpr auto mutationChance = 0.1;
  constexpr auto mutationIntensity = 10.;
  constexpr auto targetLast = 0.001;
  constexpr auto targetFirst = 0.3;
  constexpr auto generationLimit = 10.;
  constexpr auto populationSize = 36;

  using Entity = std::array<double, dimension>;

  EvolutionaryAlgorithm<Entity, LinSpaceInitiationPolicy<Entity, minInit, maxInit>,
                        AbsoluteMutationPolicy<Entity, mutationChance, mutationIntensity>, RandomCrossoverPolicy<Entity>,
                        TargetSelectionPolicy<Entity, targetFirst, targetLast, AbsoluteValueComparator<Entity>>,
                        StableAvgStopConditionPolicy<Entity, generationLimit>>
      algorithm(populationSize);
  algorithm.run();
}

void intEvolution() {
  constexpr auto minInit = 0;
  constexpr auto maxInit = 2137;
  constexpr auto mutationChance = 0.1;
  constexpr auto mutationIntensity = 10;
  constexpr auto crossoverWeight = 0.3;
  constexpr auto generationLimit = 10;
  constexpr auto populationSize = 36;

  using Entity = int;

  EvolutionaryAlgorithm<Entity, RandomInitiationPolicy<Entity, minInit, maxInit>,
                        AbsoluteMutationPolicy<Entity, mutationChance, mutationIntensity>,
                        AverageCrossoverPolicy<Entity, crossoverWeight>, UniqueRandomSelectionPolicy<Entity>,
                        StableAvgStopConditionPolicy<Entity, generationLimit>>
      algorithm(populationSize);
  algorithm.run();
}

void vectorIntEvolution() {
  constexpr auto dimension = 4;
  constexpr auto minInit = 0;
  constexpr auto maxInit = 2137;
  constexpr auto mutationChance = 0.1;
  constexpr auto mutationIntensity = 10;
  constexpr auto crossoverWeight = 0.3;
  constexpr auto generationLimit = 10;
  constexpr auto populationSize = 36;

  using Entity = std::array<int, dimension>;

  EvolutionaryAlgorithm<Entity, RandomInitiationPolicy<Entity, minInit, maxInit>,
                        PercentageMutationPolicy<Entity, mutationChance, mutationIntensity>, RandomCrossoverPolicy<Entity>,
                        UniqueRandomSelectionPolicy<Entity>, MaxGenStopConditionPolicy<Entity, generationLimit>>
      algorithm(populationSize);
  algorithm.run();
}

/* #endregion */

int main() {
  doubleEvolution();
  vectorDoubleEvolution();
  intEvolution();
  vectorIntEvolution();

  return 0;
}
