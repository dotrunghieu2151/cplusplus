#pragma once

#include <vector.hpp>

namespace graphs {

template <typename VertexData, typename Weight> class Graph {
public:
  struct Vertex {
    VertexData data{};

    virtual ~Vertex() = default;
  };

  struct Edge {
    Weight weight{};

    virtual ~Edge() = default;

    virtual bool is_empty() const = 0;
  };

  virtual ~Graph() = default;
  /**
   * Method that adds a vertex with value to the graph
   * @param value Title of the vertex
   * @return Reference to the created vertex object
   */
  virtual Vertex* add_vertex(const VertexData& value) = 0;

  virtual Vertex* add_vertex(VertexData&& value) = 0;

  /**
   * Method that removes a vertex by reference
   * @param v Reference to a vertex object
   */
  virtual void remove_vertex(Vertex* vertex) = 0;

  /**
   * Method that adds an edge to the graph
   * @param from Vertex
   * @param to Vertex
   * @param weight Weight of the edge
   * @return Reference to the created edge object
   */
  virtual Edge* add_edge(Vertex* from, Vertex* to, Weight weight) = 0;

  /**
   * Method that removes an edge
   * @param e Pointer to an edge object
   */
  virtual void remove_edge(Edge* edge) = 0;

  /**
   * Method that returns a collection of edges that are going from vertex v
   * @param v Vertex
   * @return a collection of edges that are going from vertex v
   */
  virtual Vector<Edge*> edges_from(Vertex* vertex) const = 0;

  /**
   * Method that returns a collection of edges that are going to vertex v
   * @param v Vertex
   * @return a collection of edges that are going to vertex v
   */
  virtual Vector<Edge*> edges_to(Vertex* vertex) const = 0;

  /**
   * Method that finds any vertex with the specified value
   * @param value Input value
   * @return Reference to an vertex object
   */
  virtual Vertex* find_vertex(const VertexData& data) const = 0;

  /**
   * Method that finds any edge with specified values in the source and target
   * vertices
   * @param from_value Value
   * @param to_value Value
   * @return Reference to an edge object
   */
  virtual Edge* find_edge(Vertex* from, Vertex* to) const = 0;

  /**
   * Method that determine whether there exists a directed edge from v to u
   * @param v Vertex
   * @param u Vertex
   * @return Boolean result
   */
  virtual bool has_edge(Vertex* from, Vertex* to) const = 0;
};

} // namespace graphs