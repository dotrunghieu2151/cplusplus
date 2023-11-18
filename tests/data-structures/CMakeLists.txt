# tests
add_executable(vector.test vector.test.cpp)
add_executable(list.test list.test.cpp)
add_executable(array.test array.test.cpp)
add_executable(circular-buffer.test circular-buffer.test.cpp)
add_executable(simple-deque.test simple-deque.test.cpp)
add_executable(deque.test deque.test.cpp)
add_executable(stack.test stack.test.cpp)
add_executable(heap.test heap.test.cpp)
add_executable(priority-queue.test priority-queue.test.cpp)

target_link_libraries(vector.test 
  PRIVATE 
    GTest::gtest_main
    myLib
)

target_link_libraries(list.test 
  PRIVATE 
    GTest::gtest_main
    myLib
)

target_link_libraries(array.test
  PRIVATE 
    GTest::gtest_main
    myLib
)

target_link_libraries(circular-buffer.test
  PRIVATE 
    GTest::gtest_main
    myLib
)

target_link_libraries(simple-deque.test
  PRIVATE 
    GTest::gtest_main
    myLib
    Tracy::TracyClient
)

target_link_libraries(deque.test
  PRIVATE 
    GTest::gtest_main
    myLib
    Tracy::TracyClient
)

target_link_libraries(stack.test
  PRIVATE 
    GTest::gtest_main
    myLib
    Tracy::TracyClient
)

target_link_libraries(heap.test
  PRIVATE 
    GTest::gtest_main
    myLib
    Tracy::TracyClient
)

target_link_libraries(priority-queue.test
  PRIVATE 
    GTest::gtest_main
    myLib
    Tracy::TracyClient
)

add_test(vector-gtest vector.test)
add_test(circular-buffer-gtest circular-buffer.test)
add_test(list_gtest list.test)
add_test(array_gtest array.test)
add_test(simple_deque_gtest simple-deque.test)
add_test(deque_gtest deque.test)
add_test(stack_gtest stack.test)
add_test(heap_gtest heap.test)
add_test(priority_queue_gtest priority-queue.test)

add_executable(tree.test tree.test.cpp)

target_link_libraries(tree.test
  PRIVATE 
    GTest::gtest_main
    myLib
)

add_test(tree-gtest tree.test)