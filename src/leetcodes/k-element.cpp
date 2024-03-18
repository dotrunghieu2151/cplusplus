#include <functional>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;
using Pair = pair<int, int>;
bool compare(const Pair& p1, const Pair& p2) { return p1.first > p2.first; }

class Solution {
public:
  vector<int> topKFrequent(vector<int>& nums, int k) {
    unordered_map<int, int> map{};
    priority_queue<Pair, vector<Pair>, function<bool(const Pair&, const Pair&)>>
        pq(compare);
    for (int num : nums) {
      ++map[num];
    }
    for (auto& mapPair : map) {
      pq.push({mapPair.second, mapPair.first});
      if (pq.size() > k) {
        pq.pop();
      }
    }

    vector<int> result{};
    while (pq.size()) {
      result.push_back(pq.top().second);
      pq.pop();
    }
    return result;
  }
};

int main() {
  vector<int> input{1, 1, 1, 2, 2, 3};

  Solution sol{};
  sol.topKFrequent(input, 2);
}