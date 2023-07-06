#pragma once
#include <string>
#include <string_view>

class Monster
{
public:
  enum class Type
  {
    dragon,
    goblin,
    orge,
    orc,
    skeleton,
    troll,
    vampire,
    zombie,
    max_monster_types
  };

private:
  Type m_type{};
  std::string m_name{};
  std::string m_roar{};
  int m_hitPoints{};

public:
  Monster(Type type, std::string_view name, std::string_view roar, int hitPoints);

  std::string_view getTypeString() const;

  void print() const;
};