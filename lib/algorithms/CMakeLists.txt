target_sources(myLib
  PRIVATE
    algorithm.hpp
    algorithm.cpp
    load-balancer.hpp
)

target_include_directories(myLib PUBLIC ${CMAKE_CURRENT_LIST_DIR})