#include <chrono>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

#include "skiplist.hpp"

// 并发工作线程函数
void worker_thread(skiplist::SkipList<int, std::string>* skiplist, int thread_id,
				   int num_operations) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(1, 100);

	for (int i = 0; i < num_operations; i++) {
		int key = dis(gen);
		std::string value = "value_" + std::to_string(key) + "_thread_" + std::to_string(thread_id);

		// 随机选择操作类型
		int op = dis(gen) % 3;

		switch (op) {
		case 0: // 插入
			skiplist->insert(key, value);
			break;
		case 1: // 搜索
			skiplist->search(key);
			break;
		case 2: // 删除
			skiplist->remove(key);
			break;
		}

		// 添加一些延迟来模拟真实工作负载
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}
}

int main() {
	// 创建一个跳表，最大层数为16
	skiplist::SkipList<int, std::string> sl(16);

	std::cout << "=== SkipList Demo ===" << std::endl;

	// 插入一些数据
	std::cout << "\n--- Inserting data ---" << std::endl;
	sl.insert(3, "three");
	sl.insert(6, "six");
	sl.insert(7, "seven");
	sl.insert(9, "nine");
	sl.insert(12, "twelve");
	sl.insert(19, "nineteen");
	sl.insert(17, "seventeen");
	sl.insert(26, "twenty-six");
	sl.insert(21, "twenty-one");
	sl.insert(25, "twenty-five");

	// 显示跳表结构
	sl.display();

	// 搜索测试
	std::cout << "\n--- Search test ---" << std::endl;
	sl.search(19);
	sl.search(15); // 不存在的key

	// 删除测试
	std::cout << "\n--- Delete test ---" << std::endl;
	sl.remove(19);
	sl.remove(15); // 不存在的key

	// 显示删除后的结构
	std::cout << "\n--- After deletion ---" << std::endl;
	sl.display();

	// 并发测试
	std::cout << "\n=== 并发测试 ===" << std::endl;
	std::cout << "当前大小: " << sl.size() << std::endl;

	// 创建多个工作线程
	const int num_threads = 4;
	const int operations_per_thread = 20;

	std::vector<std::thread> threads;

	std::cout << "\n启动 " << num_threads << " 个并发线程，每个线程执行 " << operations_per_thread
			  << " 次操作..." << std::endl;

	auto start_time = std::chrono::high_resolution_clock::now();

	// 启动工作线程
	for (int i = 0; i < num_threads; i++) {
		threads.emplace_back(worker_thread, &sl, i, operations_per_thread);
	}

	// 等待所有线程完成
	for (auto& thread : threads) {
		thread.join();
	}

	auto end_time = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

	std::cout << "\n所有线程完成！" << std::endl;
	std::cout << "总执行时间: " << duration.count() << " ms" << std::endl;

	// 显示最终结果
	std::cout << "\n--- 并发操作后的结果 ---" << std::endl;
	sl.display();
	std::cout << "最终大小: " << sl.size() << std::endl;

	return 0;
}
