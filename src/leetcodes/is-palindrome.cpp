#include <algorithm>
#include <functional>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;

bool is_alphanumeric(char c) {
  // check 0 <= c - '0' < 10 || 0 <= c - 'a' < 26 || 0 <= c - 'A' < 26
  return isalnum(c);
}

char to_lowercase(char c) {
  // return c + 32;
  return tolower(c);
}
class Solution {
public:
  bool isPalindrome(string s) {
    for (size_t i{}; i < s.size(); ++i) {
      if (!is_alphanumeric(s[i])) {
        s.erase(i, 1);
        --i;
      } else {
        s[i] = to_lowercase(s[i]);
      }
    }

    for (size_t i{}; i < s.size(); ++i) {
      if (s[i] != s[s.size() - 1 - i]) {
        return false;
      }
    }
    return true;
  }

  bool is_palindrome_fast(string s) {
    int l{};
    int r = s.size() - 1;

    while (true) {
      while (l < s.size() - 1 && !is_alphanumeric(s[l])) {
        ++l;
      }

      while (r > 0 && !is_alphanumeric(s[r])) {
        --r;
      }

      if (l > r) {
        return true;
      }

      if (to_lowercase(s[l]) != to_lowercase(s[r])) {
        return false;
      } else {
        ++l;
        --r;
      }
    }
  }
};

int main() {
  string input{"a.b,."};

  Solution sol{};
  sol.isPalindrome(input);
}