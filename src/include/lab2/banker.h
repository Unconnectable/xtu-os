// BankerAlgo.h
#ifndef BANKERALGO_H
#define BANKERALGO_H

#include <string>
#include <vector>
class BankerAlgo {
 public:
  BankerAlgo(int proc, int reso);

  void setAvail(std::vector<int> &avail);
  void setMAX(std::vector<std::vector<int>> &max_);
  void setAllo(std::vector<std::vector<int>> &alloc);

  bool isSafe();
  bool requestResources(int proce, std::vector<int> &request);
  std::vector<int> get_SafeSeq();

 private:
  int process, resource;
  std::vector<std::vector<int>> MAX;
  std::vector<std::vector<int>> Need;
  std::vector<std::vector<int>> Allocation;
  std::vector<int> Available;
  std::vector<int> SafeSeq;
  std::vector<int> SafeSeq_copy;
};

#endif  // BANKERALGO_H