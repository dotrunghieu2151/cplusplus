add_executable(stack-application.test stack-application.test.cpp)

target_link_libraries(stack-application.test
  PRIVATE 
    GTest::gtest_main
    myLib
)

add_test(stack-application-gtest stack-application.test)