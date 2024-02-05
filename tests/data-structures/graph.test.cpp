#include <adjacency-list.hpp>
#include <adjacency-matrix.hpp>
#include <gtest/gtest.h>
#include <unordered_set>

class AdjacencyMatrixTest : public ::testing::Test {
public:
  using Vertex = typename graphs::AdjacencyMatrix<int, int>::Vertex;
  using Edge = typename graphs::AdjacencyMatrix<int, int>::Edge;
  graphs::AdjacencyMatrix<int, int> _graph;

  AdjacencyMatrixTest() : _graph{Vector<Vertex>{{1}, {2}, {3}, {4}, {5}}} {};

protected:
  void SetUp() override {
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    Vertex* v1{_graph.find_vertex(1)};
    Vertex* v2{_graph.find_vertex(2)};
    Vertex* v3{_graph.find_vertex(3)};
    Vertex* v4{_graph.find_vertex(4)};
    Vertex* v5{_graph.find_vertex(5)};
    _graph.add_edge(v1, v2, 1);
    _graph.add_edge(v1, v3, 1);
    _graph.add_edge(v1, v4, 1);
    _graph.add_edge(v1, v5, 1);

    _graph.add_edge(v2, v3, 1);
    _graph.add_edge(v3, v4, 1);
    _graph.add_edge(v4, v5, 1);
    _graph.add_edge(v5, v1, 1);
  };
};

TEST_F(AdjacencyMatrixTest, GraphFindVertex) {
  Vertex* v1{_graph.find_vertex(1)};
  Vertex* v2{_graph.find_vertex(2)};
  Vertex* v3{_graph.find_vertex(3)};
  Vertex* v4{_graph.find_vertex(4)};
  Vertex* v5{_graph.find_vertex(5)};

  EXPECT_EQ(v1->data, 1);
  EXPECT_EQ(v2->data, 2);
  EXPECT_EQ(v3->data, 3);
  EXPECT_EQ(v4->data, 4);
  EXPECT_EQ(v5->data, 5);
}

TEST_F(AdjacencyMatrixTest, GraphRemoveVertex) {
  Vertex* v1{_graph.find_vertex(1)};

  EXPECT_EQ(v1->data, 1);

  _graph.remove_vertex(v1);

  EXPECT_EQ(_graph.find_vertex(1), nullptr);
}

TEST_F(AdjacencyMatrixTest, GraphFindEdge) {
  Vertex* v1{_graph.find_vertex(1)};
  Vertex* v2{_graph.find_vertex(2)};
  Vertex* v3{_graph.find_vertex(3)};
  Vertex* v4{_graph.find_vertex(4)};
  Vertex* v5{_graph.find_vertex(5)};

  EXPECT_EQ(_graph.find_edge(v1, v2)->is_empty(), false);
  EXPECT_EQ(_graph.find_edge(v1, v3)->is_empty(), false);
  EXPECT_EQ(_graph.find_edge(v1, v4)->is_empty(), false);
  EXPECT_EQ(_graph.find_edge(v1, v5)->is_empty(), false);
  EXPECT_EQ(_graph.find_edge(v5, v1)->is_empty(), false);

  EXPECT_EQ(_graph.find_edge(v3, v5)->is_empty(), true);
}

TEST_F(AdjacencyMatrixTest, GraphRemoveEdge) {
  Vertex* v1{_graph.find_vertex(1)};
  Vertex* v2{_graph.find_vertex(2)};
  Edge* edge{_graph.find_edge(v1, v2)};

  EXPECT_EQ(edge->is_empty(), false);

  _graph.remove_edge(edge);

  EXPECT_EQ(edge->is_empty(), true);
}

TEST_F(AdjacencyMatrixTest, GraphFindEdgeFrom) {
  Vertex* v1{_graph.find_vertex(1)};
  Vertex* v2{_graph.find_vertex(2)};
  Vertex* v3{_graph.find_vertex(3)};
  Vertex* v4{_graph.find_vertex(4)};
  Vertex* v5{_graph.find_vertex(5)};
  Vector<Edge*> edgesFrom{_graph.edges_from(v1)};

  EXPECT_EQ(edgesFrom.size(), 4);
  EXPECT_EQ(_graph.find_edge(v1, v2), edgesFrom[0]);
  EXPECT_EQ(_graph.find_edge(v1, v3), edgesFrom[1]);
  EXPECT_EQ(_graph.find_edge(v1, v4), edgesFrom[2]);
  EXPECT_EQ(_graph.find_edge(v1, v5), edgesFrom[3]);
}

