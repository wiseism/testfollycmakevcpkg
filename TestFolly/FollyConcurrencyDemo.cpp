// TestFolly/FollyConcurrencyDemo.cpp
//
// folly 并发 API 演示，按 Java JUC / libcds 的 familiar 概念对照组织。
// 概念映射与选型说明见同目录 FollyConcurrency.md。
//
// 演示 6 组：
//   1. ExecutorService        ~ folly::CPUThreadPoolExecutor + folly::via
//   2. CompletableFuture.allOf ~ folly::futures::collectAll
//   3. ConcurrentSkipListMap / ConcurrentHashMap ~ folly::ConcurrentHashMap
//   4. BlockingQueue          ~ folly::MPMCQueue（write/read 阻塞版）
//   5. Collections.synchronizedMap ~ folly::Synchronized
//   6. AtomicReference        ~ folly::atomic_shared_ptr
//
// 另有：DistributedMutex（≈LockSupport/ReentrantLock 的无等待自旋锁）已在
// src/folly_distributed_mutex_demo.cpp 单独演示；libcds 的 RCU 对应 folly 的
// concurrency/HazptrTS.h / concurrency/EpochReclaimer.h（本文件不展开）。

#include <folly/MPMCQueue.h>
#include <folly/Synchronized.h>
#include <folly/concurrency/AtomicSharedPtr.h>
#include <folly/concurrency/ConcurrentHashMap.h>
#include <folly/executors/CPUThreadPoolExecutor.h>
#include <folly/futures/Future.h>

#include <atomic>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

static int g_pass = 0;
static int g_fail = 0;
#define CHECK(cond)                                           \
  do {                                                        \
    if (cond) {                                               \
      ++g_pass;                                               \
    } else {                                                  \
      ++g_fail;                                               \
      std::cout << "FAIL " << __LINE__ << ": " #cond << "\n"; \
    }                                                         \
  } while (0)

// ---------------------------------------------------------------------------
// 1+2. 线程池 + Future 聚合
// JUC:    ExecutorService es = Executors.newFixedThreadPool(4);
//         List<Future<Integer>> fs; CompletableFuture.allOf(...)
// libcds: 无直接对应（自己配 worker 线程）
// ---------------------------------------------------------------------------
void test_executor_and_collect_all() {
  folly::CPUThreadPoolExecutor executor(4);

  std::vector<folly::Future<int>> futures;
  for (int i = 1; i <= 8; ++i) {
    // via ≈ es.submit(supplier)
    futures.push_back(folly::via(&executor, [i] { return i * i; }));
  }
  // collectAll ≈ CompletableFuture.allOf(...)，但单个失败不影响整体
  auto results = folly::collectAll(std::move(futures)).get();

  int sum = 0;
  bool allOk = true;
  for (auto& t : results) {
    if (t.hasException()) {
      allOk = false;
      continue;
    }
    sum += t.value();
  }
  CHECK(allOk);
  CHECK(sum == 204); // 1+4+9+...+64
  executor.stop();
}

// ---------------------------------------------------------------------------
// 3. ConcurrentHashMap（无锁读、分桶写）
// JUC:    ConcurrentHashMap<String,Integer> m; m.computeIfAbsent / forEach
// libcds: cds::type::container::map（或 skip_list 模拟）
// ---------------------------------------------------------------------------
void test_concurrent_hash_map() {
  folly::ConcurrentHashMap<std::string, int> map;
  std::vector<std::thread> workers;
  for (int t = 0; t < 4; ++t) {
    workers.emplace_back([&map, t] {
      for (int i = 0; i < 1000; ++i) {
        map.insert_or_assign("t" + std::to_string(t) + "_k" + std::to_string(i), i);
      }
    });
  }
  for (auto& w : workers) w.join();

  CHECK(map.size() == 4000);
  CHECK(map.find("t3_k999") != map.end());

  // assign_if_equal：本版本 ConcurrentHashMap 的 CAS 更新原语（find 只有 const
  // 版本，没有上游新版的 apply/peek）：值等于 expected 才换成 desired
  auto updated = map.assign_if_equal("t3_k999", 999, -1);
  CHECK(updated.hasValue());
  auto it = map.find("t3_k999");
  CHECK(it != map.end() && it->second == -1);
}

