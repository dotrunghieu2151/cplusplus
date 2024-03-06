add_executable(stack-application.test stack-application.test.cpp)

target_link_libraries(stack-application.test
  PRIVATE 
    GTest::gtest_main
    myLib
)

add_test(stack-application-gtest stack-application.test)


add_executable(backtracking.test backtracking.test.cpp)

target_link_libraries(backtracking.test
  PRIVATE 
    GTest::gtest_main
    myLib
)

add_test(sort-gtest sort.test)

add_executable(sort.test sort.test.cpp)

target_link_libraries(sort.test
  PRIVATE 
    GTest::gtest_main
    myLib
)

add_test(sort-gtest sort.test)


add_executable(load-balancer.test load-balancer.test.cpp)

target_link_libraries(load-balancer.test
  PRIVATE 
    GTest::gtest_main
    myLib
)

add_test(load-balancer-gtest load-balancer.test)