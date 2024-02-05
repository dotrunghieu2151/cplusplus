#pragma once

#include <functional>
#include <graph.hpp>
#include <list.hpp>
#include <queue.hpp>
#include <stack.hpp>
#include <unordered_map>
#include <unordered_set>
#include <vector.hpp>

namespace graphs {
// not cache locality friendly
// memory efficient
// faster insert time compared to AdjacencyMatrix
// need good hash function
template <typename V, typename E = int> class AdjacencyList {

public:
  class Vertex {
    friend class AdjacencyList;

  public:
    V data{};

    Vertex(const V& value) : data{value} {};
    Vertex(V&& value) : data{std::move(value)} {};
  };

  struct Edge {
    friend class AdjacencyList;

  public:
    E weight{};

    bool is_empty() const { return !_to; }

  private:
    Vertex* _to{nullptr};
    Vertex* _from{nullptr};

    Edge(E inputWeight, Vertex* to, Vertex* from)
        : weight{inputWeight}, _to{to}, _from{from} {};
  };

  std::unordered_set<Vertex*> _vertices{};
  std::unordered_map<Vertex*, List<Edge>> _edge_map{};

public:
  AdjacencyList() = default;

  AdjacencyList(const std::unordered_set<Vertex*>& vertices) {
    for (Vertex* v : vertices) {
      add_vertex(v->data);
    }
  };

  AdjacencyList(const AdjacencyList& other) {
    std::unordered_map<Vertex*, Vertex*> vertexMap{};
    // clone graph while mapping clone vertices to original ones
    for (Vertex* v : other._vertices) {
      Vertex* clonedV{add_vertex(v->data)};
      vertexMap.insert({v, clonedV});
    }

    for (Vertex* v : other._vertices) {
      List<Edge>& edgeList{other._edge_map.at(v)};
      for (Edge& edge : edgeList) {
        add_edge(vertexMap.at(v), vertexMap.at(edge->_to), edge.weight);
      }
    }
  }

  AdjacencyList& operator=(const AdjacencyList& other) {
    AdjacencyList temp{other};
    other.swap(*this);
    return *this;
  }

  AdjacencyList(AdjacencyList&& other)
      : _vertices{std::move(other._vertices)},
        _edge_map{std::move(other._edge_map)} {}

  AdjacencyList& operator=(AdjacencyList&& other) {
    AdjacencyList temp{std::move(other)};
    other.swap(*this);
    return *this;
  }

  ~AdjacencyList() {
    for (Vertex* v : _vertices) {
      delete v;
    }
  };

  void swap(AdjacencyList& other) {
    using std::swap;
    swap(_vertices, other._vertices);
    swap(_edge_map, other._edge_map);
  }

  friend void swap(AdjacencyList& g1, AdjacencyList& g2) { g1.swap(g2); }

  /**
   * Method that finds vertex
   * @param value value
   * @return Vertex*
   */
  Vertex* find_vertex(const V& value) {
    for (Vertex* v : _vertices) {
      if (v->data == value) {
        return v;
      }
    }
    return nullptr;
  }

  /**
   * Overriding of the Method that adds a vertex with value to the graph
   * @param value Title of the vertex
   * @return Reference to the created vertex object
   */
  Vertex* add_vertex(const V& value) {
    Vertex* vertex{find_vertex(value)};
    if (vertex) {
      return vertex;
    }

    Vertex* newVertex{new Vertex{value}};
    _vertices.insert(newVertex);

    _edge_map.insert({newVertex, List<Edge>{}});
    return newVertex;
  }

  Vertex* add_vertex(V&& value) {
    Vertex* vertex{find_vertex(value)};
    if (vertex) {
      return vertex;
    }

    Vertex* newVertex{new Vertex{std::move(value)}};
    _vertices.insert(newVertex);

    _edge_map.insert({newVertex, List<Edge>{}});
    return newVertex;
  }

  /**
   * Overriding of the Method that removes a vertex by reference
   * @param v Reference to a vertex object
   */
  void remove_vertex(Vertex* v) {
    if (!v) {
      return;
    }

    _edge_map.erase(v);
    _vertices.erase(v);

    for (auto& keyValue : _edge_map) {
      for (auto iter{keyValue.second.begin()}; iter != keyValue.second.end();
           ++iter) {
        if (iter->_to == v) {
          keyValue.second.erase(iter);
          break;
        }
      }
    }
    delete v;
    return;
  }

  /**
   * Overriding of the Method that adds an edge to the graph
   * @param from Vertex
   * @param to Vertex
   * @param weight Weight of the edge
   * @return Reference to the created edge object
   */
  Edge* add_edge(Vertex* fromVertex, Vertex* toVertex, E weight) {
    List<Edge>& edgeList{_edge_map.at(fromVertex)};

    edgeList.push_back(Edge{weight, toVertex, fromVertex});

    return &edgeList.back();
  }

  /**
   * Method that removes an edge
   * @param e Pointer to an edge object
   */
  void remove_edge(Edge* edge) {
    List<Edge>& edgeList{_edge_map.at(edge->_from)};

    for (auto iter{edgeList.begin()}; iter != edgeList.end(); ++iter) {
      if (iter->_to == edge->_to) {
        edgeList.erase(iter);
        return;
      }
    }
  };

  /**
   * Method that finds any edge with specified values in the source and target
   * vertices
   * @param from_value Value
   * @param to_value Value
   * @return Reference to an edge object
   */
  Edge* find_edge(Vertex* from, Vertex* to) {
    List<Edge>& edgeList{_edge_map.at(from)};

    for (Edge& edge : edgeList) {
      if (edge._to == to) {
        return &edge;
      }
    }
    return nullptr;
  };

  /**
   * Method that determine whether there exists a directed edge from v to u
   * @param v Vertex
   * @param u Vertex
   * @return Boolean result
   */
  bool has_edge(Vertex* from, Vertex* to) { return find_edge(from, to); };

  /**
   * Method that returns a collection of edges that are going from vertex v
   * @param v Vertex
   * @return a collection of edges that are going from vertex v
   */
  Vector<Edge*> edges_from(Vertex* vertex) {
    Vector<Edge*> result{};
    for (Edge& edge : _edge_map.at(vertex)) {
      result.push_back(&edge);
    }
    return result;
  };

  /**
   * Method that returns a collection of edges that are going to vertex v
   * @param v Vertex
   * @return a collection of edges that are going to vertex v
   */
  Vector<Edge*> edges_to(Vertex* vertex) {
    Vector<Edge*> result{};
    for (auto& kvPair : _edge_map) {
      if (kvPair.first == vertex) {
        continue;
      }
      for (Edge& edge : kvPair.second) {
        if (edge._to == vertex) {
          result.push_back(&edge);
          break;
        }
      }
    }
    return result;
  };

  /**
   * traverse all vertices
   */
  void dfs(const std::function<void(Vertex*)>& fn) {
    std::unordered_map<Vertex*, bool> visitedVertices{};
    for (Vertex* v : _vertices) {
      if (visitedVertices.find(v) == visitedVertices.end()) {
        dfs(v, fn, visitedVertices);
      }
    }
  }

  /**
   * traverse only vertices that are reachable from a given vertex
   */
  void dfs(Vertex* vertex, const std::function<void(Vertex*)>& fn) {
    std::unordered_map<Vertex*, bool> visitedVertices{};
    return dfs(vertex, fn, visitedVertices);
  }

  void dfs(Vertex* vertex, const std::function<void(Vertex*)>& fn,
           std::unordered_map<Vertex*, bool>& visitedVertices) {
    Stack<Vertex*> stack{};
    stack.push(vertex);

    while (stack.size()) {
      Vertex* v{stack.pop()};
      if (visitedVertices.find(v) == visitedVertices.end()) {
        fn(v);
        visitedVertices.insert({v, true});
      }

      List<Edge>& edgeList{_edge_map.at(vertex)};
      for (auto iter{edgeList.rbegin()}; iter != edgeList.rend(); ++iter) {
        if (visitedVertices.find(iter->_to) == visitedVertices.end()) {
          stack.push(iter->_to);
        }
      }
    }
    return;
  }

  /**
   * traverse all vertices
   */
  void bfs(const std::function<void(Vertex*)>& fn) {
    std::unordered_map<Vertex*, bool> visitedVertices{};
    for (Vertex* v : _vertices) {
      if (visitedVertices.find(v) == visitedVertices.end()) {
        bfs(v, fn, visitedVertices);
      }
    }
  }

  /**
   * traverse only vertices that are reachable from a given vertex
   */
  void bfs(Vertex* vertex, const std::function<void(Vertex*)>& fn) {
    std::unordered_map<Vertex*, bool> visitedVertices{};
    return bfs(vertex, fn, visitedVertices);
  }

  void bfs(Vertex* vertex, const std::function<void(Vertex*)>& fn,
           std::unordered_map<Vertex*, bool>& visitedVertices) {
    Queue<Vertex*> queue{};
    queue.push_back(vertex);
    visitedVertices.insert({vertex, true});

    while (queue.size()) {
      Vertex* v{queue.pop_front()};
      fn(v);
      List<Edge>& edgeList{_edge_map.at(v)};

      for (Edge& edge : edgeList) {
        if (visitedVertices.find(edge._to) == visitedVertices.end()) {
          visitedVertices.insert({edge._to, true});
          queue.push_back(edge._to);
        }
      }
    }
  }

  /**
   * reverse (or transpose) the current graph
   * (flip the edge direction, i.e (u, v) => (v, u))
   */
  AdjacencyList get_transpose() {
    std::unordered_map<Vertex*, Vertex*> vertexMap{};
    AdjacencyList graph{};
    // clone graph while mapping clone vertices to original ones
    for (Vertex* v : _vertices) {
      Vertex* clonedV{graph.add_vertex(v->data)};
      vertexMap.insert({v, clonedV});
    }

    for (Vertex* v : _vertices) {
      List<Edge>& edgeList{_edge_map.at(v)};
      for (Edge& edge : edgeList) {
        graph.add_edge(vertexMap.at(edge._to), vertexMap.at(v), edge.weight);
      }
    }
    return graph;
  }
};
} // namespace graphs