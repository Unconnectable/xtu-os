#include "gtest/gtest.h"
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include "lab2/banker.h"
using std::vector;
// Include your BankerAlgo header file here
// #include "BankerAlgo.h"


// Assuming vecToString is available, e.g., defined here or in a common test utility header
// Helper function to convert vector of process IDs to a string format "P0 P1 P2..."
std::string vecToString(const std::vector<int>& vec) {
    if (vec.empty()) {
        return "";
    }
    std::string s = "P" + std::to_string(vec[0]);
    for (size_t i = 1; i < vec.size(); ++i) {
        s += " P" + std::to_string(vec[i]);
    }
    return s;
}

TEST(BankerTest, SafeSequenceTest_0) {
  
  BankerAlgo banker(5, 3);
  std::vector<int> avail = {3, 3, 2};
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

  banker.setAvail(avail);
  banker.setMAX(max_);
  banker.setAllo(alloc);
  ASSERT_EQ(true,banker.isSafe());

  ASSERT_EQ(vecToString(banker.get_SafeSeq()), "P1 P2 P3 P4 P0");
}

//不安全
TEST(BankerTest, SafeSequence_1) {
    BankerAlgo banker(5, 4);

    vector<int> avail = {2, 3, 3, 2};
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

    banker.setAvail(avail);
    banker.setMAX(max_);
    banker.setAllo(alloc);

    EXPECT_FALSE(banker.isSafe());
}