#include "lab2/banker.h"
#include <gtest/gtest.h>
#include <vector>
#include <string>

using std::cerr;
using std::endl;
using std::vector;
using std::string;

// 工具函数：将 vector<int> 转换为字符串表示如 "P0 P1"
string vecToString(const vector<int>& v) {
    string res;
    for (size_t i = 0; i < v.size(); ++i) {
        if (i > 0) res += " ";
        res += "P" + std::to_string(v[i]);
    }
    return res;
}

// 课后习题
TEST(BankerTest, DISABLED_SafeSequenceTest_0) {
  
  BankerAlgo banker(5, 3);
  std::vector<int> avail_ = {3, 3, 2};
  std::vector<std::vector<int>> max_ = {
        {7, 4, 3},
        {3, 3, 2},
        {7, 0, 2},
        {2, 2, 2},
        {4, 3, 3}
    };
  std::vector<std::vector<int>> alloc = {
        {0, 1, 0},
        {2, 0, 0},
        {3, 0, 2},
        {2, 1, 1},
        {0, 0, 2}
    };

  banker.setAvail(avail_);
  banker.setMAX(max_);
  banker.setAllo(alloc);
  ASSERT_EQ(true,banker.isSafe());

  ASSERT_EQ(vecToString(banker.get_SafeSeq()), "P1 P2 P3 P4 P0");
}

//不安全
TEST(BankerTest, DISABLED_SafeSequence1) {
    BankerAlgo banker(5, 4);

    vector<int> avail_ = {2, 3, 3, 2};
    vector<vector<int>> max_ = {
        {6, 0, 1, 2},
        {2, 7, 5, 0},
        {2, 3, 5, 4},
        {1, 6, 5, 3},
        {1, 6, 5, 6}
    };
    vector<vector<int>> alloc = {
        {0, 0, 1, 2},
        {1, 0, 0, 0},
        {1, 3, 5, 4},
        {0, 6, 3, 3},
        {0, 0, 1, 4}
    };

    banker.setAvail(avail_);
    banker.setMAX(max_);
    banker.setAllo(alloc);

    EXPECT_FALSE(banker.isSafe());
}
// Test 2: 不安全
TEST(BankerTest, DISABLED_UnsafeSequence_2) {
  BankerAlgo banker(5, 4);
  std::vector<int> avail_ = {1, 0, 2, 0};
  std::vector<std::vector<int>> max_ = { 
      {1, 1, 2, 1}, 
      {2, 1, 3, 1}, 
      {1, 0, 1, 2}, 
      {3, 1, 1, 0}, 
      {2, 0, 2, 1}  
  };
  std::vector<std::vector<int>> alloc = {
      {0, 0, 1, 0}, 
      {1, 0, 1, 0}, 
      {0, 0, 0, 1}, 
      {2, 0, 0, 0}, 
      {0, 0, 1, 0}  
  };

  banker.setAvail(avail_);
  banker.setMAX(max_);
  banker.setAllo(alloc);

  EXPECT_FALSE(banker.isSafe());
}

// Test 3: 安全 
TEST(BankerTest, DISABLED_SafeSequence_3) {
  BankerAlgo banker(5, 4);
  std::vector<int> avail_ = {2, 1, 3, 2};
  std::vector<std::vector<int>> max_ = {
      {5, 2, 4, 3}, 
      {1, 3, 2, 2}, 
      {3, 1, 3, 3}, 
      {2, 4, 3, 1}, 
      {4, 2, 3, 4}  
  };
  std::vector<std::vector<int>> alloc = {
      {1, 0, 1, 1}, 
      {0, 1, 1, 0}, 
      {1, 1, 0, 1}, 
      {1, 2, 1, 0}, 
      {2, 0, 1, 1}  
  };

  banker.setAvail(avail_);
  banker.setMAX(max_);
  banker.setAllo(alloc);

  EXPECT_TRUE(banker.isSafe());
  ASSERT_EQ(vecToString(banker.get_SafeSeq()), "P2 P3 P4 P0 P1");
}

