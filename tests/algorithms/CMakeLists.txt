add_executable(stack-application stack-application.test.cpp)

target_link_libraries(stack-application 
  PRIVATE 
    GTest::gtest_main
    myLib
)

add_test(stack-application-gtest stack-application)