cmake_minimum_required(VERSION 3.15)
project(testfollycmakevcpkg)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_library(mylibrary  mylibrary/MyClass.cpp mylibrary/MyClass.h)

add_executable(testfollycmakevcpkg src/main.cpp)
#add_executable(RpcClient RpcClient.cpp)
#add_executable(RpcServer RpcServer.cpp)

add_executable(gtestmethods test/gtestmain.cpp)
add_executable(gtestpairTest test/gtestpair.cpp)


find_package(folly CONFIG REQUIRED)
add_executable(TestConcurrentHashMap TestFolly/ConcurrentHashMap.cpp)
#add_executable(AtomicHashMapTest TestFolly/AtomicHashMapTest.cpp)# undefined reference


#add_executable(FBStringBenchmark TestFolly/FBStringBenchmark.cpp)# TestFolly/FBStringBenchmark.cpp:81:10: fatal error: 'folly/test/FBStringTestBenchmarks.cpp.h' file not found
#target_link_libraries(FBStringBenchmark PRIVATE Folly::folly)


target_link_libraries(TestConcurrentHashMap PRIVATE Folly::folly)
#target_link_libraries(AtomicHashMapTest PRIVATE Folly::folly)



target_link_libraries(testfollycmakevcpkg PRIVATE Folly::folly)
add_executable(f_a_list src/f_a_list.cpp)
target_link_libraries(f_a_list PRIVATE Folly::folly)
add_executable(clh_example src/clh_example.cpp)
target_link_libraries(clh_example PRIVATE Folly::folly)
add_executable(folly_d_mutex src/folly_d_mutex.cpp)
target_link_libraries(folly_d_mutex PRIVATE Folly::folly)

enable_testing()

find_package(GTest CONFIG REQUIRED)
target_link_libraries(gtestmethods PRIVATE GTest::gtest)
target_link_libraries(gtestpairTest PRIVATE GTest::gtest)

add_test(AllTestsInMain gtestmethods)


#find_package(wangle REQUIRED)
#target_link_libraries(RpcClient PRIVATE wangle::wangle)
#target_link_libraries(RpcServer PRIVATE wangle::wangle)
