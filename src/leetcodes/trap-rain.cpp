#include <algorithm>
#include <functional>
#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;

class Solution {
public:
  int trap(vector<int>& height) {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    if (!height.size()) {
      return 0;
    }
    int water{};
    size_t l{};
    size_t r{height.size() - 1};
    int maxL{height[l]};
    int maxR{height[r]};

    while (l < r) {
      if (maxL < maxR) {
        ++l;
        maxL = max(maxL, height[l]);
        water += maxL - height[l];
      } else {
        --r;
        maxR = max(maxR, height[r]);
        water += maxR - height[r];
      }
    }
    return water;
  }
};

int main() {
  vector<int> input{4, 2, 0, 3, 2, 5};

  Solution sol{};
  sol.trap(input);
}