// ---------------------------------------------------------------------------
// 4. 阻塞队列
// JUC:    ArrayBlockingQueue / LinkedBlockingQueue
// libcds: fs::LifoQueue / VSQueue（但 read 默认自旋，需 backoff）
// ---------------------------------------------------------------------------
void test_mpmc_blocking_queue() {
  folly::MPMCQueue<int> q(64); // 容量固定，≈ ArrayBlockingQueue
  // 实测坑：本版本 folly 里 write()/read() 是【非阻塞】的（满/空直接返回
  // false，元素会被悄悄丢掉），阻塞语义要用 blockingWrite()/blockingRead()
  std::atomic<int> consumedSum{0};
  constexpr int kConsumers = 2;

  std::thread producer([&q] {
    for (int i = 1; i <= 100; ++i) q.blockingWrite(i); // 满了就阻塞
    for (int c = 0; c < kConsumers; ++c)
      q.blockingWrite(-1); // 每个消费者一个哨兵
  });
  std::vector<std::thread> consumers;
  for (int c = 0; c < kConsumers; ++c) {
    consumers.emplace_back([&q, &consumedSum] {
      int v;
      for (;;) {
        q.blockingRead(v); // 空了才真正挂起等待（内部信号量，不是纯自旋）
        if (v < 0) break;  // 拿到自己的哨兵
        consumedSum += v;
      }
    });
  }
  producer.join();
  for (auto& c : consumers) c.join();
  CHECK(consumedSum.load() == 5050);
}

// ---------------------------------------------------------------------------
// 5. Synchronized<T>：把任意非线程安全对象包起来，细粒度 API 而不是整对象大锁
// JUC:    Collections.synchronizedMap(new HashMap<>())
// libcds: 不需要（它本身就是无锁容器），这里对应"给普通容器加锁包装"的思路
// ---------------------------------------------------------------------------
void test_synchronized() {
  folly::Synchronized<std::unordered_map<int, int>> syncMap;
  std::vector<std::thread> workers;
  for (int t = 0; t < 4; ++t) {
    workers.emplace_back([&syncMap, t] {
      for (int i = 0; i < 1000; ++i) {
        auto locked = syncMap.wlock(); // 显式写锁（RAII）
        (*locked)[t * 1000 + i] = i;
      }
    });
  }
  for (auto& w : workers) w.join();

  // withRLock：读锁下做只读聚合
  auto sum = syncMap.withRLock([](const auto& m) {
    long long s = 0;
    for (auto& kv : m) s += kv.second;
    return s;
  });
  CHECK(sum == 4000 * 999 / 2);
}

// ---------------------------------------------------------------------------
// 6. atomic_shared_ptr：读侧无锁、写侧不阻塞读（RCU 式发布/订阅）
// JUC:    AtomicReference<Config> ref
// libcds: cds::TPFuchs 或 RCU 类型族
// ---------------------------------------------------------------------------
struct Config {
  int version;
  std::string name;
};

void test_atomic_shared_ptr() {
  folly::atomic_shared_ptr<const Config> config(
      std::make_shared<Config>(Config{1, "v1"}));

  std::atomic<bool> done{false};
  std::atomic<int> reads{0};
  std::thread reader([&] {
    do { // do-while 保证至少 load 一次
      auto snap = config.load(); // 无锁读，拿到一致快照
      if (snap->version >= 1 && snap->version <= 3) ++reads;
    } while (!done.load());
  });
  std::thread writer([&] {
    config.store(std::make_shared<Config>(Config{2, "v2"})); // 发布新快照
    std::this_thread::yield();
    config.store(std::make_shared<Config>(Config{3, "v3"}));
    done = true;
  });
  reader.join();
  writer.join();
  CHECK(config.load()->version == 3);
  CHECK(reads.load() > 0);
}

int main() {
  std::cout << "== folly 并发 API 演示（对照 JUC / libcds）==\n";
  test_executor_and_collect_all();
  test_concurrent_hash_map();
  test_mpmc_blocking_queue();
  test_synchronized();
  test_atomic_shared_ptr();
  std::cout << "pass=" << g_pass << " fail=" << g_fail << "\n";
  return g_fail == 0 ? 0 : 1;
}
