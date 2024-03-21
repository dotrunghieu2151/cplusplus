target_sources(myLib
  PRIVATE
    min-stack.hpp
)

target_include_directories(myLib PUBLIC ${CMAKE_CURRENT_LIST_DIR})