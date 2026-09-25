#include <folly/synchronization/DistributedMutex.h>
#include <thread>
#include <vector>
#include <iostream>
#include <mutex>

folly::DistributedMutex mutex;
int shared_counter = 0;

void worker(int id, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        std::unique_lock lock(mutex);  // ✅ 使用标准库风格加锁
        ++shared_counter;
    }
    std::cout << "Thread " << id << " done." << std::endl;
}

int main() {
    const int num_threads = 4;
    const int iterations = 100000;

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker, i, iterations);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Final counter value: " << shared_counter << std::endl;
    return 0;
}