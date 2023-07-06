target_sources(myLib 
  PRIVATE
    helpers.hpp
    utilities.hpp
)
target_include_directories(myLib PUBLIC ${CMAKE_CURRENT_LIST_DIR})