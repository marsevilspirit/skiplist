#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <random>
#include <atomic>
#include "../src/skiplist.hpp"

using namespace skiplist;

// 简化的性能测试函数
void performance_test(const std::string& test_name, int num_threads, int operations_per_thread) {
    std::cout << "\n=== " << test_name << " ===" << std::endl;
    
    SkipList<int, std::string> sl(16);
    
    // 先插入一些初始数据
    for (int i = 0; i < 100; i++) {
        sl.insert(i, "initial_" + std::to_string(i));
    }
    
    std::vector<std::thread> threads;
    std::atomic<int> total_operations{0};
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // 启动工作线程
    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back([&sl, i, operations_per_thread, &total_operations]() {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(1, 200); // 减小范围避免冲突
            
            for (int j = 0; j < operations_per_thread; j++) {
                int key = dis(gen);
                int op = dis(gen) % 3;
                
                try {
                    switch (op) {
                        case 0: // 插入
                            sl.insert(key, "thread_" + std::to_string(i) + "_" + std::to_string(j));
                            break;
                        case 1: // 搜索
                            sl.search(key);
                            break;
                        case 2: // 删除
                            sl.remove(key);
                            break;
                    }
                    total_operations.fetch_add(1);
                } catch (...) {
                    // 忽略异常，继续执行
                }
            }
        });
    }
    
    // 等待所有线程完成
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    std::cout << "线程数: " << num_threads << std::endl;
    std::cout << "每线程操作数: " << operations_per_thread << std::endl;
    std::cout << "总操作数: " << total_operations.load() << std::endl;
    std::cout << "执行时间: " << duration.count() << " ms" << std::endl;
    if (duration.count() > 0) {
        std::cout << "吞吐量: " << (total_operations.load() * 1000.0 / duration.count()) << " ops/sec" << std::endl;
    }
    std::cout << "最终大小: " << sl.size() << std::endl;
}

int main() {
    std::cout << "=== SkipList 两把锁性能测试 ===" << std::endl;
    
    // 测试场景1：低并发
    std::cout << "\n📊 场景1：低并发 (4线程)" << std::endl;
    performance_test("两把锁 - 低并发", 4, 100);
    
    // 测试场景2：中等并发
    std::cout << "\n📊 场景2：中等并发 (8线程)" << std::endl;
    performance_test("两把锁 - 中等并发", 8, 100);
    
    // 测试场景3：高并发
    std::cout << "\n📊 场景3：高并发 (16线程)" << std::endl;
    performance_test("两把锁 - 高并发", 16, 50);
    
    std::cout << "\n✅ 性能测试完成！" << std::endl;
    return 0;
}