TEST_F(AdjacencyMatrixTest, GraphFindEdgeTo) {
  Vertex* v1{_graph.find_vertex(1)};
  Vertex* v5{_graph.find_vertex(5)};
  Vector<Edge*> edgesTo{_graph.edges_to(v1)};

  EXPECT_EQ(edgesTo.size(), 1);
  EXPECT_EQ(_graph.find_edge(v5, v1), edgesTo[0]);
}

TEST_F(AdjacencyMatrixTest, GraphBFS) {
  _graph.add_vertex(6);
  _graph.add_vertex(7);
  _graph.add_edge(_graph.find_vertex(5), _graph.find_vertex(6), 1);
  _graph.add_edge(_graph.find_vertex(4), _graph.find_vertex(6), 1);
  _graph.add_edge(_graph.find_vertex(2), _graph.find_vertex(7), 1);

  Vector<int> result{1, 2, 3, 4, 5, 7, 6};
  std::size_t index{};
  _graph.bfs(_graph.find_vertex(1), [&result, &index](Vertex* v) {
    EXPECT_EQ(v->data, result[index]);
    ++index;
  });
}

TEST_F(AdjacencyMatrixTest, GraphBFSAll) {
  _graph.add_vertex(6);
  _graph.add_vertex(7);
  _graph.add_edge(_graph.find_vertex(5), _graph.find_vertex(6), 1);
  _graph.add_edge(_graph.find_vertex(4), _graph.find_vertex(6), 1);
  _graph.add_edge(_graph.find_vertex(2), _graph.find_vertex(7), 1);

  std::unordered_set<int> result{1, 2, 3, 4, 5, 7, 6};
  std::size_t size{result.size()};

  _graph.bfs([&result, &size](Vertex* v) {
    result.erase(v->data);
    --size;
  });
  EXPECT_EQ(result.size(), 0);
  EXPECT_EQ(result.size(), size);
}

TEST_F(AdjacencyMatrixTest, GraphDFS) {
  _graph.add_vertex(6);
  _graph.add_edge(_graph.find_vertex(5), _graph.find_vertex(6), 1);
  _graph.add_edge(_graph.find_vertex(4), _graph.find_vertex(6), 1);

  Vector<int> result{1, 2, 3, 4, 5, 6};
  std::size_t index{};
  _graph.dfs(_graph.find_vertex(1), [&result, &index](Vertex* v) {
    EXPECT_EQ(v->data, result[index]);
    ++index;
  });
}

TEST_F(AdjacencyMatrixTest, GraphDFSAll) {
  _graph.add_vertex(6);
  _graph.add_edge(_graph.find_vertex(5), _graph.find_vertex(6), 1);
  _graph.add_edge(_graph.find_vertex(4), _graph.find_vertex(6), 1);

  std::unordered_set<int> result{1, 2, 3, 4, 5, 6};
  std::size_t size{result.size()};

  _graph.dfs([&result, &size](Vertex* v) {
    result.erase(v->data);
    --size;
  });
  EXPECT_EQ(result.size(), 0);
  EXPECT_EQ(result.size(), size);
}

TEST_F(AdjacencyMatrixTest, GraphTranspose) {
  _graph.add_vertex(6);
  _graph.add_vertex(7);
  _graph.add_edge(_graph.find_vertex(5), _graph.find_vertex(6), 1);
  _graph.add_edge(_graph.find_vertex(4), _graph.find_vertex(6), 1);
  _graph.add_edge(_graph.find_vertex(2), _graph.find_vertex(7), 1);

  graphs::AdjacencyMatrix transposed{_graph.get_transpose()};

  Vertex* v1{transposed.find_vertex(1)};
  Vertex* v2{transposed.find_vertex(2)};
  Vertex* v3{transposed.find_vertex(3)};
  Vertex* v4{transposed.find_vertex(4)};
  Vertex* v6{transposed.find_vertex(6)};
  Vertex* v7{transposed.find_vertex(7)};

  EXPECT_EQ(transposed.has_edge(v2, v1), true);
  EXPECT_EQ(transposed.has_edge(v1, v2), false);

  EXPECT_EQ(transposed.has_edge(v3, v1), true);
  EXPECT_EQ(transposed.has_edge(v1, v3), false);

  EXPECT_EQ(transposed.has_edge(v4, v1), true);
  EXPECT_EQ(transposed.has_edge(v1, v4), false);

  EXPECT_EQ(transposed.has_edge(v7, v2), true);
  EXPECT_EQ(transposed.has_edge(v2, v7), false);

  EXPECT_EQ(transposed.has_edge(v6, v4), true);
  EXPECT_EQ(transposed.has_edge(v4, v6), false);
}

