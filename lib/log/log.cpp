#include <iostream>
#include <log.hpp>

using namespace std;

namespace Log
{
  void log(const string message)
  {
    cout << message << '\n';
  }
}
