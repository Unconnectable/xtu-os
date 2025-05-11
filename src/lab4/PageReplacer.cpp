#include "lab4/PageReplacer.h"

#include <algorithm>
#include <deque>
#include <limits>
#include <list>
#include <map>
#include <optional>
#include <stdexcept>
#include <vector>
PageReplacer::PageReplacer(PageReplaceAlgo algo, size_t capacity, size_t k,
                           const std::optional<std::vector<size_t>> &ref_seq)
    : algorithm_(algo),
      frame_capacity_(capacity),
      k_value_(k),
      ref_seq_(ref_seq) {
  //
  if (frame_capacity_ == 0) {
    throw(std::invalid_argument("Check Capacity!!"));
  }
  if (!ref_seq.has_value()) {
    throw(std::invalid_argument("Check RefSeq!!"));
  }
}

void PageReplacer::access_page(size_t pg) {
  //
  auto it = std::find(frames_.begin(), frames_.end(), pg);
  access_index_++;

  // Hit 命中
  if (it != frames_.end()) {
    if (algorithm_ == PageReplaceAlgo::LFU ||
        algorithm_ == PageReplaceAlgo::LRU_K) {
      //
      access_history_[pg].push_back(access_index_);

      if (access_history_[pg].size() > k_value_) {
        access_history_[pg].pop_front();
      }
    }
  }

  else {
    // 未命中
    page_faults_++;
    // 未满
    if (frames_.size() < frame_capacity_) {
      frames_.push_back(pg);
      if (algorithm_ == PageReplaceAlgo::FIFO) {
        fifo_queue_.push_back(pg);
      } else if (algorithm_ == PageReplaceAlgo::LFU ||
                 algorithm_ == PageReplaceAlgo::LRU_K) {
        //
        access_history_[pg].push_back(access_index_);
      }
    } else {
      // 已经满了 需要置换
      size_t replace_idx;
      size_t pg_to_replace;
      switch (algorithm_) {
        case PageReplaceAlgo::OPT:
          replace_idx = find_opt();
          break;
        case PageReplaceAlgo::FIFO:
          replace_idx = find_fifo();
          break;
        case PageReplaceAlgo::LRU_K:
          replace_idx = find_lru_k();
          break;
        case PageReplaceAlgo::LFU:
          replace_idx = find_lfu();
          break;
        default:
          throw std::runtime_error("Algorithm Error!");
      }
      // 把要置换的页面 pg_to_replace换成 pg
      pg_to_replace = frames_[replace_idx];
      frames_[replace_idx] = pg;
      replace_count_++;

      switch (algorithm_) {
        case PageReplaceAlgo::FIFO: {
          // FIFO
          auto fifo_it =
              std::find(fifo_queue_.begin(), fifo_queue_.end(), pg_to_replace);
          if (fifo_it != fifo_queue_.end()) {
            fifo_queue_.erase(fifo_it);
          }
          fifo_queue_.push_back(pg);
          break;
        }

        case PageReplaceAlgo::LRU_K:
        case PageReplaceAlgo::LFU: {
          // LRU-K 和 LFU
          access_history_.erase(pg_to_replace);
          access_history_[pg].push_back(access_index_);
          break;
        }

        default:
          break;
      }
    }
  }
}

std::vector<size_t> &PageReplacer::get_frames() {
  //
  return frames_;
}

size_t PageReplacer::get_page_fault() { return page_faults_; }

size_t PageReplacer::get_replace_count() { return replace_count_; }

double PageReplacer::page_fault_ratio() {
  if (access_index_ == 0) {
    return 0.0;
  }
  return static_cast<double>(get_page_fault()) / access_index_;
}

size_t PageReplacer::find_opt() {
  if (!ref_seq_.has_value() || ref_seq_->empty()) {
    throw std::runtime_error("Reference sequence is invalid or empty");
  }
  std::map<size_t, size_t> next_use;
  for (size_t page : frames_) {
    next_use[page] = std::numeric_limits<size_t>::max();
    for (size_t i = access_index_; i < ref_seq_->size(); ++i) {
      if ((*ref_seq_)[i] == page) {
        next_use[page] = i;
        break;
      }
    }
  }
  size_t max_distance = 0;
  size_t replace_idx = 0;
  for (size_t i = 0; i < frames_.size(); ++i) {
    size_t page = frames_[i];
    if (next_use[page] > max_distance) {
      max_distance = next_use[page];
      replace_idx = i;
    }
  }
  return replace_idx;
}

