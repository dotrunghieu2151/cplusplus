target_sources(myLib
  PRIVATE
    algorithm.hpp
    algorithm.cpp
)

target_include_directories(myLib PUBLIC ${CMAKE_CURRENT_LIST_DIR})