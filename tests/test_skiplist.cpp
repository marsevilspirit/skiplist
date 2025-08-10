#include <gtest/gtest.h>

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