// Test 4: 安全
TEST(BankerTest, DISABLED_SafeSequence_4) {
  BankerAlgo banker(6, 5); 
  std::vector<int> avail_ = {3, 2, 2, 1, 3};
  std::vector<std::vector<int>> max_ = {
      {6, 4, 3, 2, 5}, 
      {2, 1, 2, 3, 1}, 
      {5, 3, 3, 1, 2}, 
      {1, 3, 2, 4, 0}, 
      {4, 2, 1, 0, 3}, 
      {3, 3, 3, 3, 3}  
  };
  std::vector<std::vector<int>> alloc = {
      {2, 1, 0, 1, 1}, 
      {1, 0, 1, 1, 0}, 
      {1, 1, 2, 0, 1}, 
      {0, 1, 1, 2, 0}, 
      {2, 1, 0, 0, 1}, 
      {1, 1, 1, 0, 1}  
  };

  banker.setAvail(avail_);
  banker.setMAX(max_);
  banker.setAllo(alloc);

  EXPECT_TRUE(banker.isSafe());
  ASSERT_EQ(vecToString(banker.get_SafeSeq()), "P4 P2 P0 P1 P3 P5");
}

// Test 5: 单个请求合法
TEST(BankerTest, DISABLED_Request_5_SingleValid) {
  BankerAlgo banker(5, 3);
  std::vector<int> avail_t5 = {3, 3, 2};
  std::vector<std::vector<int>> max_t5 = {
      {7, 5, 3}, {3, 2, 2}, {9, 0, 2}, {2, 2, 2}, {4, 3, 3}
  };
  std::vector<std::vector<int>> alloc_t5 = {
      {0, 1, 0}, {2, 0, 0}, {3, 0, 2}, {2, 1, 1}, {0, 0, 2}
  };
  banker.setAvail(avail_t5);
  banker.setMAX(max_t5);
  banker.setAllo(alloc_t5);
  ASSERT_TRUE(banker.isSafe()); 

  std::vector<int> request_p0 = {1, 1, 0};
  EXPECT_TRUE(banker.requestResources(0, request_p0)); 

  EXPECT_TRUE(banker.isSafe()); 
  ASSERT_EQ(vecToString(banker.get_SafeSeq()), "P1 P3 P4 P0 P2");
}

// Test 6: 多个请求合法 
TEST(BankerTest, DISABLED_Request_6_MultipleValid) {
  BankerAlgo banker(5, 3);
  std::vector<int> avail_ = {3, 3, 2};
  std::vector<std::vector<int>> max_ = {
      {7, 5, 3}, {3, 2, 2}, {9, 0, 2}, {2, 2, 2}, {4, 3, 3}
  };
  std::vector<std::vector<int>> alloc_orig = {
      {0, 1, 0}, {2, 0, 0}, {3, 0, 2}, {2, 1, 1}, {0, 0, 2}
  };
  banker.setAvail(avail_);
  banker.setMAX(max_);
  banker.setAllo(alloc_orig);

  
  std::vector<int> request1_p0 = {1, 1, 0};
  ASSERT_TRUE(banker.requestResources(0, request1_p0));
 
  std::vector<int> request2_p4 = {1, 0, 0};
  EXPECT_TRUE(banker.requestResources(4, request2_p4));

  EXPECT_TRUE(banker.isSafe());
  ASSERT_EQ(vecToString(banker.get_SafeSeq()), "P1 P3 P4 P0 P2");
}

