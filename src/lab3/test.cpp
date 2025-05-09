#include <iostream>
#include <list>
#include <memory>
#include <optional>
#include <string>

const int MEMORY_SIZE = 1024;
using std::cin;
using std::cout;
using std::endl;
using std::list;
using std::next;
using std::nullopt;
using std::optional;
using std::prev;
using std::to_string;
// 内存块定义
class MemoryBlock {
 public:
  int start;
  int size;
  bool is_free;
  std::optional<int> job_id;

  MemoryBlock(int s, int sz, bool free, std::optional<int> id) : start(s), size(sz), is_free(free), job_id(id) {}

  void print() const {
    cout << "Start: " << start << endl;
    cout << "Size: " << size << endl;
    cout << (is_free ? "Yes" : "No") << ", JobID: " << (job_id.has_value() ? std::to_string(*job_id) : "None")
         << std::endl;
  }
};

// 内存管理器
class MemoryManager {
 private:
  std::list<MemoryBlock> blocks;

 public:
  MemoryManager() {
    // 初始化一个完整的空闲块
    blocks.emplace_back(MemoryBlock(0, MEMORY_SIZE, true, std::nullopt));
  }

  // 分配内存（首次适应）
  bool allocate(int job_id, int size) {
    for (auto it = blocks.begin(); it != blocks.end(); ++it) {
      if (it->is_free && it->size >= size) {
        if (it->size == size) {
          // 完全匹配，直接占用
          it->is_free = false;
          it->job_id = job_id;
        } else {
          // 插入新块分割
          MemoryBlock new_block(it->start + size, it->size - size, true, std::nullopt);
          blocks.insert(std::next(it), new_block);

          it->size = size;
          it->is_free = false;
          it->job_id = job_id;
        }
        return true;
      }
    }
    return false;  // 没有合适块
  }

  // 回收内存并合并
  bool free(int job_id) {
    for (auto it = blocks.begin(); it != blocks.end(); ++it) {
      if (!it->is_free && it->job_id && *it->job_id == job_id) {
        // 标记为空闲
        it->is_free = true;
        it->job_id = std::nullopt;

        // 合并前一个
        auto prev = std::prev(it);
        if (prev != blocks.end() && prev->is_free && prev->start + prev->size == it->start) {
          prev->size += it->size;
          blocks.erase(it);
          it = prev;
        }

        // 合并后一个
        auto next = std::next(it);
        if (next != blocks.end() && next->is_free && it->start + it->size == next->start) {
          it->size += next->size;
          blocks.erase(next);
        }
        return true;
      }
    }
    return false;  // 没找到
  }

  // 打印当前内存状态
  void print() const {
    std::cout << "Memory Status:\n";
    for (const auto &block : blocks) {
      block.print();
    }
    std::cout << std::endl;
  }
};

// 主函数
int main() {
  MemoryManager manager;

  int choice;
  while (true) {
    std::cout << "1. Allocate\n2. Free\n3. Print\n4. Exit\n> ";
    std::cin >> choice;

    if (choice == 1) {
      int job_id, size, runtime;
      std::cout << "Enter job info (job_id size runtime): ";
      std::cin >> job_id >> size >> runtime;

      if (manager.allocate(job_id, size)) {
        std::cout << "Allocated successfully for job " << job_id << "\n\n";
      } else {
        std::cout << "Allocation failed for job " << job_id << "\n\n";
      }
    } else if (choice == 2) {
      int job_id;
      std::cout << "Enter job_id to free: ";
      std::cin >> job_id;

      if (manager.free(job_id)) {
        std::cout << "Freed successfully for job " << job_id << "\n\n";
      } else {
        std::cout << "Failed to find job " << job_id << "\n\n";
      }
    } else if (choice == 3) {
      manager.print();
    } else if (choice == 4) {
      break;
    }
  }

  return 0;
}