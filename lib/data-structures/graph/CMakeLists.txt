target_sources(myLib
  PRIVATE
    graph.hpp
    adjacency-list.hpp
    adjacency-matrix.hpp
)

target_include_directories(myLib PUBLIC ${CMAKE_CURRENT_LIST_DIR})