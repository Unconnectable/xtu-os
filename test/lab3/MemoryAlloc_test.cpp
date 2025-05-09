#include "lab3/MemoryAlloc.h"
#include <gtest/gtest.h>
#include <iostream>

TEST(MemoryAlloc, TEST0) {
  MemoryManager manager;
  bool result = manager.allocate(1, 100);
  EXPECT_TRUE(result);

  bool found = false;
  for (auto block : manager.getBlocks()) {
    if (!block.is_free && block.job_id.has_value() && block.job_id.value() == 1) {
      found = true;
      ASSERT_EQ(block.size, 100);
      break;
    }
  }
  EXPECT_TRUE(found);
}

TEST(MemoryAlloc, TEST1) {
  MemoryManager manager;

  manager.allocate(1, MEMORY_SIZE - 1);

  bool result = manager.allocate(2, 2);
  EXPECT_FALSE(result);
}

TEST(MemoryAlloc, TEST2) {
  MemoryManager manager;

  // 分配两个任务
  EXPECT_TRUE(manager.allocate(33, 100));
  EXPECT_TRUE(manager.allocate(32, 100));

  for (const auto &block : manager.getBlocks()) {
    if (block.job_id.has_value()) {
      // 已分配块
      size_t jobId = block.job_id.value();
      EXPECT_FALSE(block.is_free);
      EXPECT_EQ(block.size, 100);
      EXPECT_TRUE(jobId == 33 || jobId == 32);
    } else {
      EXPECT_TRUE(block.is_free);
      EXPECT_EQ(block.size, 824);  // 剩余1024 - 2*100
    }
  }
}

TEST(MemoryAlloc, TEST3) {
  MemoryManager manager;
  manager.allocate(1, 100);

  bool result = manager.free(2);  // 不存在的 JobID
  EXPECT_FALSE(result);
}