class AdjacencyListTest : public ::testing::Test {
public:
  using Vertex = typename graphs::AdjacencyList<int, int>::Vertex;
  using Edge = typename graphs::AdjacencyList<int, int>::Edge;
  graphs::AdjacencyList<int, int> _graph{};

  AdjacencyListTest() = default;

protected:
  void SetUp() override {
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    Vertex* v1{_graph.add_vertex(1)};
    Vertex* v2{_graph.add_vertex(2)};
    Vertex* v3{_graph.add_vertex(3)};
    Vertex* v4{_graph.add_vertex(4)};
    Vertex* v5{_graph.add_vertex(5)};

    _graph.add_edge(v1, v2, 1);
    _graph.add_edge(v1, v3, 1);
    _graph.add_edge(v1, v4, 1);
    _graph.add_edge(v1, v5, 1);

    _graph.add_edge(v2, v3, 1);
    _graph.add_edge(v3, v4, 1);
    _graph.add_edge(v4, v5, 1);
    _graph.add_edge(v5, v1, 1);
  };
};

TEST_F(AdjacencyListTest, GraphFindVertex) {
  Vertex* v1{_graph.find_vertex(1)};
  Vertex* v2{_graph.find_vertex(2)};
  Vertex* v3{_graph.find_vertex(3)};
  Vertex* v4{_graph.find_vertex(4)};
  Vertex* v5{_graph.find_vertex(5)};

  EXPECT_EQ(v1->data, 1);
  EXPECT_EQ(v2->data, 2);
  EXPECT_EQ(v3->data, 3);
  EXPECT_EQ(v4->data, 4);
  EXPECT_EQ(v5->data, 5);
}

TEST_F(AdjacencyListTest, GraphRemoveVertex) {
  Vertex* v1{_graph.find_vertex(1)};

  EXPECT_EQ(v1->data, 1);

  _graph.remove_vertex(v1);

  EXPECT_EQ(_graph.find_vertex(1), nullptr);
}

TEST_F(AdjacencyListTest, GraphFindEdge) {
  Vertex* v1{_graph.find_vertex(1)};
  Vertex* v2{_graph.find_vertex(2)};
  Vertex* v3{_graph.find_vertex(3)};
  Vertex* v4{_graph.find_vertex(4)};
  Vertex* v5{_graph.find_vertex(5)};

  EXPECT_EQ(_graph.find_edge(v1, v2)->is_empty(), false);
  EXPECT_EQ(_graph.find_edge(v1, v3)->is_empty(), false);
  EXPECT_EQ(_graph.find_edge(v1, v4)->is_empty(), false);
  EXPECT_EQ(_graph.find_edge(v1, v5)->is_empty(), false);
  EXPECT_EQ(_graph.find_edge(v5, v1)->is_empty(), false);

  EXPECT_EQ(_graph.find_edge(v3, v5), nullptr);
}

TEST_F(AdjacencyListTest, GraphRemoveEdge) {
  Vertex* v1{_graph.find_vertex(1)};
  Vertex* v2{_graph.find_vertex(2)};
  Edge* edge{_graph.find_edge(v1, v2)};

  EXPECT_EQ(edge->is_empty(), false);

  _graph.remove_edge(edge);

  EXPECT_EQ(_graph.find_edge(v1, v2), nullptr);
}

TEST_F(AdjacencyListTest, GraphFindEdgeFrom) {
  Vertex* v1{_graph.find_vertex(1)};
  Vertex* v2{_graph.find_vertex(2)};
  Vertex* v3{_graph.find_vertex(3)};
  Vertex* v4{_graph.find_vertex(4)};
  Vertex* v5{_graph.find_vertex(5)};
  Vector<Edge*> edgesFrom{_graph.edges_from(v1)};

  EXPECT_EQ(edgesFrom.size(), 4);
  EXPECT_EQ(_graph.find_edge(v1, v2), edgesFrom[0]);
  EXPECT_EQ(_graph.find_edge(v1, v3), edgesFrom[1]);
  EXPECT_EQ(_graph.find_edge(v1, v4), edgesFrom[2]);
  EXPECT_EQ(_graph.find_edge(v1, v5), edgesFrom[3]);
}

