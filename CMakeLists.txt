cmake_minimum_required(VERSION 3.18)
project(testfollycmakevcpkg)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_library(mylibrary  mylibrary/MyClass.cpp mylibrary/MyClass.h)

# 命名约定：folly_<组件>_demo = 演示程序；gtest_<主题>_test / <api>_test = 断言测试
add_executable(folly_futures_uri_demo src/main.cpp)
#add_executable(RpcClient RpcClient.cpp)
#add_executable(RpcServer RpcServer.cpp)

add_executable(gtest_basics_test test/gtest_basics.cpp)
add_executable(gtest_make_pair_test test/gtest_make_pair.cpp)


find_package(folly CONFIG REQUIRED)
if(NOT TARGET folly::folly AND TARGET Folly::folly)
    add_library(folly::folly ALIAS Folly::folly)
endif()
add_executable(folly_concurrent_hash_map_demo TestFolly/ConcurrentHashMap.cpp)
#add_executable(AtomicHashMapTest TestFolly/AtomicHashMapTest.cpp)# undefined reference

# folly::AtomicIntrusiveHashMap 不存在于开源 folly（调查报告见 README.md 对应章节），
# 实测其真实存在的家族成员 AtomicIntrusiveLinkedList
add_executable(atomic_intrusive_linked_list_test TestFolly/AtomicIntrusiveLinkedListTest.cpp)

# folly 并发 API 演示（对照 JUC/libcds，说明见 TestFolly/FollyConcurrency.md）
add_executable(folly_concurrent_api_demo TestFolly/FollyConcurrencyDemo.cpp)


#add_executable(FBStringBenchmark TestFolly/FBStringBenchmark.cpp)# TestFolly/FBStringBenchmark.cpp:81:10: fatal error: 'folly/test/FBStringTestBenchmarks.cpp.h' file not found
#target_link_libraries(FBStringBenchmark PRIVATE folly::folly)


target_link_libraries(folly_concurrent_hash_map_demo PRIVATE folly::folly)
target_link_libraries(atomic_intrusive_linked_list_test PRIVATE folly::folly)
target_link_libraries(folly_concurrent_api_demo PRIVATE folly::folly)
#target_link_libraries(AtomicHashMapTest PRIVATE folly::folly)



target_link_libraries(folly_futures_uri_demo PRIVATE folly::folly)
add_executable(folly_atomic_linked_list_demo src/folly_atomic_linked_list_demo.cpp)
target_link_libraries(folly_atomic_linked_list_demo PRIVATE folly::folly)
add_executable(folly_mpmc_queue_demo src/folly_mpmc_queue_demo.cpp)
target_link_libraries(folly_mpmc_queue_demo PRIVATE folly::folly)
add_executable(folly_distributed_mutex_demo src/folly_distributed_mutex_demo.cpp)
target_link_libraries(folly_distributed_mutex_demo PRIVATE folly::folly)

enable_testing()

find_package(GTest CONFIG REQUIRED)
if(NOT TARGET gtest::gtest AND TARGET GTest::gtest)
    add_library(gtest::gtest ALIAS GTest::gtest)
endif()
# 测试自带 main，绝不能写成 GTEST_API_ int main（该宏在 MSVC 共享 gtest 下展开为
# dllimport，定义它直接 C2491 编译错误；GTEST_API_ 只属于 gtest 自己的 gtest_main.cc）
target_link_libraries(gtest_basics_test PRIVATE GTest::gtest)
target_link_libraries(gtest_make_pair_test PRIVATE GTest::gtest)

add_test(GtestBasics gtest_basics_test)
add_test(GtestMakePair gtest_make_pair_test)
add_test(AtomicIntrusiveLinkedList atomic_intrusive_linked_list_test)


#find_package(wangle REQUIRED)
#target_link_libraries(RpcClient PRIVATE wangle::wangle)
#target_link_libraries(RpcServer PRIVATE wangle::wangle)
