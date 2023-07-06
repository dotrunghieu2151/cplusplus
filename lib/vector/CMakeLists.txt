target_sources(myLib
  PRIVATE
    vector.hpp
    vector.cpp
)

target_include_directories(myLib PUBLIC ${CMAKE_CURRENT_LIST_DIR})