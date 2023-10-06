include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

# tests
add_executable(list-test list.test.cpp)
add_executable(array-test array.test.cpp)
add_executable(circular-buffer-test circular-buffer.test.cpp)
add_executable(simple-deque-test simple-deque.test.cpp)

target_link_libraries(list-test 
  PRIVATE 
    GTest::gtest_main
    myLib
)

target_link_libraries(array-test
  PRIVATE 
    GTest::gtest_main
    myLib
)

target_link_libraries(circular-buffer-test
  PRIVATE 
    GTest::gtest_main
    myLib
)

target_link_libraries(simple-deque-test
  PRIVATE 
    GTest::gtest_main
    myLib
)

add_test(circular-buffer-gtest circular-buffer-test)
add_test(list_gtest list-test)
add_test(array_gtest array-test)
add_test(simple_deque_gtest simple-deque-test)