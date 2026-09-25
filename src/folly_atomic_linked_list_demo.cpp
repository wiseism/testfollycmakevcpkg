#include <atomic>
#include <iostream>
#include <thread>
#include <folly/AtomicLinkedList.h>

template<typename T>
struct AtomicNode {
    T value;
    std::atomic<AtomicNode*> next;

    explicit AtomicNode(T v) : value(v), next(nullptr) {}
};

template<typename T>
class SimpleAtomicList {
public:
    void pushHead(T value) {
        auto node = new AtomicNode<T>(value);
        AtomicNode<T>* oldHead = head.load();
        do {
            node->next.store(oldHead, std::memory_order_relaxed);
        } while (!head.compare_exchange_weak(oldHead, node));
    }

    void traverse() {
        auto current = head.load();
        while (current) {
            std::cout << current->value << " ";
            current = current->next.load();
        }
        std::cout << std::endl;
    }

private:
    std::atomic<AtomicNode<T>*> head{nullptr};
};

// 使用示例
void producer(SimpleAtomicList<int>& list) {
    for (int i = 0; i < 5; ++i)
        list.pushHead(i);
}

int main() {
    SimpleAtomicList<int> list;

    std::thread t1(producer, std::ref(list));
    std::thread t2(producer, std::ref(list));

    t1.join();
    t2.join();

    list.traverse();  // 输出可能是乱序的，因为并发 pushHead

    return 0;
}