// BankerAlgo.cpp
#include "lab2/banker.h"
#include <iostream>
#include <vector>

using std::cerr;
using std::endl;
using std::vector;

BankerAlgo::BankerAlgo(int proc, int reso)
    : process(proc),
      resource(reso),
      Available(reso, 0),
      MAX(proc, vector<int>(reso, 0)),
      Need(proc, vector<int>(reso, 0)),
      Allocation(proc, vector<int>(reso, 0)) {}

void BankerAlgo::setAvail(vector<int> &avail) { Available = avail; }

void BankerAlgo::setMAX(vector<vector<int>> &max_) { MAX = max_; }

void BankerAlgo::setAllo(vector<vector<int>> &alloc) {
  Allocation = alloc;
  for (int i = 0; i < process; ++i) {
    for (int j = 0; j < resource; ++j) {
      Need[i][j] = MAX[i][j] - Allocation[i][j];
    }
  }
}

bool BankerAlgo::isSafe() {
  SafeSeq.clear();
  vector<int> Work = Available;
  vector<bool> finish(process, false);
  int cnt = 0;

  while (cnt < process) {
    bool ok = false;
    for (int i = 0; i < process; ++i) {
      if (!finish[i]) {
        bool canAllocate = true;
        for (int j = 0; j < resource; ++j) {
          if (Need[i][j] > Work[j]) {
            canAllocate = false;
            break;
          }
        }

        if (canAllocate) {
          for (int j = 0; j < resource; ++j) {
            Work[j] += Allocation[i][j];
          }
          finish[i] = true;
          SafeSeq.push_back(i);
          cnt++;
          ok = true;
        }
      }
    }

    if (!ok) {
      SafeSeq.clear();
      return false;
    }
  }
  return true;
}

bool BankerAlgo::requestResources(int proce, vector<int> &request) {
  if (proce < 0 || request.size()!=resource) {
    cerr << "Process num Wrong!";
    return false;
  }
  // 线程 proce 请求资源分配
  for (int j = 0; j < resource; ++j) {
    if (request[j] > Need[proce][j] || request[j] > Available[j] || request[j] < 0) {
      //cerr << "Error: Request exceeds need or Available" << endl;
      return false;
    }
  }

  for (int j = 0; j < resource; ++j) {
    Available[j] -= request[j];
    Allocation[proce][j] += request[j];
    Need[proce][j] -= request[j];
  }

  auto roolback = [&]() {
    for (int j = 0; j < resource; ++j) {
      Available[j] += request[j];
      Allocation[proce][j] -= request[j];
      Need[proce][j] += request[j];
    }
  };
  if (isSafe()) {
    roolback();
    return true;
  } else {
    // Rollback
    roolback();
    return false;
  }
}

std::vector<int> BankerAlgo::get_SafeSeq() { return SafeSeq; }
