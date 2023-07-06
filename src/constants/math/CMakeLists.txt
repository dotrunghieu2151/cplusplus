target_sources(myLib 
  PRIVATE
    math.constant.hpp
)
target_include_directories(myLib PUBLIC ${CMAKE_CURRENT_LIST_DIR})