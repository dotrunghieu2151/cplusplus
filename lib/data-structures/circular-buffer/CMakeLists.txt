target_sources(myLib
  PRIVATE
    circular-buffer.hpp
    static-circular-buffer.hpp
)

target_include_directories(myLib PUBLIC ${CMAKE_CURRENT_LIST_DIR})