target_sources(myLib 
  PRIVATE
    monster-generator.hpp
    monster-generator.cpp
)
target_include_directories(myLib PUBLIC ${CMAKE_CURRENT_LIST_DIR})