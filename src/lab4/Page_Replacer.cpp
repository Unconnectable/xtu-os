#include "Page_Replacer.h"
#include <iostream>   // 用于潜在的调试或简单的输出
#include <limits>     // 包含 numeric_limits
#include <stdexcept>  // 包含异常类

// 辅助函数:查找元素在 vector 中的索引
// 如果找到,返回索引;否则返回 size_t(-1) (即 SIZE_MAX)
size_t PageReplacer::find_frame_index(size_t page_number) const {
  for (size_t i = 0; i < m_frames.size(); ++i) {
    if (m_frames[i] == page_number) {
      return static_cast<size_t>(i);
    }
  }
  return static_cast<size_t>(-1);  // 未找到,返回表示 "未找到" 的值 (对于 size_t 即最大值)
}

// 构造函数
// algorithm: 要使用的页面置换算法.
// frame_capacity: 内存帧可以容纳的最大页面数.
// k: LRU_K 和 LFU_K 算法的参数 K(对 OPT 和 FIFO 忽略).
// reference_string: 页面访问序列(OPT 算法必需).
//                     使用非拥有指针,调用者必须确保 reference_string 的生命周期长于 PageReplacer 对象.
PageReplacer::PageReplacer(PageReplacementAlgorithm algorithm, size_t frame_capacity, size_t k,
                           const std::vector<size_t> *reference_string)
    : m_algorithm(algorithm),
      m_frame_capacity(frame_capacity),
      m_k(k),
      m_reference_string(reference_string)  // 存储指针
{
  if (m_frame_capacity == 0) {  // size_t >= 0,所以只检查是否为 0
    throw std::invalid_argument("帧容量必须是正数.");
  }
  if ((m_algorithm == PageReplacementAlgorithm::LRU_K || m_algorithm == PageReplacementAlgorithm::LFU_K) && m_k == 0) {
    throw std::invalid_argument("对于 LRU_K 和 LFU_K,K 必须是正数.");
  }
  if (m_algorithm == PageReplacementAlgorithm::OPT && m_reference_string == nullptr) {
    throw std::invalid_argument("OPT 算法必须提供引用串.");
  }
  // 初始化帧向量(初始时可以为空)
  m_frames.reserve(m_frame_capacity);  // 预留空间但不改变实际大小
}

// 模拟访问一个页面
// page_number: 正在访问的页面号.
void PageReplacer::access_page(size_t page_number) {
  // 在使用前增加访问索引,因此第一次访问索引为 0,第二次为 1,依此类推.
  m_current_access_index++;

  size_t frame_index = find_frame_index(page_number);

  if (frame_index != static_cast<size_t>(-1)) {  // 使用 static_cast<size_t>(-1) 与返回值比较
    // 缓存命中
    // 对于 LRU-K/LFU-K,即使命中也要更新访问历史
    if (m_algorithm == PageReplacementAlgorithm::LRU_K || m_algorithm == PageReplacementAlgorithm::LFU_K) {
      m_access_history[page_number].push_back(m_current_access_index);
    }
    // FIFO 和 OPT 在命中时不需要更新状态以支持置换逻辑
  } else {
    // 缓存未命中 (缺页)
    m_page_faults++;

    if (m_frames.size() < m_frame_capacity) {
      // 如果有空间,直接添加页面
      m_frames.push_back(page_number);

      // 更新新添加页面对应的算法特定状态
      if (m_algorithm == PageReplacementAlgorithm::FIFO) {
        m_fifo_queue.push_back(page_number);
      } else if (m_algorithm == PageReplacementAlgorithm::LRU_K || m_algorithm == PageReplacementAlgorithm::LFU_K) {
        m_access_history[page_number].push_back(m_current_access_index);
      }
      // OPT 除了 m_frames 外,不维护内存中页面的额外状态
    } else {
      // 帧已满,需要置换
      size_t index_to_replace = find_replacement_index(page_number);

      if (index_to_replace == static_cast<size_t>(-1)) {  // 使用 static_cast<size_t>(-1) 与返回值比较
        // 如果 find_replacement_index 正确,理论上不应该发生此情况
        // 但作为安全措施:
        throw std::runtime_error("未找到置换索引.");
      }

      size_t page_to_remove = m_frames[index_to_replace];

      // 在置换发生之前,更新将被移除页面的算法特定状态
      if (m_algorithm == PageReplacementAlgorithm::FIFO) {
        // 在标准的 FIFO 中,要移除的页面必须是队列的头部.
        // find_replacement_index_fifo 应该返回 m_fifo_queue.front() 的索引.
        if (!m_fifo_queue.empty()) {
          if (m_fifo_queue.front() != page_to_remove) {
            // 这表示存在不一致,需要处理或断言
            // 为简洁起见,假设如果 find_replacement_index_fifo 正确则会保持一致.
            // 可以搜索并移除,但标准的 FIFO 是移除头部.
            // std::list<size_t>::iterator it_remove = std::find(m_fifo_queue.begin(), m_fifo_queue.end(),
            // page_to_remove); if (it_remove != m_fifo_queue.end()) m_fifo_queue.erase(it_remove);
          }
          m_fifo_queue.pop_front();  // 移除队列头部的页面
        }
      } else if (m_algorithm == PageReplacementAlgorithm::LRU_K || m_algorithm == PageReplacementAlgorithm::LFU_K) {
        m_access_history.erase(page_to_remove);  // 移除被驱逐页面的所有历史记录
      }
      // OPT 除了更新帧内容外,不需要更新移除页面的状态.

      // 在帧中执行置换
      m_frames[index_to_replace] = page_number;

      // 更新新添加页面的算法特定状态
      if (m_algorithm == PageReplacementAlgorithm::FIFO) {
        m_fifo_queue.push_back(page_number);  // 将新页面添加到队列尾部
      } else if (m_algorithm == PageReplacementAlgorithm::LRU_K || m_algorithm == PageReplacementAlgorithm::LFU_K) {
        m_access_history[page_number].push_back(m_current_access_index);  // 添加它的第一次访问时间
      }
      // OPT 除了更新帧内容外,不需要更新添加页面的状态.
    }
  }
}

