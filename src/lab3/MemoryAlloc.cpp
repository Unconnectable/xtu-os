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
    if (!it->is_free && *it->job_id == JobId) {
      // 回收
      it->is_free = true;
      it->job_id = std::nullopt;

      // 如果是 free并且
      // 是否存在 !=end
      // 是否连续
      auto prev_ = std::prev(it);
      if (prev_ != blocks.end() && prev_->is_free && prev_->start + prev_->size == it->start) {
        prev_->size += it->size;
        blocks.erase(it);
        it = prev_;
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