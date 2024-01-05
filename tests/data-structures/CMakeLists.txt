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


add_executable(b-tree.test b-tree.test.cpp)

target_link_libraries(b-tree.test
  PRIVATE 
    GTest::gtest_main
    myLib
)

add_test(b-tree-gtest b-tree.test)


add_executable(static-circular-buffer.test static-circular-buffer.test.cpp)

target_link_libraries(static-circular-buffer.test
  PRIVATE 
    GTest::gtest_main
    myLib
)

add_test(static-circular-buffer-gtest static-circular-buffer.test)

add_executable(b-plus-tree.test b-plus-tree.test.cpp)

target_link_libraries(b-plus-tree.test
  PRIVATE 
    GTest::gtest_main
    myLib
)

add_test(b-plus-tree-gtest b-plus-tree.test)

add_executable(avl-tree.test avl-tree.test.cpp)

target_link_libraries(avl-tree.test
  PRIVATE 
    GTest::gtest_main
    myLib
)

add_test(avl-tree-gtest avl-tree.test)


add_executable(rbt.test rbt.test.cpp)

target_link_libraries(rbt.test
  PRIVATE 
    GTest::gtest_main
    myLib
)

add_test(rbt-gtest rbt.test)

# add_executable(avl-rbt-perf.test avl-rbt-perf.test.cpp)

# target_link_libraries(avl-rbt-perf.test
#   PRIVATE 
#     GTest::gtest_main
#     myLib
# )

# add_test(avl-rbt-perf-gtest avl-rbt-perf.test)

add_executable(splay-tree.test splay-tree.test.cpp)

target_link_libraries(splay-tree.test
  PRIVATE 
    GTest::gtest_main
    myLib
)

add_test(trie-gtest trie.test)

add_executable(trie.test trie.test.cpp)

target_link_libraries(trie.test
  PRIVATE 
    GTest::gtest_main
    myLib
)

add_test(trie-gtest trie.test)