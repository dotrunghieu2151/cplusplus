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

target_link_libraries(list-test 
  PRIVATE 
    GTest::gtest_main
    myLib
)
add_test(list_gtest list-test)