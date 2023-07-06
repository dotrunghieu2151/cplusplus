target_sources(myLib 
  PRIVATE
    timer.hpp
    timer.cpp
)
target_include_directories(myLib PUBLIC ${CMAKE_CURRENT_LIST_DIR})