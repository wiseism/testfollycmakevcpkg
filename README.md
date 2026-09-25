# README

make_pair

下面按字母顺序介绍了一系列Folly组件，另外附有每个组件的简短描述。
Arena.h，ThreadCachedArena.h
内存分配的简单地方：多次内存分配同时被释放。使用线程版本。
AtomicHashMap.h，AtomicHashArray.h
高性能的原子哈希图，采用几乎无锁的操作。
Benchmark.h
用于代码基准测试的小型框架。客户端代码注册基准测试，可选情况下使用一个变量来规定基准测试的范围（迭代和工作集大小等）。框架运行基准测试（受制于命令行标记），生成带计时信息的格式化输出。
Bits.h
各种位处理实用组件，针对速度而优化。
Bits.h
位变换函数，使用统一接口包装ffsl(l)图元。
ConcurrentSkipList.h
实现了用证实正确的可扩展并发跳跃表（Provably Correct Scalable Concurrent Skip List）描述的结构，这种跳跃表由Herlihy及其他人共同开发。
Conv.h
各种数据转换例程（尤其是to和from字符串），针对速度和安全进行了优化。
DiscriminatedPtr.h
类似boost::variant，但完全局限于指针。使用指针中最高位、未使用的16位作为鉴别器。所以sizeof(DiscriminatedPtr<int, string, Widget>) == sizeof(void*)。
dynamic.h
动态类型对象，创建时关注JSON对象。
Endian.h
Endian转换图元。
Escape.h
以C方式转义字符串。
eventfd.h
针对eventfd系统调用的包装器。
FBString.h
嵌入式实现std::string，进行了诸多优化。
FBVector.h
基本上嵌入式实现std::vector，进行了诸多优化。
Foreach.h
伪语句（作为宏语句来实现），用于迭代。
Format.h
Python式样的格式化实用组件。
GroupVarint.h
针对32位值的Group Varint编码。
Hash.h
各种流行的哈希函数实现。
Histogram.h
一个简单的类，用于收集直方图数据。
IntrusiveList.h
方便类型定义，用于使用boost::intrusive_list。
json.h
JSON序列化器和反序列化器。使用dynamic.h。
Likely.h
针对__builtin_expect的包装器。
Malloc.h
内存分配助手，尤其是使用jemalloc时。
MapUtil.h
一种助手，用于查找联合容器中的项目（比如std::map和std::unordered_map）。
PackedSyncPtr.h
一种高度专业化的数据结构，含有指针、1位旋转锁和15位整数，它们都在一个64位单词中。
Preprocessor.h
不好但又必不可少的组件。
PrettyPrint.h
针对数字的美化打印组件，用于添加所用单元的后缀：字节（kb、MB等）、度量单位后缀（k、M和G等）以及时间（s、ms、us和ns等）。
ProducerConsumerQueue.h
无锁单读取器单写入器队列。
Random.h
只定义了一个函数：randomNumberSeed()。
Range.h
Boost式样的范围工具和StringPiece专门化。
RWSpinLock.h
快速而紧凑的读取器/写入器旋转锁。
ScopeGuard.h
老式ScopeGuard用语的C++11版本。
SmallLocks.h
非常小的旋转锁（1字节和1位）。
small_vector.h
一种向量，含有小缓冲器方面的优化和可选的嵌入式PicoSpinLock。
sorted_vector_types.h
类似std::map的集合体，但是作为排序向量来实现。
StlAllocator.h
标准模板库（STL分配器），包装简单的分配/取消分配接口。
String.h
连接folly::fbstring和std::string的字符串实用组件。
Synchronized.h
高级同步库。
System.h
解码和errno实用组件。
ThreadCachedInt.h
使用线程缓存的高性能原子增量。
ThreadLocal.h
经过改进的线程本地存储，用于存储非平凡类型。
TimeoutQueue.h
按项目设定超时的队列。
Traits.h
类型特性，补充了在标准的C++11头<traits>中定义的那些类型特性。
Unicode.h
定义了codePointToUtf8函数。


folly库解读
c++11特性：
auto
static_assert
nullptr
initializer list

https://www.zhihu.com/column/c_1327270420144570368
字符串https://zhuanlan.zhihu.com/p/348614098

NoBarrier_AtomicIncrement
NoBarrier_Load
https://www.cnblogs.com/gistao/p/4583734.html

