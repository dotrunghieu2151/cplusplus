target_sources(myLib 
  PRIVATE
    log.cpp
    log.hpp
)
target_include_directories(myLib PUBLIC ${CMAKE_CURRENT_LIST_DIR})