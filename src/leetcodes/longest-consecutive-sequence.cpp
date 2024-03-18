#include <algorithm>
#include <functional>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;

class Solution {
public:
  int longestConsecutive(vector<int>& nums) {
    if (!nums.size()) {
      return 0;
    }
    int maxLength{};
    int currentLength{1};
    sort(nums.begin(), nums.end());

    for (size_t i{1}; i < nums.size(); ++i) {
      if (nums[i] - 1 == nums[i - 1]) {
        ++currentLength;
      } else if (nums[i] == nums[i - 1]) {
        continue;
      } else {
        maxLength = max(maxLength, currentLength);
        currentLength = 1;
      }
    }
    maxLength = max(maxLength, currentLength);
    return maxLength;
  }
};

int main() {
  vector<int> input{0, 3, 7, 2, 5, 8, 4, 6, 0, 1};

  Solution sol{};
  sol.longestConsecutive(input);
}