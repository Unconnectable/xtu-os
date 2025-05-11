# 实验报告

## 基本信息

- **班级**: 计科 23 级 1 班
- **学号**: 202305133352
- **姓名**: 周潍可
- **实验名称**: 内存动态分区分配与回收模拟程序设计

---

## 1. 实验目的

> 本次实验旨在理解和掌握操作系统中内存管理的基本原理,特别是**动态分区分配机制**的实现方式.通过编程模拟内存的申请与释放过程,熟悉首次适应(First Fit)算法的应用,并掌握如何在内存回收时进行相邻空闲块的合并操作,以减少内存碎片、提高内存利用率.

---

## 2. 数据结构说明

描述你在程序中使用的数据结构及其作用.

> 本实验中使用了以下数据结构:

> - `MemoryBlock`: 表示一个内存块,包含以下字段:
>   - `start`: 起始地址
>   - `size`: 分区大小
>   - `is_free`: 分区状态(是否为空闲)
>   - `job_id`: 分配给的作业编号(可选值)

> - `std::list<MemoryBlock>`: 使用 C++ STL 中的链表结构来维护所有内存块.这种结构便于插入、删除和遍历,非常适合模拟内存的动态分配与回收.

> - `MemoryManager`: 封装内存管理功能的类,提供如下方法:
>   - `allocate(JobID, Size)`: 动态分配内存
>   - `free(JobID)`: 回收指定作业的内存
>   - `print()`: 打印当前内存状态
>   - `getBlocks()`: 获取当前内存块列表

这些数据结构共同构建了一个简单的内存管理系统模型.

---

## 3. 处理过程(流程图或盒图)

![lab3](/assets/lab3.png)

## 4. 源代码

```cpp
#include <list>
#include <optional>

const size_t MEMORY_SIZE = 1024;

// 内存块定义
class MemoryBlock {
 public:
  size_t start;
  size_t size;
  bool is_free;
  std::optional<size_t> job_id;
  MemoryBlock(size_t s, size_t sz, bool free, std::optional<size_t> id);
  void print();
};

class MemoryManager {
 private:
  std::list<MemoryBlock> blocks;

 public:
  MemoryManager();
  bool allocate(size_t JobId, size_t Size);

  bool free(size_t JobId);

  void print();
  std::list<MemoryBlock> getBlocks();
};
```

```cpp
#include "lab3/MemoryAlloc.h"
#include <iostream>
#include <list>
#include <optional>
#include <string>

MemoryBlock::MemoryBlock(size_t s, size_t sz, bool free, std::optional<size_t> id)
    : start(s), size(sz), is_free(free), job_id(id) {}

void MemoryBlock::print() {
  std::cout << "Start: " << start << std::endl;
  std::cout << "Size: " << size << std::endl;
  std::cout << (is_free ? "Yes" : "No") << ", JobID: " << (job_id.has_value() ? std::to_string(*job_id) : "None")
            << std::endl;
}

MemoryManager::MemoryManager() { blocks.push_back(MemoryBlock(0, MEMORY_SIZE, true, std::nullopt)); }

bool MemoryManager::allocate(size_t JobId, size_t Size) {
  for (auto it = blocks.begin(); it != blocks.end(); ++it) {
    if (it->is_free && it->size >= Size) {
      // free and 可分配
      if (it->size == Size) {
        it->job_id = JobId;
        it->is_free = false;
      } else {
        // 只能分配小的
        MemoryBlock new_block(it->start + Size, it->size - Size, true, std::nullopt);
        blocks.insert(std::next(it), new_block);

        it->size = Size;
        it->job_id = JobId;
        it->is_free = false;
      }
      return true;
    }
  }
  return false;
}

bool MemoryManager::free(size_t JobId) {
  // 回收某个块
  for (auto it = blocks.begin(); it != blocks.end(); ++it) {
    if (!it->is_free && it->job_id.has_value() && *it->job_id == JobId) {
      // 回收
      it->is_free = true;
      it->job_id = std::nullopt;

      // 如果是 free并且
      // 是否存在 !=end
      // 是否连续
      auto prev_ = std::prev(it);
      if (prev_ != blocks.end() && prev_->is_free && prev_->start + prev_->size == it->start) {
        prev_->size += it->size;
        it = blocks.erase(it);
        //it = prev_;
      }
      auto next_ = std::next(it);
      if (next_->is_free && next_ != blocks.end() && it->start + it->size == next_->start) {
        it->size += next_->size;
        blocks.erase(next_);
      }
      return true;
    }
  }
  return false;
}

void MemoryManager::print() {
  std::cout << "MemorysStatus:" << std::endl;
  for (auto &block : blocks) {
    block.print();
  }
  std::cout << std::endl;
}

std::list<MemoryBlock> MemoryManager::getBlocks() { return blocks; }
```

---

## 5. 实验总结

> 本次实验让我深入理解了操作系统中内存管理的核心概念,尤其是动态分区分配机制的实现细节.通过编写 `MemoryManager` 类,我掌握了如何用面向对象的方式模拟内存块的管理,并实现了首次适应(First Fit)算法进行资源分配.
>
> 在开发过程中,我遇到了几个关键问题:
>
> - 如何高效地对内存块进行分割与合并;
> - 如何处理链表中的指针移动和节点删除;
> - 如何判断相邻空闲块并正确合并;
> - 如何确保内存分配不出现重叠或越界错误.
>
> 通过合理设计 `MemoryBlock` 结构体和使用 `std::list` 来管理内存块,这些问题都得到了有效解决.尤其是在回收操作中,通过对前一个和后一个块的检查,实现了自动合并功能,大大减少了内存碎片.
>
> 此外,我还学习到了如何将抽象的操作系统理论转化为实际的程序逻辑,并验证其行为是否符合预期.实验测试表明,该程序能够正确执行内存分配与回收操作,具备良好的健壮性和扩展性.
>

---
