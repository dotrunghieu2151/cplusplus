#include <algorithm>
#include <functional>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;
constexpr char deliminator{'|'};
constexpr char escapeChar{'/'};

class Solution {
public:
  /*
   * @param strs: a list of strings
   * @return: encodes a list of strings to a single string.
   */
  string encode(vector<string>& strs) {
    string result{};
    for (string& str : strs) {
      for (char c : str) {
        if (c != deliminator) {
          result += c;
        } else {
          result += escapeChar;
          result += c;
        }
      }
      result += deliminator;
    }
    return result;
  }

  /*
   * @param str: A string
   * @return: decodes a single string to a list of strings
   */
  vector<string> decode(string& str) {
    vector<string> result{};
    string word{};
    for (int i{}; i < str.size(); ++i) {
      char c{str[i]};
      if (c == deliminator) {
        result.push_back(word);
        word.clear();
      } else if (c == escapeChar) {
        word += str[i + 1];
        ++i;
      } else {
        word += c;
      }
    }
    return result;
  }
};