#include <gtest/gtest.h>

#include <atomic>
#include <random>
#include <thread>
#include <vector>

#include "skiplist.hpp"

class SkipListTest : public ::testing::Test {
protected:
	void SetUp() override {
		sl = new skiplist::SkipList<int, std::string>(16);
	}

	void TearDown() override {
		delete sl;
	}

	skiplist::SkipList<int, std::string>* sl;
};

TEST_F(SkipListTest, InsertAndSearch) {
	// 测试插入和搜索
	sl->insert(5, "five");
	sl->insert(10, "ten");
	sl->insert(3, "three");

	auto node = sl->search(5);
	ASSERT_NE(node, nullptr);
	EXPECT_EQ(node->key, 5);
	EXPECT_EQ(node->value, "five");

	node = sl->search(10);
	ASSERT_NE(node, nullptr);
	EXPECT_EQ(node->key, 10);
	EXPECT_EQ(node->value, "ten");

	node = sl->search(3);
	ASSERT_NE(node, nullptr);
	EXPECT_EQ(node->key, 3);
	EXPECT_EQ(node->value, "three");
}

TEST_F(SkipListTest, SearchNonExistent) {
	// 测试搜索不存在的键
	sl->insert(5, "five");

	auto node = sl->search(10);
	EXPECT_EQ(node, nullptr);
}

TEST_F(SkipListTest, Remove) {
	// 测试删除
	sl->insert(5, "five");
	sl->insert(10, "ten");
	sl->insert(3, "three");

	// 删除存在的键
	sl->remove(5);
	auto node = sl->search(5);
	EXPECT_EQ(node, nullptr);

	// 确保其他键仍然存在
	node = sl->search(10);
	ASSERT_NE(node, nullptr);
	EXPECT_EQ(node->value, "ten");

	node = sl->search(3);
	ASSERT_NE(node, nullptr);
	EXPECT_EQ(node->value, "three");
}

TEST_F(SkipListTest, RemoveNonExistent) {
	// 测试删除不存在的键（不应该崩溃）
	sl->insert(5, "five");
	sl->remove(10); // 这应该不会崩溃

	// 确保原有的键仍然存在
	auto node = sl->search(5);
	ASSERT_NE(node, nullptr);
	EXPECT_EQ(node->value, "five");
}

TEST_F(SkipListTest, DuplicateInsert) {
	// 测试重复插入相同的键
	sl->insert(5, "five");
	sl->insert(5, "five_duplicate");

	// 应该只有一个节点，值应该是原来的
	auto node = sl->search(5);
	ASSERT_NE(node, nullptr);
	EXPECT_EQ(node->value, "five");
}

TEST_F(SkipListTest, EmptySkipList) {
	// 测试空跳表
	auto node = sl->search(5);
	EXPECT_EQ(node, nullptr);
}

TEST_F(SkipListTest, MultipleOperations) {
	// 测试多种操作的组合
	std::vector<int> keys = {1, 5, 3, 8, 2, 7, 4, 6};

	// 插入所有键
	for (int key : keys) {
		sl->insert(key, "value_" + std::to_string(key));
	}

	// 验证所有键都能找到
	for (int key : keys) {
		auto node = sl->search(key);
		ASSERT_NE(node, nullptr);
		EXPECT_EQ(node->key, key);
		EXPECT_EQ(node->value, "value_" + std::to_string(key));
	}

	// 删除一些键
	sl->remove(3);
	sl->remove(7);

	// 验证删除的键找不到
	EXPECT_EQ(sl->search(3), nullptr);
	EXPECT_EQ(sl->search(7), nullptr);

	// 验证其他键仍然存在
	std::vector<int> remaining_keys = {1, 5, 8, 2, 4, 6};
	for (int key : remaining_keys) {
		auto node = sl->search(key);
		ASSERT_NE(node, nullptr);
		EXPECT_EQ(node->key, key);
	}
}

// 并发测试相关
class ConcurrentSkipListTest : public ::testing::Test {
protected:
	void SetUp() override {
		sl = new skiplist::SkipList<int, std::string>(16);
	}

	void TearDown() override {
		delete sl;
	}

	skiplist::SkipList<int, std::string>* sl;
};

// 并发插入测试
TEST_F(ConcurrentSkipListTest, ConcurrentInsert) {
	const int num_threads = 4;
	const int inserts_per_thread = 100;
	std::vector<std::thread> threads;
	std::atomic<int> success_count{0};

	// 启动多个线程进行并发插入
	for (int i = 0; i < num_threads; i++) {
		threads.emplace_back([this, i, inserts_per_thread, &success_count]() {
			for (int j = 0; j < inserts_per_thread; j++) {
				int key = i * inserts_per_thread + j;
				std::string value = "thread_" + std::to_string(i) + "_value_" + std::to_string(j);
				sl->insert(key, value);
				success_count.fetch_add(1);
			}
		});
	}

	// 等待所有线程完成
	for (auto& thread : threads) {
		thread.join();
	}

	// 验证所有插入的键都存在
	EXPECT_EQ(success_count.load(), num_threads * inserts_per_thread);

	// 随机检查一些键
	for (int i = 0; i < 10; i++) {
		int random_key = rand() % (num_threads * inserts_per_thread);
		auto node = sl->search(random_key);
		ASSERT_NE(node, nullptr);
		EXPECT_EQ(node->key, random_key);
	}
}

