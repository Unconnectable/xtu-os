#ifndef PAGE_REPLACER_H
#define PAGE_REPLACER_H

#pragma once
#include <algorithm>
#include <deque>
#include <limits>
#include <list>
#include <map>
#include <optional>
#include <vector>
enum class PageReplaceAlgo {
  OPT,
  FIFO,
  LRU_K,  // LUR_K
  LFU
};

class PageReplacer {
 public:
  PageReplacer(
      PageReplaceAlgo algorithm, size_t frame_capacity,
      size_t k = 2,  // default K=2
      const std::optional<std::vector<size_t>> &ref_seq = std::nullopt);

  void access_page(size_t page_num);  // 访问 某个页面

  std::vector<size_t> &get_frames();  // 当前的 frame 有哪些页

  size_t get_page_fault();     // 当前的 缺页数目
  double page_fault_ratio();   // 计算缺页率
  size_t get_replace_count();  // 当前的 置换次数

 private:
  PageReplaceAlgo algorithm_;
  size_t frame_capacity_;
  std::vector<size_t> frames_;
  size_t k_value_;  // LRU/LFU -K

  std::optional<std::vector<size_t>> ref_seq_;
  size_t access_index_ = 0;   // 当前访问位置
  size_t page_faults_ = 0;    // 缺页数目
  size_t replace_count_ = 0;  // 置换次数

  std::list<size_t> fifo_queue_;
  std::map<size_t, std::deque<size_t>>
      access_history_;  // 每个元素的访问历史 5:{2,4,5} 第2 4 5 次访问了
                        // 5这个元素

  size_t find_opt();
  size_t find_fifo();
  size_t find_lru_k();
  size_t find_lfu();
};

#endif  // PAGE_REPLACER_H