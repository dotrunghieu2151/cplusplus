target_sources(myLib 
  PRIVATE
    math.hpp
    math.cpp
)
target_include_directories(myLib PUBLIC ${CMAKE_CURRENT_LIST_DIR})