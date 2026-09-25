#include <folly/MPMCQueue.h>
#include <thread>
#include <iostream>

int main() {
    folly::MPMCQueue<int> queue(128); // 队列容量

    std::thread producer([&] {
        for (int i = 0; i < 10; ++i) {
            while (!queue.write(i)); // 忙写入
            std::cout << "Produced: " << i << std::endl;
        }
    });

    std::thread consumer([&] {
        int value;
        for (int i = 0; i < 10; ++i) {
            while (!queue.read(value)); // 忙读取
            std::cout << "Consumed: " << value << std::endl;
        }
    });

    producer.join();
    consumer.join();

    return 0;
}