- folly
- wangle
- gtest

找到菜单 File->Settings->Build,Execution->Toolchains 取消
```shell script
-DCMAKE-BUILD-TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake
```


### folly
目前mac编译成功，目标Windows，Linux

`vcpkg install folly`

folly版本
2019.10.21.00

https://github.com/facebook/folly/blob/v2019.10.21.00/folly/docs/Overview.md

- AtomicHashMap

[使用clion+cmake+vcpkg的开发现代的c++跨平台程序](https://www.cnblogs.com/Leo_wl/p/11555600.html)


### code

std::move
在C++11中，标准库在<utility>中提供了一个有用的函数std::move，std::move并不能移动任何东西，它唯一的功能是将一个左值强制转化为右值引用，继而可以通过右值引用使用该值，以用于移动语义。从实现上讲，std::move基本等同于一个类型转换：static_cast<T&&>(lvalue);

[c++ 之 std::move 原理实现与用法总结](https://blog.csdn.net/p942005405/article/details/84644069)

[使用cmake和gtest来规划单元测试](https://menrfa.wordpress.com/2012/02/09/%E4%BD%BF%E7%94%A8cmake%E5%92%8Cgtest%E6%9D%A5%E8%A7%84%E5%88%92%E5%8D%95%E5%85%83%E6%B5%8B%E8%AF%95/)


```shell script

====================[ Build | sample1_unittest | Debug ]========================
/usr/local/bin/cmake --build /Users/ibqo/CLionProjects/testfollycmakevcpkg/cmake-build-debug --target sample1_unittest -- -j 4
[ 50%] Linking CXX executable sample1_unittest
Undefined symbols for architecture x86_64:
  "IsPrime(int)", referenced from:
      (anonymous namespace)::IsPrimeTest_Negative_Test::TestBody() in sample1_unittest.cc.o
      (anonymous namespace)::IsPrimeTest_Trivial_Test::TestBody() in sample1_unittest.cc.o
      (anonymous namespace)::IsPrimeTest_Positive_Test::TestBody() in sample1_unittest.cc.o
  "Factorial(int)", referenced from:
      (anonymous namespace)::FactorialTest_Negative_Test::TestBody() in sample1_unittest.cc.o
      (anonymous namespace)::FactorialTest_Zero_Test::TestBody() in sample1_unittest.cc.o
      (anonymous namespace)::FactorialTest_Positive_Test::TestBody() in sample1_unittest.cc.o
ld: symbol(s) not found for architecture x86_64
clang: error: linker command failed with exit code 1 (use -v to see invocation)
make[3]: *** [sample1_unittest] Error 1
make[2]: *** [CMakeFiles/sample1_unittest.dir/all] Error 2
make[1]: *** [CMakeFiles/sample1_unittest.dir/rule] Error 2
make: *** [sample1_unittest] Error 2

```

[Mac下Clion编译错误：Undefined symbols for architecture x86_64](https://blog.csdn.net/iamoldpan/article/details/78258107)

cmake如何自动运行gtest测试

[建立简单的带单元测试的CMake项目](https://www.blurredcode.com/2019/03/452/)

make test
运行测试

[cmake处理多源文件目录的方法](https://www.cnblogs.com/fnlingnzb-learner/p/7202236.html）

子文件夹
CMakeLists.txt add_execute失败


[wangle examples](https://github.com/facebook/wangle/tree/master/wangle/example)


- RpcServer
- RpcClient
使用了thrift folley wangle的例子

```shell script
vcpkg install thrift
Your feedback is important to improve Vcpkg! Please take 3 minutes to complete our survey by running: vcpkg contact --survey
The following packages will be built and installed:
    thrift[core]:x64-osx
Starting package 1/1: thrift:x64-osx
Building package thrift[core]:x64-osx...
-- Using cached /Users/ibqo/vcpkg/downloads/apache-thrift-acdd4226c210336e9e15eb812e5932a645fcd5ce.tar.gz
-- Using source at /Users/ibqo/vcpkg/buildtrees/thrift/src/a645fcd5ce-616856a2c8
-- Configuring x64-osx-dbg
-- Configuring x64-osx-rel
-- Building x64-osx-dbg
CMake Error at scripts/cmake/vcpkg_execute_build_process.cmake:136 (message):
    Command failed: /Users/ibqo/vcpkg/downloads/tools/cmake-3.14.0-osx/cmake-3.14.0-Darwin-x86_64/CMake.app/Contents/bin/cmake --build . --config Debug --target install -- -v
    Working Directory: /Users/ibqo/vcpkg/buildtrees/thrift/x64-osx-dbg
    See logs for more information:
      /Users/ibqo/vcpkg/buildtrees/thrift/install-x64-osx-dbg-out.log

Call Stack (most recent call first):
  scripts/cmake/vcpkg_build_cmake.cmake:91 (vcpkg_execute_build_process)
  scripts/cmake/vcpkg_install_cmake.cmake:24 (vcpkg_build_cmake)
  ports/thrift/portfile.cmake:45 (vcpkg_install_cmake)
  scripts/ports.cmake:94 (include)


Error: Building package thrift:x64-osx failed with: BUILD_FAILED
Please ensure you're using the latest portfiles with `.\vcpkg update`, then
submit an issue at https://github.com/Microsoft/vcpkg/issues including:
  Package: thrift:x64-osx
  Vcpkg version: 2019.09.12-unknownhash

Additionally, attach any relevant sections from the log files above.
```

### cpp
c++的class不能用public修饰

#pragma once
相当于只引用一次

移除gtest sample

先熟悉gtest，在引用gtest测试folly


## gtest
gtest例子
cmake:
add_executable()

enable_testing()

find_package(GTest CONFIG REQUIRED)
target_link_libraries(gtest_basics_test PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

源代码：
```c++
#include "gtest/gtest.h"
#define TEST(test_suite_name, test_name) GTEST_TEST(test_suite_name, test_name)
#define EXPECT_EQ(val1, val2)

    testing::InitGoogleTest(&argc, argv);
```

namespace testing

## clh

你尝试包含的是一个 Folly 内部头文件：

Cpp
#include <folly/synchronization/detail/CLHQueue.h>
而这个文件并不是 Folly 的公开 API，它属于 Folly 源码中的 内部实现细节（detail namespace），因此在你通过包管理器（如 vcpkg、conan 或系统安装）安装的 Folly 库中，并不会被导出。

✅ 正确做法：使用 Folly 提供的公共接口
🚫 不要直接包含 detail 头文件！
Folly 的官方文档和社区建议是：不要依赖 detail/ 中的任何类或函数，它们可能会在版本更新时被删除或重构。

🔍 查看是否真的有 CLHQueue 可用？
截至当前（2025年6月），Folly 并没有提供公开的 CLH 队列接口。也就是说：

CLHQueue.h 是内部测试或实现文件
它并不属于 Folly 的 public headers
所以你不能在你的项目中使用它

folly::MPMCQueue<T>	多生产者多消费者无锁队列
folly::ProducerConsumerQueue<T>	单生产者单消费者队列（更快）
folly::Synchronized<T>	封装 std::mutex + RAII，简化同步

https://fossies.org/dox/folly-v2025.06.02.00/


https://fossies.org/dox/folly-v2025.06.02.00/classes.html

## 结论

**`folly::AtomicIntrusiveHashMap` 在开源 folly 中不存在**（当前版本和 main 分支均无此容器），
它属于"看着像 folly 风格、但实际查无此 API"的名字（和本仓库之前踩过的 `folly::AtomicLinkedList`
模板名坑是同类问题）。

## 证据链（全部实测）

1. **本机安装包**：`vcpkg_installed/x64-windows/include/folly/` 下全量检索
   `grep -rl "IntrusiveHashMap"` → **0 命中**；`folly/container/` 目录里只有
   `IntrusiveHeap.h`、`IntrusiveList.h`，无任何 Atomic* 哈希表。
2. **vcpkg 解包源码树**（`buildtrees/folly/src/5.03.31.00-*/`，比安装目录更全）：
   全树 grep `AtomicIntrusiveHashMap` → **0 命中**。
3. **GitHub 上游 main 分支**（`gh api repos/facebook/folly/contents/folly/container`）：
   无 `AtomicIntrusiveHashMap.h`；`folly/docs/` 里也没有对应文档（只有 `AtomicHashMap.md`）。
4. **GitHub 全仓库 commit 搜索**（search/commits?q=repo:facebook/folly+AtomicIntrusiveHashMap
   及更宽泛的 IntrusiveHashMap）→ **0 条**。
5. **Meta 工程博客** engineering.fb.com 站内搜索 → "Nothing Found"。
   （网络上流传的"Introducing AtomicIntrusiveHashMap"记忆实际对应的是开源过的
   **AtomicUnorderedMap** 与 **F14** 系列，不是这个名字。）

> 教训延续：凡是从 AI 回答/二手博客里来的 folly API 名，先
> `find vcpkg_installed/x64-windows/include/folly -iname "*关键词*"` 验一下再写代码。

## 真实存在、功能上最接近的东西

| API | 头文件 | 是什么 |
|---|---|---|
| `AtomicIntrusiveLinkedListHook<T>` + `AtomicIntrusiveLinkedList<T, &T::hook>` | `folly/AtomicIntrusiveLinkedList.h` | 无锁（CAS 头插）侵入式**单向**链表；"Atomic + Intrusive" 家族的正主 |
| `folly::IntrusiveList<T, Hook1, Hook2...>` | `folly/container/IntrusiveList.h` | 非线程安全、侵入式双向链表（boost Intrusive 风格），LRU 常用 |
| `folly::ConcurrentHashMap` | `folly/concurrency/ConcurrentHashMap.h` | 线程安全哈希表（非侵入式），本仓库 `TestFolly/ConcurrentHashMap.cpp` 已测 |
| `folly::AtomicHashMap` / `AtomicUnorderedMap` | `folly/AtomicHashMap.h` 等 | 预分配容量、写多读少的一次性哈希表（Windows 下 AtomicHashMapTest 曾链接失败，已注释） |
| `folly::EvictingCacheMap` | `folly/container/EvictingCacheMap.h` | 带 LRU 逐出的非线程安全 map |

如果目标是"无锁 + 侵入式 + 哈希查找"，开源 folly 的组合拳是：
**分桶数组（自己写）+ 每桶一个 `AtomicIntrusiveLinkedList`**（这正是该链表注释里的经典用法：
任务队列/回收栈/与 F14 配合做对象池）。

## AtomicIntrusiveLinkedList API 速查（本次实测通过）

```cpp
class Node {
 public:
  folly::AtomicIntrusiveLinkedListHook<Node> hook;  // 侵入点：只有一个 next 指针
  int id;
};

folly::AtomicIntrusiveLinkedList<Node, &Node::hook> list;  // 模板参数：成员指针

list.insertHead(&a);        // CAS 头插；返回 true 表示插入后链表只有它自己
                            // 前置要求：a.hook.next == nullptr（重复插入会 assert）
list.unsafeHead();          // 当前头指针（并发修改下可能已失效，故名 unsafe）
list.empty();               // 快照判断
list.sweep(cb);             // 原子摘链，按【尾→头】（=插入顺序 FIFO）回调；循环直到清空
list.sweepOnce(cb);         // 只摘一轮，返回是否非空；并发消费者轮询用这个
list.reverseSweep(cb);      // 原子摘一轮，按【头→尾】LIFO 回调
list.spliceAll();           // 元素整体挪给一个新链表（返回右值），原链表变空
list.reverseSweepAndAssign(std::move(other), cb); // 换入新内容+换出旧内容回调，
                                                  // move 赋值操作符的安全替代
```

关键约定与坑：

1. **析构时链表必须是空的**（debug 下 assert），所以程序退出前要 sweep 干净——
   这也是它没有 move 赋值操作符的原因。
2. 元素被摘链后 `hook.next` 自动复位为 `nullptr`，可以再次 `insertHead`。
3. 容器**完全不分配内存**：节点生命周期自己管（对象池/静态数组最合适）。
4. `sweep` 回调里不要再去操作同一个链表（无意义且绕）。

## 测试文件

`TestFolly/AtomicIntrusiveLinkedListTest.cpp`（目标名 `atomic_intrusive_linked_list_test`）：
基本顺序 / reverseSweep LIFO 与钩子复位 / 4线程×10000 并发插入+并发 sweepOnce 不丢不重 / spliceAll。
运行方式：

```
cmake --build --preset vcpkg-debug --target atomic_intrusive_linked_list_test
build-vcpkg\Debug\atomic_intrusive_linked_list_test.exe
```

实测结果（2026-09-25）：`pass=13 fail=0`。