TEST_F(AdjacencyListTest, GraphFindEdgeTo) {
  Vertex* v1{_graph.find_vertex(1)};
  Vertex* v5{_graph.find_vertex(5)};
  Vector<Edge*> edgesTo{_graph.edges_to(v1)};

  EXPECT_EQ(edgesTo.size(), 1);
  EXPECT_EQ(_graph.find_edge(v5, v1), edgesTo[0]);
}

TEST_F(AdjacencyListTest, GraphBFS) {
  _graph.add_vertex(6);
  _graph.add_vertex(7);
  _graph.add_edge(_graph.find_vertex(5), _graph.find_vertex(6), 1);
  _graph.add_edge(_graph.find_vertex(4), _graph.find_vertex(6), 1);
  _graph.add_edge(_graph.find_vertex(2), _graph.find_vertex(7), 1);

  Vector<int> result{1, 2, 3, 4, 5, 7, 6};
  std::size_t index{};
  _graph.bfs(_graph.find_vertex(1), [&result, &index](Vertex* v) {
    EXPECT_EQ(v->data, result[index]);
    ++index;
  });
}

TEST_F(AdjacencyListTest, GraphBFSAll) {
  _graph.add_vertex(6);
  _graph.add_vertex(7);
  _graph.add_edge(_graph.find_vertex(5), _graph.find_vertex(6), 1);
  _graph.add_edge(_graph.find_vertex(4), _graph.find_vertex(6), 1);
  _graph.add_edge(_graph.find_vertex(2), _graph.find_vertex(7), 1);

  std::unordered_set<int> result{1, 2, 3, 4, 5, 7, 6};
  std::size_t size{result.size()};

  _graph.bfs([&result, &size](Vertex* v) {
    result.erase(v->data);
    --size;
  });
  EXPECT_EQ(result.size(), 0);
  EXPECT_EQ(result.size(), size);
}

TEST_F(AdjacencyListTest, GraphDFS) {
  _graph.add_vertex(6);
  _graph.add_edge(_graph.find_vertex(5), _graph.find_vertex(6), 1);
  _graph.add_edge(_graph.find_vertex(4), _graph.find_vertex(6), 1);

  Vector<int> result{1, 2, 3, 4, 5, 6};
  std::size_t index{};
  _graph.dfs(_graph.find_vertex(1), [&result, &index](Vertex* v) {
    EXPECT_EQ(v->data, result[index]);
    ++index;
  });
}

TEST_F(AdjacencyListTest, GraphDFSAll) {
  _graph.add_vertex(6);
  _graph.add_edge(_graph.find_vertex(5), _graph.find_vertex(6), 1);
  _graph.add_edge(_graph.find_vertex(4), _graph.find_vertex(6), 1);

  std::unordered_set<int> result{1, 2, 3, 4, 5, 6};
  std::size_t size{result.size()};

  _graph.dfs([&result, &size](Vertex* v) {
    result.erase(v->data);
    --size;
  });
  EXPECT_EQ(result.size(), 0);
  EXPECT_EQ(result.size(), size);
}

TEST_F(AdjacencyListTest, GraphTranspose) {
  _graph.add_vertex(6);
  _graph.add_vertex(7);
  _graph.add_edge(_graph.find_vertex(5), _graph.find_vertex(6), 1);
  _graph.add_edge(_graph.find_vertex(4), _graph.find_vertex(6), 1);
  _graph.add_edge(_graph.find_vertex(2), _graph.find_vertex(7), 1);

  graphs::AdjacencyList transposed{_graph.get_transpose()};

  Vertex* v1{transposed.find_vertex(1)};
  Vertex* v2{transposed.find_vertex(2)};
  Vertex* v3{transposed.find_vertex(3)};
  Vertex* v4{transposed.find_vertex(4)};
  Vertex* v6{transposed.find_vertex(6)};
  Vertex* v7{transposed.find_vertex(7)};

  EXPECT_EQ(transposed.has_edge(v2, v1), true);
  EXPECT_EQ(transposed.has_edge(v1, v2), false);

  EXPECT_EQ(transposed.has_edge(v3, v1), true);
  EXPECT_EQ(transposed.has_edge(v1, v3), false);

  EXPECT_EQ(transposed.has_edge(v4, v1), true);
  EXPECT_EQ(transposed.has_edge(v1, v4), false);

  EXPECT_EQ(transposed.has_edge(v7, v2), true);
  EXPECT_EQ(transposed.has_edge(v2, v7), false);

  EXPECT_EQ(transposed.has_edge(v6, v4), true);
  EXPECT_EQ(transposed.has_edge(v4, v6), false);
}