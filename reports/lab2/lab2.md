# 实验报告

## 基本信息

- **班级**: 计科 23 级 1 班
- **学号**: 202305133352
- **姓名**: 周潍可
- **实验名称**: 银行家算法

## 1. 实验目的

> 本次实验旨在理解和掌握操作系统中进程资源分配的安全性检测机制——银行家算法.通过编程实现资源请求检测、系统安全性检验等关键功能,深入理解死锁预防策略中的资源分配限制方法.实验要求能够判断系统的当前状态是否安全,以及在某个进程提出资源请求时,决定是否允许该请求,以避免系统进入死锁状态.

> 本实验中使用了以下数据结构:
>
> - Available: 一维数组,表示系统当前可分配的各类资源数量.
> - Max: 二维数组,表示每个进程对每类资源的最大需求.
> - Allocation: 二维数组,表示当前已为各进程分配的资源数量.
>   - Need: 二维数组,表示每个进程尚需的资源数量,由 Need[i][j] = Max[i][j] - Allocation[i][j] 动态计算得出.
>   - Work: 临时变量,在安全检验过程中用于模拟资源回收过程.
>   - Finish: 布尔数组,记录每个进程是否可以在当前状态下完成.
>   - SafeSeq: 一维数组,保存一个可能的安全执行序列(若存在).
>   - request: 一维数组,表示某个进程提出的资源请求向量.
>   - 这些数据结构共同模拟了银行家算法的核心逻辑,实现了资源分配与安全检查的功能.

## 3.处理过程(流程图或盒图)

![lab2](/assets/lab2.png)

## 4.源代码

```cpp
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
```

```cpp
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

```

## 5. 实验总结

> 通过本次实验,我深入了解了银行家算法的基本原理与实现方式.在编写程序的过程中,我掌握了如何构建资源分配模型、进行安全性检测以及处理进程的资源请求.

> 在实现过程中,遇到的主要问题包括:

> - 如何动态维护 Need 矩阵; -安全性检验中可能出现的无限循环或死锁判定错误;
> - 多个资源请求下的边界条件判断;
> - 请求处理后需要回滚以保持系统状态一致性.
> - 通过合理设计类结构、严格校验输入请求、递归查找可执行进程并模拟资源回收,最终成功实现了银行家算法的关键功能.

> 实验测试表明,该程序能够准确判断系统状态是否安全,并在资源请求合法的情况下作出正确响应.此外,程序具备良好的健壮性,能够处理各种边界情况及非法请求.