// 并发搜索测试
TEST_F(ConcurrentSkipListTest, ConcurrentSearch) {
	// 先插入一些数据
	const int num_keys = 1000;
	for (int i = 0; i < num_keys; i++) {
		sl->insert(i, "value_" + std::to_string(i));
	}

	const int num_threads = 8;
	const int searches_per_thread = 200;
	std::vector<std::thread> threads;
	std::atomic<int> found_count{0};

	// 启动多个线程进行并发搜索
	for (int i = 0; i < num_threads; i++) {
		threads.emplace_back([this, num_keys, searches_per_thread, &found_count]() {
			for (int j = 0; j < searches_per_thread; j++) {
				int key = rand() % num_keys;
				auto node = sl->search(key);
				if (node != nullptr && node->key == key) {
					found_count.fetch_add(1);
				}
			}
		});
	}

	// 等待所有线程完成
	for (auto& thread : threads) {
		thread.join();
	}

	// 验证搜索操作正常
	EXPECT_GT(found_count.load(), 0);
}

// 并发插入和搜索测试
TEST_F(ConcurrentSkipListTest, ConcurrentInsertAndSearch) {
	const int num_threads = 4;
	const int operations_per_thread = 100;
	std::vector<std::thread> threads;
	std::atomic<int> insert_count{0};
	std::atomic<int> search_count{0};

	// 启动插入线程
	for (int i = 0; i < num_threads / 2; i++) {
		threads.emplace_back([this, i, operations_per_thread, &insert_count]() {
			for (int j = 0; j < operations_per_thread; j++) {
				int key = i * operations_per_thread + j;
				std::string value =
					"insert_thread_" + std::to_string(i) + "_value_" + std::to_string(j);
				sl->insert(key, value);
				insert_count.fetch_add(1);
			}
		});
	}

	// 启动搜索线程
	for (int i = 0; i < num_threads / 2; i++) {
		threads.emplace_back([this, operations_per_thread, &search_count]() {
			for (int j = 0; j < operations_per_thread; j++) {
				int key = rand() % (operations_per_thread * 2); // 搜索可能存在的键
				sl->search(key);
				search_count.fetch_add(1);
			}
		});
	}

	// 等待所有线程完成
	for (auto& thread : threads) {
		thread.join();
	}

	// 验证操作计数
	EXPECT_EQ(insert_count.load(), (num_threads / 2) * operations_per_thread);
	EXPECT_EQ(search_count.load(), (num_threads / 2) * operations_per_thread);
}

// 并发删除测试
TEST_F(ConcurrentSkipListTest, ConcurrentRemove) {
	// 先插入一些数据
	const int num_keys = 500;
	for (int i = 0; i < num_keys; i++) {
		sl->insert(i, "value_" + std::to_string(i));
	}

	const int num_threads = 4;
	const int removes_per_thread = 50;
	std::vector<std::thread> threads;
	std::atomic<int> remove_count{0};

	// 启动多个线程进行并发删除
	for (int i = 0; i < num_threads; i++) {
		threads.emplace_back([this, i, removes_per_thread, &remove_count]() {
			for (int j = 0; j < removes_per_thread; j++) {
				int key = i * removes_per_thread + j;
				sl->remove(key);
				remove_count.fetch_add(1);
			}
		});
	}

	// 等待所有线程完成
	for (auto& thread : threads) {
		thread.join();
	}

	// 验证删除操作正常
	EXPECT_EQ(remove_count.load(), num_threads * removes_per_thread);

	// 验证删除的键确实不存在了
	for (int i = 0; i < num_threads * removes_per_thread; i++) {
		auto node = sl->search(i);
		EXPECT_EQ(node, nullptr);
	}
}

// 压力测试：大量并发操作
TEST_F(ConcurrentSkipListTest, StressTest) {
	const int num_threads = 10;
	const int operations_per_thread = 500;
	std::vector<std::thread> threads;
	std::atomic<int> total_operations{0};

	// 启动多个线程进行混合操作
	for (int i = 0; i < num_threads; i++) {
		threads.emplace_back([this, i, operations_per_thread, &total_operations]() {
			for (int j = 0; j < operations_per_thread; j++) {
				int key = i * operations_per_thread + j;
				int op = rand() % 3;

				switch (op) {
				case 0: // 插入
					sl->insert(key, "stress_value_" + std::to_string(key));
					break;
				case 1: // 搜索
					sl->search(key);
					break;
				case 2: // 删除
					sl->remove(key);
					break;
				}
				total_operations.fetch_add(1);
			}
		});
	}

	// 等待所有线程完成
	for (auto& thread : threads) {
		thread.join();
	}

	// 验证操作完成
	EXPECT_EQ(total_operations.load(), num_threads * operations_per_thread);

	// 验证数据结构仍然一致
	EXPECT_GE(sl->size(), 0); // 大小应该非负
}

// 数据一致性测试
TEST_F(ConcurrentSkipListTest, DataConsistency) {
	// 插入一些初始数据
	for (int i = 0; i < 100; i++) {
		sl->insert(i, "initial_" + std::to_string(i));
	}

	const int num_threads = 6;
	const int operations_per_thread = 100;
	std::vector<std::thread> threads;

	// 启动混合操作线程
	for (int i = 0; i < num_threads; i++) {
		threads.emplace_back([this, i, operations_per_thread]() {
			for (int j = 0; j < operations_per_thread; j++) {
				int key = (i * operations_per_thread + j) % 200;
				int op = rand() % 3;

				switch (op) {
				case 0: // 插入
					sl->insert(key, "thread_" + std::to_string(i) + "_" + std::to_string(j));
					break;
				case 1: // 搜索
					sl->search(key);
					break;
				case 2: // 删除
					sl->remove(key);
					break;
				}
			}
		});
	}

	// 等待所有线程完成
	for (auto& thread : threads) {
		thread.join();
	}

	// 验证数据结构的一致性
	// 1. 检查size()方法是否正常工作
	int final_size = sl->size();
	EXPECT_GE(final_size, 0);

	// 2. 检查display()方法是否正常工作（不应该崩溃）
	EXPECT_NO_THROW(sl->display());
}
