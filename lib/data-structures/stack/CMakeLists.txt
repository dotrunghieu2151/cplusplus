target_sources(myLib
  PRIVATE
    stack.hpp
)

target_include_directories(myLib PUBLIC ${CMAKE_CURRENT_LIST_DIR})