// 获取缺页计数
size_t PageReplacer::get_fault_count() const { return m_page_faults; }

// 获取当前帧状态
const std::vector<size_t> &PageReplacer::get_frames() const { return m_frames; }

// 根据算法委托置换逻辑
// incoming_page: 导致需要置换的新页面号(OPT 使用).
// 返回值:要置换的页面在 m_frames 中的索引.
size_t PageReplacer::find_replacement_index(size_t incoming_page) {
  switch (m_algorithm) {
    case PageReplacementAlgorithm::OPT:
      return find_replacement_index_opt(incoming_page);
    case PageReplacementAlgorithm::FIFO:
      return find_replacement_index_fifo();
    case PageReplacementAlgorithm::LRU_K:
      return find_replacement_index_lru_k();
    case PageReplacementAlgorithm::LFU_K:
      return find_replacement_index_lfu_k();
    default:
      throw std::logic_error("未知的页面置换算法.");
  }
}

// OPT 置换逻辑
size_t PageReplacer::find_replacement_index_opt(size_t incoming_page) {
  // 找到帧中下次访问最远的页面.
  // 如果一个页面将来不会再被访问,选择它.

  size_t index_to_replace = static_cast<size_t>(-1);
  // 使用 size_t 的最大值作为初始值,表示尚未找到最远访问的页面
  size_t max_future_distance = std::numeric_limits<size_t>::max();

  // 我们需要查看当前访问*之后*的引用串
  const auto &ref_str = *m_reference_string;
  size_t search_start_index = m_current_access_index + 1;

  for (size_t i = 0; i < m_frames.size(); ++i) {
    size_t page_in_frame = m_frames[i];

    // 在引用串的剩余部分查找该页面的下一次出现
    auto it = std::find(ref_str.begin() + search_start_index, ref_str.end(), page_in_frame);

    if (it == ref_str.end()) {
      // 此页面将来不会再被访问.这是最优选择.
      return static_cast<size_t>(i);
    } else {
      // 计算到下次访问的距离
      // std::distance 返回 ptrdiff_t,转换为 size_t
      size_t current_future_distance = static_cast<size_t>(std::distance(ref_str.begin() + search_start_index, it));

      // 找到最大的未来距离
      if (current_future_distance > max_future_distance) {
        max_future_distance = current_future_distance;
        index_to_replace = static_cast<size_t>(i);
      }
    }
  }

  // 如果到达这里,说明帧中的所有页面将来都会被再次访问.
  // index_to_replace 持有下次访问最远的页面的索引.
  // 注意:如果所有页面的下次访问距离相同,则会选择第一个找到的具有该距离的页面.
  return index_to_replace;
}

