#include <chrono>
#include <random>

namespace Random
{
  using std::mt19937;

  mt19937 init()
  {
    std::random_device randomDevice;
    unsigned int systemTicks = static_cast<unsigned int>(
        std::chrono::steady_clock::now().time_since_epoch().count());
    std::seed_seq seedSequence{
        systemTicks,
        randomDevice(),
        randomDevice(),
        randomDevice(),
        randomDevice(),
        randomDevice(),
        randomDevice(),
        randomDevice(),
        randomDevice()};
    return mt19937{seedSequence};
  };

  mt19937 rng{init()};

  int uniformRand(int min, int max)
  {
    std::uniform_int_distribution randomNumberDistribution{min, max};
    return randomNumberDistribution(rng);
  };
}