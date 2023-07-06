add_executable(bin main.cpp)
add_executable(monster monster.cpp)
add_executable(vector vector-test.cpp)

target_link_libraries(bin myLib)
target_link_libraries(monster myLib)
target_link_libraries(vector myLib)

add_subdirectory(constants/math)
add_subdirectory(compound-types)
add_subdirectory(monster-generator)