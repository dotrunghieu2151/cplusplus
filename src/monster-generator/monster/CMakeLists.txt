target_sources(myLib 
  PRIVATE
    monster.hpp
    monster.cpp
)
target_include_directories(myLib PUBLIC ${CMAKE_CURRENT_LIST_DIR})