size_t PageReplacer::find_fifo() {
  //
  if (fifo_queue_.empty()) {
    throw std::runtime_error("FIFO Queue Is Empty!!");
  }

  size_t pg_to_find = fifo_queue_.front();

  auto it = std::find(frames_.begin(), frames_.end(), pg_to_find);

  if (it == frames_.end()) {
    throw std::runtime_error("FIFO Page not Found in Frames");
  }
  // 返回该页面在 frames_ 中的idx
  return std::distance(frames_.begin(), it);
}

size_t PageReplacer::find_lru_k() {
  size_t best_idx = 0;  // 最优页面的最后一次访问时间
  size_t min_k_access_time =
      std::numeric_limits<size_t>::max();  // 当前最优页面的第 K 次访问时间
  size_t min_cnt =
      std::numeric_limits<size_t>::max();  // 当前最优页面的历史访问次数
  size_t last_access_tb =
      std::numeric_limits<size_t>::max();  // 最优页面的最后一次访问时间

  for (size_t i = 0; i < frames_.size(); ++i) {
    size_t current_pg = frames_[i];
    const auto &history = access_history_[current_pg];

    size_t cnt = history.size();
    size_t k_access_time;  // 当前页面的 __倒数__ 第 K 次访问时间

    size_t last_access_time =
        history.empty() ? 0 : history.back();  // 当前页面的第 K 次访问时间
    // 2:history:{4,5,7,10} k=2:
    // 倒数第 k=2次就是 his[4-2] = his[2]=7

    // 如果访问次数没有 k 次 设置为0
    /*
    access_history_ = {
      1: [10, 20, 30],
      2: [15, 25],
      3: [5]
    }
    */
    if (cnt < k_value_) {
      k_access_time = 0;
    } else {
      k_access_time = history[cnt - k_value_];
    }
    /*
    优先考虑历史访问次数小于 K 的页面 (说明使用频率低)
    若多个页面都满足此条件,选访问次数最少的
    如果访问次数相同,则选最后一次访问时间最早的
    否则,比较第 K 次访问时间
    越早越好(表示很久没用了)
    如果相同,则看最后一次访问时间(越早越好)
    */
    bool is_better = false;

    // 优先考虑访问次数小于 K 的页面
    if (k_access_time == 0) {
      if (min_k_access_time == 0) {
        if (cnt < min_cnt) {
          is_better = true;
        } else if (cnt == min_cnt) {
          if (last_access_time < last_access_tb) {
            is_better = true;
          }
        }
      } else {
        is_better = true;
      }
    } else {
      if (min_k_access_time != 0) {
        if (k_access_time < min_k_access_time) {
          is_better = true;
        } else if (k_access_time == min_k_access_time) {
          if (last_access_time < last_access_tb) {
            is_better = true;
          }
        }
      }
    }
    if (is_better) {
      min_k_access_time = k_access_time;
      min_cnt = cnt;
      last_access_tb = last_access_time;
      best_idx = i;
    }
  }
  return best_idx;
}
size_t PageReplacer::find_lfu() {
  size_t best_idx = 0;
  size_t min_freq = std::numeric_limits<size_t>::max();  // 最小访问频率
  size_t last_access_tb =
      std::numeric_limits<size_t>::max();  // 对应页面的最后访问时间

  for (size_t i = 0; i < frames_.size(); ++i) {
    size_t current_pg = frames_[i];
    const auto &history = access_history_[current_pg];

    size_t cnt = history.size();
    size_t last_access_time = history.empty() ? 0 : history.back();

    // 如果当前页面访问频率更低,则更新最优页面
    if (cnt < min_freq) {
      min_freq = cnt;
      last_access_tb = last_access_time;
      best_idx = i;
    }
    // 如果访问频率相等,比较最后一次访问时间
    else if (cnt == min_freq) {
      if (last_access_time < last_access_tb) {
        last_access_tb = last_access_time;
        best_idx = i;
      }
    }
  }

  return best_idx;
}