target_sources(myLib
  PRIVATE
    heap.hpp
)

target_include_directories(myLib PUBLIC ${CMAKE_CURRENT_LIST_DIR})