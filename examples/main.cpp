#include <iostream>
#include "skiplist.hpp"

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
    
    return 0;
}