// Test 7: 多个请求存在 1个非法
TEST(BankerTest, DISABLED_Request_7_OneInvalid) {
  BankerAlgo banker(5, 3);
  std::vector<int> avail_ = {3, 3, 2};
  std::vector<std::vector<int>> max_ = {
      {7, 5, 3}, {3, 2, 2}, {9, 0, 2}, {2, 2, 2}, {4, 3, 3}
  };
  std::vector<std::vector<int>> alloc_orig = {
      {0, 1, 0}, {2, 0, 0}, {3, 0, 2}, {2, 1, 1}, {0, 0, 2}
  };
  banker.setAvail(avail_);
  banker.setMAX(max_);
  banker.setAllo(alloc_orig);

  std::vector<int> request1_p0 = {1, 1, 0};
  ASSERT_TRUE(banker.requestResources(0, request1_p0));

  std::vector<int> request2_p2_unsafe = {10, 0, 0};
  EXPECT_FALSE(banker.requestResources(2, request2_p2_unsafe));


  EXPECT_TRUE(banker.isSafe());
  ASSERT_EQ(vecToString(banker.get_SafeSeq()), "P1 P3 P4 P0 P2");
}

// Test 8: 多个请求存在多个非法
TEST(BankerTest, DISABLED_Request_8_MultipleInvalid) {
  
  BankerAlgo banker(6, 6);  
  std::vector<int> avail_ = {10, 9, 2, 1, 0, 0};
  std::vector<std::vector<int>> max_ = {
      {2, 2, 2, 2, 0, 0},     
      {3, 3, 3, 3, 0, 0},     
      {1, 1, 1, 1, 0, 0},     
      {4, 4, 4, 4, 0, 0},     
      {2, 2, 2, 2, 0, 0},     
      {1, 0, 0, 0, 0, 0}      
  };

  
  std::vector<std::vector<int>> alloc_orig = {
      {1, 1, 1, 1, 0, 0},     
      {2, 2, 2, 2, 0, 0},     
      {1, 0, 0, 0, 0, 0},     
      {3, 3, 3, 3, 0, 0},     
      {1, 1, 1, 1, 0, 0},     
      {0, 0, 0, 0, 0, 0}      
  };

  // 设置资源状态
  banker.setAvail(avail_);
  banker.setMAX(max_);
  banker.setAllo(alloc_orig);

  ASSERT_EQ(true,banker.isSafe());


  std::vector<int> req1 = {2, 0, 0, 0, 0, 0};
  EXPECT_FALSE(banker.requestResources(0, req1));  

  std::vector<int> req3 = {10, 0, 0, 0, 0, 0}; 
  EXPECT_FALSE(banker.requestResources(5, req3));

  std::vector<int> req4 = {-1, 0, 0, 0, 0, 0};
  EXPECT_FALSE(banker.requestResources(0, req4));

  // 非法请求 5: 资源总数不匹配
  std::vector<int> req5 = {1, 1}; // 不足 6 个资源维度
  EXPECT_FALSE(banker.requestResources(0, req5));
}

// Test 9: 本身系统不合法 
TEST(BankerTest, DISABLED_InitialState_9_InvalidSystem) {
  BankerAlgo banker(5, 4);
  std::vector<int> avail_ = {1, 1, 1, 1}; 
  std::vector<std::vector<int>> max_ = { 
      {2, 2, 2, 2}, 
      {3, 3, 3, 3}, 
      {1, 1, 1, 1},  
      {4, 4, 4, 4}, 
      {2, 2, 2, 2}  
  };
  std::vector<std::vector<int>> alloc = { // Currently allocated
      {1, 1, 1, 1}, 
      {1, 1, 1, 1}, 
      {2, 0, 0, 0},
      {1, 1, 1, 1}, 
      {1, 1, 0, 0}  
  };

  banker.setAvail(avail_);
  banker.setMAX(max_);
  banker.setAllo(alloc); 

  EXPECT_TRUE(banker.isSafe());
}
// 测试用例 10: 边界情况
TEST(BankerTest, DISABLED_EdgeCase) {
    BankerAlgo banker(1, 1);

    vector<int> avail_ = {1};
    vector<vector<int>> max_ = {{1}};
    vector<vector<int>> alloc = {{0}};

    banker.setAvail(avail_);
    banker.setMAX(max_);
    banker.setAllo(alloc);

    EXPECT_TRUE(banker.isSafe());

    EXPECT_EQ(vecToString(banker.get_SafeSeq()), "P0");
}