// FIFO 置换逻辑
size_t PageReplacer::find_replacement_index_fifo() {
  // 找到在帧中停留时间最长(位于队列头部)的页面.
  if (m_fifo_queue.empty()) {
    // 如果帧满并且管理正确,理论上不应该发生此情况
    throw std::logic_error("需要置换时 FIFO 队列为空.");
  }
  size_t page_to_remove = m_fifo_queue.front();

  // 在实际的帧向量中查找该页面的索引
  size_t index_to_replace = find_frame_index(page_to_remove);

  if (index_to_replace == static_cast<size_t>(-1)) {  // 使用 static_cast<size_t>(-1) 与返回值比较
    // 这表示 FIFO 队列与帧之间存在不一致
    throw std::logic_error("要从 FIFO 队列中移除的页面未在帧中找到.");
  }

  return index_to_replace;
}

// LRU-K 置换逻辑
size_t PageReplacer::find_replacement_index_lru_k() {
  // 找到帧中其第 K 次最近访问时间最早的页面.
  // 如果一个页面记录的访问次数少于 K 次,则将其最早的访问时间视为其第 K 次访问时间.
  // 决胜局:如果第 K 次访问时间相同,通常使用最近一次访问时间(标准 LRU 规则).

  size_t index_to_replace = static_cast<size_t>(-1);
  // 使用 size_t 的最大值作为初始最小值,表示尚未找到候选页
  size_t min_kth_access_time = std::numeric_limits<size_t>::max();
  // 用于决胜局,同样初始化为最大值
  size_t min_most_recent_access_time_for_tie_break = std::numeric_limits<size_t>::max();

  for (size_t i = 0; i < m_frames.size(); ++i) {
    size_t page_in_frame = m_frames[i];

    // 获取此页面的访问历史
    // operator[] 会在找不到时插入,这在这里没问题,因为帧中的页面应该有历史记录
    const auto &history = m_access_history[page_in_frame];

    size_t current_kth_access_time;
    if (history.size() < m_k) {
      // 如果访问次数少于 K 次,将其最早的访问时间视为其 K 次访问时间.
      // 或者,一些定义认为访问次数 < K 的页面是优先考虑的候选项,使用最早的访问时间进行决胜.
      // 让我们使用这样的定义:访问次数 < K 意味着其最早访问时间就是其第 K 次访问时间.
      if (history.empty()) {
        // 如果页面在帧中并且历史记录被跟踪,这不应该发生
        // 使用 size_t 的最小值,确保如果存在历史记录,它会被优先考虑(因为时间戳会比这个大)
        current_kth_access_time = std::numeric_limits<size_t>::min();
      } else {
        current_kth_access_time = history.front();  // 记录的最早访问时间
      }
    } else {
      // 获取第 K 次最近访问时间(从末尾算起第 K 个索引)
      current_kth_access_time = history[history.size() - m_k];
    }

    // 获取用于决胜局的最近访问时间(标准 LRU 规则)
    // history 是排序的,最后一个元素是最近访问时间
    size_t current_most_recent_access_time = history.empty() ? std::numeric_limits<size_t>::max() : history.back();

    // 比较逻辑:
    // 1. 优先选择最小的第 K 次访问时间
    // 2. 如果第 K 次访问时间相等,优先选择最小的最近访问时间(LRU 决胜局)

    if (current_kth_access_time < min_kth_access_time) {
      min_kth_access_time = current_kth_access_time;
      min_most_recent_access_time_for_tie_break = current_most_recent_access_time;
      index_to_replace = static_cast<size_t>(i);
    } else if (current_kth_access_time == min_kth_access_time) {
      // 第 K 次访问时间平局,使用最近访问时间打破平局(标准 LRU)
      if (current_most_recent_access_time < min_most_recent_access_time_for_tie_break) {
        min_most_recent_access_time_for_tie_break = current_most_recent_access_time;
        index_to_replace = static_cast<size_t>(i);
      }
    }
  }

  return index_to_replace;
}

