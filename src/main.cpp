#include <iostream>
#include <cstdlib>
#include <ctime>

#include "skiplist.hpp"

int main() {
    // 初始化随机数种子
    srand((unsigned)time(NULL));

    // 创建一个最大层数为 15 的跳表 (0-15), 键为 int, 值为 string
	skiplist::SkipList<int, std::string> sl(15);

    sl.insert(10, "Apple");
    sl.insert(30, "Cat");
    sl.insert(20, "Banana");
    sl.insert(50, "Egg");
    sl.insert(40, "Dog");
    sl.insert(5, "Grape");
    sl.insert(25, "Xylophone");

    sl.display();

    std::cout << "\n--- Searching ---\n";
    sl.search(20);
    sl.search(45);

    std::cout << "\n--- Deleting ---\n";
    sl.remove(30);
    sl.remove(5);
    sl.remove(100); // 尝试删除一个不存在的键

    sl.display();

    return 0;
}
