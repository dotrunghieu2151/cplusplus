target_sources(myLib
  PRIVATE
    structs.hpp
    enums.hpp
)

target_include_directories(myLib PUBLIC ${CMAKE_CURRENT_LIST_DIR})