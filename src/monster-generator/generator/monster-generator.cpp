#include <array>
#include <monster-generator.hpp>
#include <monster.hpp>
#include <random.hpp>

namespace MonsterGenerator {
Monster generate() {
  using Type = Monster::Type;
  using Random::uniformRand;

  static constexpr std::array roarRndPool{"Rarr", "Aha", "lewd", "Tada",
                                          "dafa"};
  static constexpr std::array nameRndPool{"Jeff", "John", "David", "Stephen",
                                          "Joe"};

  auto rndType{static_cast<Type>(
      uniformRand(0, static_cast<int>(Type::max_monster_types) - 1))};
  return Monster{rndType,
                 nameRndPool.at(static_cast<std::size_t>(
                     uniformRand(0, nameRndPool.size() - 1))),
                 roarRndPool.at(static_cast<std::size_t>(
                     uniformRand(0, roarRndPool.size() - 1))),
                 uniformRand(0, 1000)};
}
} // namespace MonsterGenerator