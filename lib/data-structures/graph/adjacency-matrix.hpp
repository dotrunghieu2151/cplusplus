#pragma once

#include <functional>
#include <graph.hpp>
#include <queue.hpp>
#include <stack.hpp>
#include <vector.hpp>

namespace graphs {
// good when the number of node is low
// good when the graph is static (no mutation)
// good cache locality
// not memory efficient if graph is large
template <typename V, typename E = int> class AdjacencyMatrix {

public:
  class Vertex {
    friend class AdjacencyMatrix;

  public:
    V data{};

    Vertex(const V& value) : data{value} {};
    Vertex(V&& value) : data{std::move(value)} {};

  private:
    std::size_t _index{};
  };

  class Edge {
    friend class AdjacencyMatrix;

  public:
    E weight{};

    bool is_empty() const { return _to == -1; }

  private:
    int _to{};

    Edge(E inputWeight, int to) : weight{inputWeight}, _to{to} {};
  };

  Vector<Vertex> _vertices{};
  Vector<Vector<Edge>> _edges{};

public:
  AdjacencyMatrix() = default;

  AdjacencyMatrix(const Vector<Vertex>& vertices)
      : _vertices{vertices},
        _edges(_vertices.size(), Vector(_vertices.size(), Edge{0, -1})) {
    for (std::size_t i{}; i < _vertices.size(); ++i) {
      _vertices[i]._index = i;
    }
  }

  AdjacencyMatrix(Vector<Vertex>&& vertices)
      : _vertices{std::move(vertices)},
        _edges(_vertices.size(), Vector<Edge>(_vertices.size(), Edge{0, -1})) {
    for (std::size_t i{}; i < _vertices.size(); ++i) {
      _vertices[i]._index = i;
    }
  }

  /**
   * Method that finds vertex
   * @param value value
   * @return Vertex*
   */
  Vertex* find_vertex(const V& value) {
    for (std::size_t i{}; i < _vertices.size(); ++i) {
      if (_vertices[i].data == value) {
        return &_vertices[i];
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

    _vertices.push_back(Vertex{value, _vertices.size()});

    Vector<Edge> v(_vertices.size(), Edge{0, -1});

    _edges.push_back(v);

    for (std::size_t i{}; i < _vertices.size() - 1; ++i) {
      _edges[i].push_back(Edge{0, -1});
    }

    return &_vertices.back();
  }

  // invalidates all pointers, references
  Vertex* add_vertex(V&& value) {
    Vertex* vertex{find_vertex(value)};
    if (vertex) {
      return vertex;
    }
    Vertex tempV{std::move(value)};
    tempV._index = _vertices.size();
    _vertices.push_back(std::move(tempV));

    Vector<Edge> v(_vertices.size(), Edge{0, -1});

    _edges.push_back(v);

    for (std::size_t i{}; i < _vertices.size() - 1; ++i) {
      _edges[i].push_back(Edge{0, -1});
    }

    return &_vertices.back();
  }

  /**
   * Overriding of the Method that removes a vertex by reference
   * @param v Reference to a vertex object
   */
  void remove_vertex(Vertex* v) {
    if (!v) {
      return;
    }
    for (std::size_t i{v->_index + 1}; i < _vertices.size(); ++i) {
      --_vertices[i]._index;
    }

    _vertices.erase(v->_index);

    _edges.erase(v->_index);

    for (std::size_t i{}; i < _vertices.size(); ++i) {
      _edges[i].erase(v->_index);
    }
  }

  /**
   * Overriding of the Method that adds an edge to the graph
   * @param from Vertex
   * @param to Vertex
   * @param weight Weight of the edge
   * @return Reference to the created edge object
   */
  Edge* add_edge(Vertex* fromVertex, Vertex* toVertex, E weight) {

    _edges[fromVertex->_index][toVertex->_index] = Edge{weight, 1};

    return &_edges[fromVertex->_index][toVertex->_index];
  }

  /**
   * Method that removes an edge
   * @param e Pointer to an edge object
   */
  void remove_edge(Edge* edge) {
    edge->weight = 0;
    edge->_to = -1;
  };

  /**
   * Method that finds any edge with specified values in the source and target
   * vertices
   * @param from_value Value
   * @param to_value Value
   * @return Reference to an edge object
   */
  Edge* find_edge(Vertex* from, Vertex* to) {
    return &_edges[from->_index][to->_index];
  };

  /**
   * Method that determine whether there exists a directed edge from v to u
   * @param v Vertex
   * @param u Vertex
   * @return Boolean result
   */
  bool has_edge(Vertex* from, Vertex* to) {
    Edge* edge{find_edge(from, to)};
    return !edge->is_empty();
  };

  /**
   * Method that returns a collection of edges that are going from vertex v
   * @param v Vertex
   * @return a collection of edges that are going from vertex v
   */
  Vector<Edge*> edges_from(Vertex* vertex) {
    Vector<Edge*> result{};
    for (Edge& edge : _edges.at(vertex->_index)) {
      if (!edge.is_empty()) {
        result.push_back(&edge);
      }
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
    for (Vector<Edge>& edges : _edges) {
      if (!edges.at(vertex->_index).is_empty()) {
        result.push_back(&edges.at(vertex->_index));
      }
    }
    return result;
  };

  /**
   * traverse all vertices
   */

  void dfs(const std::function<void(Vertex*)>& fn) {
    std::unordered_map<std::size_t, bool> visitedVertices{};
    for (Vertex& v : _vertices) {
      if (visitedVertices.find(v._index) == visitedVertices.end()) {
        dfs(&v, fn, visitedVertices);
      }
    }
  }

  /**
   * traverse only vertices that are reachable from a given vertex
   */
  void dfs(Vertex* vertex, const std::function<void(Vertex*)>& fn) {
    std::unordered_map<std::size_t, bool> visitedVertices{};
    return dfs(vertex, fn, visitedVertices);
  }
  void dfs(Vertex* vertex, const std::function<void(Vertex*)>& fn,
           std::unordered_map<std::size_t, bool>& visitedVertices) {
    Stack<std::size_t> stack{};
    stack.push(vertex->_index);

    while (stack.size()) {
      std::size_t index{stack.pop()};
      Vertex& v{_vertices.at(index)};
      if (visitedVertices.find(index) == visitedVertices.end()) {
        fn(&v);
        visitedVertices.insert({index, true});
      }

      Vector<Edge>& edgeList{_edges.at(index)};
      for (std::size_t i{edgeList.size()}; i != 0; --i) {
        if (!edgeList[i - 1].is_empty() &&
            visitedVertices.find(i - 1) == visitedVertices.end()) {
          stack.push(i - 1);
        }
      }
    }
  }

  /**
   * traverse all vertices
   */
  void bfs(const std::function<void(Vertex*)>& fn) {
    std::unordered_map<std::size_t, bool> visitedVertices{};
    for (Vertex& v : _vertices) {
      if (visitedVertices.find(v._index) == visitedVertices.end()) {
        bfs(&v, fn, visitedVertices);
      }
    }
  }

  /**
   * traverse only vertices that are reachable from a given vertex
   */
  void bfs(Vertex* vertex, const std::function<void(Vertex*)>& fn) {
    std::unordered_map<std::size_t, bool> visitedVertices{};
    return bfs(vertex, fn, visitedVertices);
  }

  void bfs(Vertex* vertex, const std::function<void(Vertex*)>& fn,
           std::unordered_map<std::size_t, bool>& visitedVertices) {
    Queue<std::size_t> queue{};
    queue.push_back(vertex->_index);
    visitedVertices.insert({vertex->_index, true});

    while (queue.size()) {
      std::size_t index{queue.pop_front()};
      Vertex& v{_vertices.at(index)};
      fn(&v);

      Vector<Edge>& edgeList{_edges.at(index)};
      for (std::size_t i{}; i < edgeList.size(); ++i) {
        if (!edgeList[i].is_empty() &&
            visitedVertices.find(i) == visitedVertices.end()) {
          visitedVertices.insert({i, true});
          queue.push_back(i);
        }
      }
    }
  }

  /**
   * reverse (or transpose) the current graph
   */
  AdjacencyMatrix get_transpose() {
    AdjacencyMatrix graph{_vertices};
    for (std::size_t i{}; i < _vertices.size(); ++i) {
      for (std::size_t j{}; j < _edges[i].size(); ++j) {
        if (!_edges[i][j].is_empty()) {
          graph.add_edge(&graph._vertices[j], &graph._vertices[i],
                         _edges[i][j].weight);
        }
      }
    }
    return graph;
  }
};
} // namespace graphs