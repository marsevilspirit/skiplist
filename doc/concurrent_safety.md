# SkipList 并发安全实现说明

## 概述

本文档描述了如何在原有SkipList基础上实现并发安全，确保多个线程可以安全地同时访问和修改SkipList数据结构。

## 并发安全问题

原始的SkipList实现在多线程环境下存在以下问题：

1. **数据竞争**：多个线程同时修改同一个节点
2. **指针失效**：一个线程正在遍历时，另一个线程删除了节点
3. **原子性**：插入和删除操作需要原子性保证
4. **一致性**：数据结构的状态可能不一致

## 解决方案

### 1. 锁机制

- **读写锁 (std::shared_mutex)**：保护整个数据结构
  - 读操作使用 `std::shared_lock`，允许多个线程同时读取
  - 写操作使用 `std::unique_lock`，确保独占访问
  
- **插入/删除互斥锁 (std::mutex)**：确保修改操作的原子性

### 2. 原子操作

- **currentLevel**：使用 `std::atomic<int>` 确保线程安全
- **原子方法**：`load()`, `store()`, `fetch_sub()` 等

### 3. 锁的层次结构

```
rw_mutex (读写锁)
├── 读操作：search(), display(), size()
└── 写操作：insert(), remove()

insert_mutex (插入/删除互斥锁)
├── insert()
└── remove()
```

## 实现细节

### 修改的成员变量

```cpp
private:
    int maxLevel;
    float p;
    std::atomic<int> currentLevel;  // 原子操作
    Node<K, V>* header;
    mutable std::shared_mutex rw_mutex;  // 读写锁
    std::mutex insert_mutex;  // 插入/删除互斥锁
```

### 修改的方法

1. **insert()**：使用 `insert_mutex` 确保原子性
2. **remove()**：使用 `insert_mutex` 确保原子性  
3. **search()**：使用 `rw_mutex` 的读锁
4. **display()**：使用 `rw_mutex` 的读锁
5. **size()**：新增方法，使用 `rw_mutex` 的读锁

## 性能特点

### 优点

- **读操作并发**：多个线程可以同时读取，提高并发性能
- **写操作安全**：确保数据一致性
- **最小化锁竞争**：只在必要时加锁

### 缺点

- **写操作串行化**：插入和删除操作不能并发执行
- **锁开销**：增加了锁管理的开销
- **内存使用**：增加了锁相关的内存占用

## 使用示例

```cpp
#include "skiplist.hpp"
#include <thread>
#include <vector>

int main() {
    skiplist::SkipList<int, std::string> sl(16);
    
    // 多线程安全操作
    std::thread t1([&]() {
        sl.insert(1, "one");
        sl.search(1);
    });
    
    std::thread t2([&]() {
        sl.insert(2, "two");
        sl.remove(1);
    });
    
    t1.join();
    t2.join();
    
    return 0;
}
```

## 进一步优化建议

1. **无锁实现**：使用CAS操作和内存屏障实现无锁SkipList
2. **分层锁**：为不同层级使用不同的锁，减少锁竞争
3. **乐观锁**：使用版本号或时间戳实现乐观并发控制
4. **内存池**：使用内存池减少内存分配开销

## 注意事项

1. **死锁预防**：确保锁的获取顺序一致
2. **异常安全**：使用RAII管理锁资源
3. **性能测试**：在不同并发度下测试性能
4. **内存管理**：注意节点的生命周期管理