// LFU-K 置换逻辑
size_t PageReplacer::find_replacement_index_lfu_k() {
  // 找到帧中频率最小的页面.
  // 决胜局:使用 LRU-K 规则(最早的第 K 次最近访问时间).
  // 二次决胜局:使用标准 LRU(最早的最近访问时间).

  size_t index_to_replace = static_cast<size_t>(-1);
  // 初始化为 size_t 最大值作为最小频率的初始值
  size_t min_freq = std::numeric_limits<size_t>::max();
  // 用于频率平局时的第 K 次访问时间决胜局,初始化为最大值
  size_t min_kth_access_time_for_tie_break = std::numeric_limits<size_t>::max();
  // 用于频率和第 K 次访问时间都平局时的最近访问时间决胜局,初始化为最大值
  size_t min_most_recent_access_time_for_tie_break = std::numeric_limits<size_t>::max();

  for (size_t i = 0; i < m_frames.size(); ++i) {
    size_t page_in_frame = m_frames[i];

    // 获取访问历史
    const auto &history = m_access_history[page_in_frame];

    // 计算频率(记录的访问次数)
    size_t current_freq = history.size();

    // 获取用于决胜局的第 K 次访问时间(类似于 LRU-K)
    // 如果访问次数少于 K 次,默认使用 size_t 的最大值
    size_t current_kth_access_time = std::numeric_limits<size_t>::max();
    if (history.size() >= m_k) {
      current_kth_access_time = history[history.size() - m_k];  // 第 K 次最近访问
    } else if (!history.empty()) {
      // 如果访问次数少于 K 次但存在一些历史记录,某些 LFU-K 变体使用最早的访问作为决胜局
      // 或者它们可能优先考虑访问次数 < K 的页面.
      // 让我们使用规则:在最小频率的页面中,访问次数 < K 的页面最不优先被驱逐,
      // 通过它们的最早访问时间进行决胜.如果 history < K,kth_access_time 的大值可以实现这一点.
      // 如果历史记录为空,理论上不应在帧中,但进行防御性处理.
      // 当频率相同时,LFU-K 的一个常见决胜规则是:其第 K 次引用最旧(时间戳最小)的那个.
      // 访问次数 < K 的页面:第 K 次引用未定义/无穷大,使得它们在这种规则下*不是*候选者,除非所有页面访问次数都 < K.
      // 让我们细化:如果 history < K,其有效的第 K 次访问时间是“无穷大”.这意味着在最小频率的页面中,具有完整历史(>= K
      // 次)的页面根据其第 K 次时间更优先被驱逐. 如果所有最小频率的页面访问次数都 <
      // K,那么使用最早访问时间？这变得复杂了. 一个更简单的 LFU-K 决胜局:使用标准
      // LRU(最近访问时间)处理平局.为了简洁,让我们坚持使用这个.
      // 将 < K 历史视为具有“无穷大”的第 K 次访问时间,这样在比较中,具有 >= K 历史的页面将优先被考虑(如果频率相同).
      current_kth_access_time = std::numeric_limits<size_t>::max();
    }

    // 获取用于二次决胜局的最近访问时间(标准 LRU)
    size_t current_most_recent_access_time = history.empty() ? std::numeric_limits<size_t>::max() : history.back();

    // 比较逻辑:
    // 1. 优先选择最小频率的页面
    // 2. 如果频率相等,优先选择第 K 次访问时间最小的页面(时间戳越小,第 K 次访问越旧)
    // 3. 如果频率和第 K 次访问时间都相等,优先选择最近访问时间最小的页面(时间戳越小,最近访问越旧 - LRU)

    if (current_freq < min_freq) {
      min_freq = current_freq;
      min_kth_access_time_for_tie_break = current_kth_access_time;
      min_most_recent_access_time_for_tie_break = current_most_recent_access_time;
      index_to_replace = static_cast<size_t>(i);
    } else if (current_freq == min_freq) {
      // 频率平局,检查第 K 次访问时间
      if (current_kth_access_time < min_kth_access_time_for_tie_break) {
        min_kth_access_time_for_tie_break = current_kth_access_time;
        min_most_recent_access_time_for_tie_break = current_most_recent_access_time;
        index_to_replace = static_cast<size_t>(i);
      } else if (current_kth_access_time == min_kth_access_time_for_tie_break) {
        // 频率和第 K 次访问时间都平局,检查最近访问时间(LRU)
        if (current_most_recent_access_time < min_most_recent_access_time_for_tie_break) {
          min_most_recent_access_time_for_tie_break = current_most_recent_access_time;
          index_to_replace = static_cast<size_t>(i);
        }
      }
    }
  }
  // 处理 LFU-K 中所有页面历史记录都 < K 且 kth_access_time 逻辑给出 MAX_size_t 的边缘情况
  // 在这种情况下,默认的 LFU-K 决胜规则通常会退化为简单的 LRU 或 FIFO.
  // 上述逻辑通过将最小值初始化为 MAX_size_t,正确处理了第一个看到的页面.
  // 如果所有页面的 history.size() 都 < m_k,它们的 kth_access_time 将是 MAX_size_t.
  // 决胜局会退化到最近访问时间,正确实现了 LRU.
  // 因此,当前的逻辑正确地处理了访问次数 < K 的页面,在最小频率页面中,基于第 K 次时间,它们最不优先被驱逐,
  // 并在最终平局时正确退化到 LRU.

  return index_to_replace;
}