#pragma once

#include <algorithm>
#include <limits>
#include <random.hpp>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector.hpp>

namespace load_balancer {
class RoundRobinLB {
  Vector<std::string> _servers{};
  int _currentIndex{};

  void add_server(std::string_view name) {
    _servers.push_back(std::string{name});
  }

  void remove_server(std::string_view name) {
    Vector<std::string>::Iterator server{
        std::find(_servers.begin(), _servers.end(), name)};
    if (server != _servers.end()) {
      _servers.erase(server);
    }
  }

  const std::string* get_server() {
    if (_servers.empty()) {
      return nullptr;
    }
    const std::string* nextServer{&_servers[(std::size_t)_currentIndex]};
    _currentIndex = ((std::size_t)_currentIndex + 1) % _servers.size();
    return nextServer;
  }
};

class WeightedRoundRobinLB {
  Vector<std::string> _servers{};
  Vector<int> _culminativeWeight{};
  int _totalWeight{};

  void add_server(std::string_view name, int weight) {
    _servers.push_back(std::string{name});
    _totalWeight += weight;

    if (!_culminativeWeight.empty()) {
      _culminativeWeight.push_back(_culminativeWeight.back() + weight);
    } else {
      _culminativeWeight.push_back(weight);
    }
  }

  void remove_server(std::string_view name) {
    auto iter{std::find(_servers.begin(), _servers.end(), name)};
    if (iter != _servers.end()) {
      std::size_t index{(std::size_t)std::distance(_servers.begin(), iter)};
      _servers.erase(index);
      int weightOfServer{index > 0 ? _culminativeWeight[index] -
                                         _culminativeWeight[index - 1]
                                   : _culminativeWeight[index]};
      _totalWeight -= weightOfServer;

      // recompute culminative weight sum
      _culminativeWeight.erase(index);
      for (std::size_t i{index}; i < _culminativeWeight.size(); ++i) {
        _culminativeWeight[index] -= weightOfServer;
      }
    }
  }

  const std::string* get_server() {
    int randomValue{Random::uniformRand(0, _totalWeight)};

    for (std::size_t i{}; i < _culminativeWeight.size(); ++i) {
      if (randomValue < _culminativeWeight[i]) {
        return &_servers[i];
      }
    }
    return nullptr;
  }
};

class IpHashLB {
  Vector<std::string> _servers{};
  std::hash<std::string_view> hasheFn{};

  void add_server(std::string_view name) {
    _servers.push_back(std::string{name});
  }

  void remove_server(std::string_view name) {
    auto iter{std::find(_servers.begin(), _servers.end(), name)};
    if (iter != _servers.end()) {
      _servers.erase(iter);
    }
  }

  const std::string* get_server(std::string_view ip) {
    if (_servers.empty()) {
      return nullptr;
    }
    int hashed = hasheFn(ip);

    return &_servers[(std::size_t)hashed % _servers.size()];
  }
};

class LeastConnectionLB {
  std::unordered_map<std::string, int> _servers{};

  void add_server(std::string_view name) {
    _servers.emplace(std::make_pair(std::string{name}, 0));
  }

  void remove_server(const std::string& name) { _servers.erase(name); }

  const std::string* get_server() {
    std::pair<const std::string, int>* kvPair{nullptr};
    int minConnectionCount{std::numeric_limits<int>::max()};
    for (auto& pair : _servers) {
      if (pair.second < minConnectionCount) {
        kvPair = &pair;
        minConnectionCount = pair.second;
      }
    }
    if (kvPair) {
      kvPair->second += 1;
      return &kvPair->first;
    }
    return nullptr;
  }
};

// similar to LeastConnectionLB but instead of storing connection num in
// _servers we store last response time
class LeastResponseTimeLB {};

class ConsistentHashRing {
  int _replicas{};
  std::unordered_map<int, std::string> _ring{};
  std::set<int> _sortedKeys{};
  std::hash<std::string_view> _hasheFn{};

  ConsistentHashRing(int duplicatesPerNode) : _replicas{duplicatesPerNode} {}

  void add_server(const std::string& name) {
    for (int i{}; i < _replicas; ++i) {
      int hashed = _hasheFn(name + '_' + std::to_string(i));
      _ring.insert({hashed, name});
      _sortedKeys.insert(hashed);
    }
  }

  void remove_server(const std::string& name) {
    for (int i{}; i < _replicas; ++i) {
      int hashed = _hasheFn(name + '_' + std::to_string(i));
      _ring.erase(hashed);
      _sortedKeys.erase(hashed);
    }
  }

  std::string* get_server(const std::string& key) {
    if (_ring.empty()) {
      return nullptr;
    }

    int hashed = _hasheFn(key);

    auto closedServerOnRing{_sortedKeys.lower_bound(hashed)};

    if (closedServerOnRing == _sortedKeys.end()) {
      closedServerOnRing = _sortedKeys.begin();
    }

    return &_ring.at(*closedServerOnRing);
  }
};
} // namespace load_balancer