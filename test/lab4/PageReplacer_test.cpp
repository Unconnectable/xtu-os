
#include "lab4/PageReplacer.h"
#include <gtest/gtest.h>
#include <iostream>
#include <vector>

#include <optional>
const std::vector<size_t> opt_test1_ref_seq = {
    53, 4,  83, 79, 19, 25, 68, 33, 91, 25, 22,  57, 31, 81, 39, 59, 49,
    43, 5,  8,  51, 69, 62, 85, 72, 57, 12, 26,  4,  85, 19, 73, 58, 2,
    27, 28, 92, 45, 95, 24, 55, 93, 32, 57, 100, 63, 23, 27, 52, 65, 12,
    4,  48, 41, 32, 75, 64, 84, 46, 80, 14, 21,  70, 92, 87, 95, 38, 36,
    59, 8,  36, 61, 63, 18, 73, 23, 16, 35, 84,  66, 39, 28, 32, 61, 15,
    68, 38, 18, 29, 35, 89, 12, 27, 53, 9,  13,  98, 60, 93, 56};
const std::vector<size_t> fifo_test1_ref_seq = {
    41, 65, 65, 84, 61, 96, 33, 68, 24, 25, 85, 73, 3,  38, 94, 92, 54,
    15, 5,  77, 96, 8,  68, 51, 89, 85, 54, 29, 37, 93, 45, 22, 24, 38,
    58, 90, 61, 98, 93, 35, 49, 99, 92, 92, 74, 6,  87, 80, 89, 29, 50,
    11, 84, 19, 20, 6,  1,  1,  51, 63, 62, 88, 29, 85, 88, 35, 4,  34,
    96, 63, 90, 11, 49, 53, 88, 92, 18, 50, 16, 9,  27, 56, 84, 86, 7,
    32, 5,  30, 29, 55, 72, 94, 93, 80, 99, 94, 85, 65, 24, 72};

const std::vector<size_t> lfu_test2_ref_seq = {
    17, 34, 75, 85, 89, 39, 23, 12, 26, 28, 32, 20, 12, 66, 82, 40,  85,
    34, 80, 40, 27, 63, 25, 17, 53, 45, 92, 95, 24, 21, 28, 66, 47,  37,
    13, 6,  78, 71, 94, 90, 64, 14, 53, 87, 82, 49, 77, 73, 90, 13,  38,
    58, 61, 79, 27, 86, 12, 54, 69, 92, 71, 22, 58, 7,  40, 26, 68,  39,
    46, 80, 14, 17, 86, 10, 30, 61, 58, 92, 65, 1,  84, 47, 68, 100, 99,
    52, 22, 7,  87, 46, 6,  37, 11, 71, 45, 36, 47, 89, 50, 91};

const std::vector<size_t> lruk_test3_ref_seq = {
    31, 20, 25, 61, 90, 38, 67, 32, 52, 72, 83, 78, 51, 24, 97, 77, 10,
    0,  20, 48, 75, 48, 44, 99, 35, 53, 3,  24, 24, 10, 79, 7,  60, 76,
    84, 35, 23, 35, 11, 58, 67, 4,  50, 29, 31, 52, 95, 21, 92, 48, 4,
    57, 31, 62, 72, 82, 90, 98, 63, 3,  85, 89, 3,  9,  62, 77, 65, 48,
    51, 7,  31, 1,  10, 46, 83, 2,  48, 52, 98, 5,  95, 54, 48, 36, 37,
    10, 31, 11, 45, 44, 81, 53, 74, 33, 64, 18, 20, 73, 58, 79};

TEST(PageReplacerOptTest, TEST0) {
  size_t frame_capacity = 3;
  size_t expected_faults = 90;
  size_t expected_replacements = 87;

  PageReplacer replacer(PageReplaceAlgo::OPT, frame_capacity, 2,
                        opt_test1_ref_seq);

  for (size_t page_num : opt_test1_ref_seq) {
    replacer.access_page(page_num);
  }

  ASSERT_EQ(replacer.get_page_fault(), expected_faults);
  ASSERT_EQ(replacer.get_replace_count(), expected_replacements);
}

TEST(PageReplacerFifoTest, TEST1) {
  size_t frame_capacity = 3;
  size_t expected_faults = 96;
  size_t expected_replacements = 93;

  PageReplacer replacer(PageReplaceAlgo::FIFO, frame_capacity, 2,
                        fifo_test1_ref_seq);

  for (size_t page_num : fifo_test1_ref_seq) {
    replacer.access_page(page_num);
  }

  ASSERT_EQ(replacer.get_page_fault(), expected_faults);
  ASSERT_EQ(replacer.get_replace_count(), expected_replacements);
}

TEST(PageReplacerLfuTest, TEST2) {
  size_t frame_capacity = 3;
  size_t expected_faults = 100;
  size_t expected_replacements = 97;

  PageReplacer replacer(PageReplaceAlgo::LFU, frame_capacity, 2,
                        lfu_test2_ref_seq);

  for (size_t page_num : lfu_test2_ref_seq) {
    replacer.access_page(page_num);
  }

  ASSERT_EQ(replacer.get_page_fault(), expected_faults);
  ASSERT_EQ(replacer.get_replace_count(), expected_replacements);
}

TEST(PageReplacerLruKTest, TEST3) {
  size_t frame_capacity = 3;
  size_t k_value = 2;
  size_t expected_faults = 94;
  size_t expected_replacements = 91;
  /* size_t expected_faults = 97;
  size_t expected_replacements = 94;
 */
  PageReplacer replacer(PageReplaceAlgo::LRU_K, frame_capacity, k_value,
                        lruk_test3_ref_seq);

  for (size_t page_num : lruk_test3_ref_seq) {
    replacer.access_page(page_num);
  }

  EXPECT_EQ(replacer.get_page_fault(), expected_faults);
  ASSERT_EQ(replacer.get_replace_count(), expected_replacements);
}