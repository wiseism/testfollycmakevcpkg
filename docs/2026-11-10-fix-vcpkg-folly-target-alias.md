# 2026-11-10 folly::folly target not found 修复

## 现象

vcpkg 升级到 2026.10.16 后依赖安装成功（folly 2026.02.23 #2, gtest 1.18.0），但项目 configure 的 Generate 阶段报：

```
CMake Error at CMakeLists.txt:34 (target_link_libraries):
  Target "f_a_list" links to: folly::folly
  but the target was not found.
CMake Generate step failed.
```

## 根因

新版 folly 的 vcpkg port 在包配置导出的目标命名与旧的 namespace alias 兼容性有差异，
`target_link_libraries(... folly::folly)` 在 generate 阶段找不到该 ALIAS。

## 修复

`find_package(folly CONFIG REQUIRED)` 之后按需补建 ALIAS（仅当 `folly::folly` 不存在且 `folly` 存在时）：

```cmake
if(NOT TARGET folly::folly AND TARGET folly)
  add_library(folly::folly ALIAS folly)
endif()
```


## 三次修正

从 vcpkg 输出确认新 port 导出的 CMake 目标为 `Folly::folly`（注：903 个 target）与
`GTest::gtest`。将 CMakeLists 中所有 `folly::folly`→`Folly::folly`、`gtest::gtest`→`GTest::gtest`，
并移除上一轮加入的临时 alias 补丁。


## 四次修正

vcpkg 缓存命中后编译推进到项目源码：`src/f_a_list.cpp:55` 裸用 `AtomicLinkedList`（folly 新版不再
泄漏到全局作用域，且其为模板类），改为 `folly::AtomicLinkedList<int> a;`（该变量未使用，仅示例代码）。

## 验证记录
| Run | 结果 |
|---|---|
| 42844406776 | ✅ success |
