#ifndef PAGE_REPLACER_H
#define PAGE_REPLACER_H

#pragma once  // More modern alternative to include guards in supported compilers

#include <algorithm>
#include <limits>
#include <list>
#include <map>
#include <stdexcept>
#include <vector>

// Use an enum class for type safety
enum class PageReplacementAlgorithm {
  OPT,    // Optimal
  FIFO,   // First-In, First-Out
  LRU_K,  // Least Recently Used (K-th access)
  LFU_K   // Least Frequently Used (with K-th access tie-breaker)
};

class PageReplacer {
 public:
  // Constructor
  // algorithm: The page replacement algorithm to use.
  // frame_capacity: The maximum number of pages that can be held in memory frames.
  // k: Parameter K for LRU_K and LFU_K algorithms (ignored for OPT and FIFO).
  // reference_string: The sequence of page accesses (REQUIRED for OPT algorithm).
  //                     A non-owning posize_ter is used, the caller must ensure
  //                     the reference_string outlives the PageReplacer object.
  PageReplacer(PageReplacementAlgorithm algorithm, size_t frame_capacity,
               size_t k = 2,  // Default k=1 for LRU/LFU
               const std::vector<size_t> *reference_string = nullptr);

  // Simulate accessing a page
  // page_number: The page number being accessed.
  void access_page(size_t page_number);

  // Get the total number of page faults that occurred
  size_t get_fault_count() const;

  // Get the current state of the memory frames
  const std::vector<size_t> &get_frames() const;

 private:
  // Configuration
  PageReplacementAlgorithm m_algorithm;
  size_t m_frame_capacity;
  size_t m_k;  // Parameter K for LRU_K and LFU_K

  // Required for OPT algorithm - non-owning posize_ter
  const std::vector<size_t> *m_reference_string;
  size_t m_current_access_index = -1;  // Tracks the index in the reference string for the current access (0-based)

  // Common state
  std::vector<size_t> m_frames;  // Represents the memory frames
  size_t m_page_faults = 0;      // Counter for page faults

  // Algorithm-specific state
  std::list<size_t> m_fifo_queue;  // Stores pages in FIFO order

  // For LRU-K and LFU-K: maps page number to a sorted list of access indices (timestamps)
  std::map<size_t, std::vector<size_t>> m_access_history;

  // Private helper to find the index of a page in m_frames
  // Returns index if found, -1 otherwise.
  size_t find_frame_index(size_t page_number) const;

  // Private helper to find the index in m_frames of the page to be replaced
  // incoming_page: The page number causing the need for replacement (used by OPT).
  // This method delegates to the specific algorithm's logic.
  size_t find_replacement_index(size_t incoming_page);

  // Specific algorithm replacement logic helpers
  size_t find_replacement_index_opt(size_t incoming_page);
  size_t find_replacement_index_fifo();
  size_t find_replacement_index_lru_k();
  size_t find_replacement_index_lfu_k();
};

#endif  // PAGE_REPLACER_H