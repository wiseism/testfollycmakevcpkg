# folly 并发 API 对照：Java JUC / libcds / folly

日期：2026-09-26 ｜ 环境：folly 2025.03.31.00（vcpkg x64-windows）
演示代码：`TestFolly/FollyConcurrencyDemo.cpp`（目标 `folly_concurrent_api_demo`）

libcds 的定位是"C++ 世界的 JUC"：把 Java 并发包里久经考验的无锁数据结构搬到
C++（并且做得更细，提供 RCU/HP 等多种内存回收策略）。但 folly 里其实也能找到
几乎每一个对应物——而且很多在实现思想上同宗同源（都出自 Michael/Herlihy 系论文）。
下面这张表按 JUC 概念组织。

## 总映射表

| JUC 概念 | libcds 对应 | folly 对应 | 头文件 | 备注 |
|---|---|---|---|---|
| `ConcurrentHashMap` | `skip_list::MSkipList` 模拟 | `folly::ConcurrentHashMap` | `folly/concurrency/ConcurrentHashMap.h` | folly 版读完全无锁，写按桶；内部用 hazard pointer 回收 |
| `ConcurrentSkipListMap` | `skip_list::SkipList` | `folly::ConcurrentSkipList` | `folly/concurrency/ConcurrentSkipList.h` | 预分配容量（风格同 AtomicHashMap），两侧端点 |
| `BlockingQueue`（Array/Linked） | `fs::LifoQueue`、`msqueue` + backoff | `folly::MPMCQueue` | `folly/MPMCQueue.h` | ⚠️ 实测（folly 2025.03）：`write/read` 是**非阻塞**版，满/空直接返回 false 丢元素；阻塞语义要用 `blockingWrite/blockingRead`（挂起到 LIFO 信号量，不是纯自旋） |
| 多锁粒度队列 | `VSQueue` 等 | `folly::LifoSemMPMCQueue` | `folly/MPMCQueue.h` | 可调自旋阈值，性能取向的 MPMCQueue 变体 |
| `ConcurrentLinkedQueue/Deque` | `TreiberStack`、`MSQueue` | `folly::AtomicLinkedList` / `AtomicIntrusiveLinkedList` | `folly/AtomicLinkedList.h`、`folly/AtomicIntrusiveLinkedList.h` | 都是 Treiber 栈家族；JUC 的多节点队列语义在 folly 没有一一对应 |
| `ExecutorService` | ——（自管线程） | `folly::CPUThreadPoolExecutor` | `folly/executors/CPUThreadPoolExecutor.h` | 还有 `IOThreadPoolExecutor`、`UnboundedThreadPoolExecutor` |
| `CompletableFuture` / `allOf` | futurelib 风格（folly 自带） | `folly::Future` + `futures::collectAll` | `folly/futures/Future.h` | `collectAll` 返回 `Try` 向量，单个失败不拖垮整体（≈ allOf+handle 组合） |
| `CyclicBarrier` | —— | `folly::futures::Barrier` | `folly/futures/Barrier.h` | 面向 future 等待值就绪，语义近似 |
| `AtomicReference<Config>` | `cds::atomics` + RCU `TPFuchs` | `folly::atomic_shared_ptr` | `folly/concurrency/AtomicSharedPtr.h` | 读侧无锁 + 快照一致性，RCU 式发布/订阅 |
| ——（GC 白送的安全发布） | HP/Guarded/EBR 全家桶 | `folly::hazptr` / `EpochReclaimer` | `folly/concurrency/HazptrTS.h`、`folly/concurrency/EpochReclaimer.h` | 这是 libcds 与 folly 共同面对、而 JUC 靠 GC 回避的问题 |
| `ReentrantLock`/`LockSupport` | `sync::SpinLock` 等 | `folly::DistributedMutex` | `folly/synchronization/DistributedMutex.h` | 无等待自旋锁，配合 `std::unique_lock` 用（见 `folly_distributed_mutex_demo`） |
| `ReadWriteLock`/`StampedLock` | `sync::RGlock`（读聚合） | `folly::SharedMutex`、`RWSpinLock` | `folly/synchronization/SharedMutex.h` 等 | folly 的锁基本是 boost 移植（`Synchronization.h` 系） |
| `Semaphore` | `sync::Semaphore` | `folly::LifoSemaphore`、`SymmetricSemaphore` | `folly/synchronization/Semaphore*.h` | Windows 推荐 Symmetric 版（无 posix 语义包袱） |
| `Collections.synchronizedMap` | ——（不需要） | `folly::Synchronized<T>` | `folly/Synchronized.h` | 比 JUC 强：可 `withRLock`/`wlock` 细粒度选锁型 |

## 三个世界观差异（选型时最容易踩）

1. **内存回收**：JUC 有 GC，摘下来的节点自动安全；libcds 把选择权给你（Hazard
   Pointer / Guarded / EBR / TPFuchs……）；folly 默认帮你选了 hazard pointer
   （ConcurrentHashMap/SkipList 内部），要手写无锁结构时用 `hazptr` 家族。
2. **阻塞 vs 自旋**：libcds 的队列读空默认自旋 + backoff；folly MPMCQueue 要显式
   选函数——`read()` 非阻塞（空返回 false），`blockingRead()` 才把线程挂到信号量上。
   上下文切换换 CPU 利用率，压测数据别直接对比。这个命名和直觉相反，
   `FollyConcurrencyDemo.cpp` 第 4 节实测踩过：用 `write/read` 当阻塞版，-producer
   满队丢数据、consumer 空队提前退出，`consumedSum` 恒等于 2080（1..64 之和）。
3. **容量语义**：folly 的 `AtomicHashMap`/`ConcurrentSkipList` 都要求预分配容量
   （换无 resize 的读锁路径）；JUC/libcds 没有这个约束。

## 本仓库已有代码索引（全部实测可编译）

| 演示 | 文件 / 目标 | 对应表行 |
|---|---|---|
| 线程池 + collectAll + CHM + MPMC 阻塞队列 + Synchronized + atomic_shared_ptr | `TestFolly/FollyConcurrencyDemo.cpp` → `folly_concurrent_api_demo` | 1/2/3/4/5/6 |
| ConcurrentHashMap 单独示例 | `TestFolly/ConcurrentHashMap.cpp` → `folly_concurrent_hash_map_demo` | CHM |
| DistributedMutex 多线程计数 | `src/folly_distributed_mutex_demo.cpp` | 锁 |
| MPMCQueue 生产者/消费者 | `src/folly_mpmc_queue_demo.cpp` | 队列 |
| AtomicLinkedList（Treiber 栈） | `src/folly_atomic_linked_list_demo.cpp` | 队列 |
| AtomicIntrusiveLinkedList（含 4 线程压测） | `TestFolly/AtomicIntrusiveLinkedListTest.cpp` → `atomic_intrusive_linked_list_test` | 栈 |
