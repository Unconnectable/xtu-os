#include <cstdio>
#include <iostream>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::vector;

class BankerAlgo {
public:
  BankerAlgo(int proc, int reso)
      : process(proc),//
       resource(reso),//
        Available(reso, 0),//
        MAX(proc, vector<int>(reso, 0)), //
        Need(proc, vector<int>(reso, 0)),//
        Allocation(proc, vector<int>(reso, 0)) {}

  void setAvail(vector<int> &avail) { Available = avail; }
  void setMAX(vector<vector<int>> &max_) { MAX = max_; }

  void setAllo(vector<vector<int>> &alloc) {
    Allocation = alloc;
    for (int i = 0; i < process; ++i) {
      for (int j = 0; j < resource; ++j) {
        Need[i][j] = MAX[i][j] - Allocation[i][j];
      }
    }
  }

  bool isSafe() {
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
      if (!ok)
        return false;
    }
    return true;
  }

  bool RequestReso(int proce, vector<int> &request) {
    // Step 1: Check if Need >= request
    for (int j = 0; j < resource; ++j) {
      if (request[j] > Need[proce][j]) {
        cout << "Error: Request exceeds need." << endl;
        return false;
      }
    }

    // Step 2: Check if Available >= request
    for (int j = 0; j < resource; ++j) {
      if (request[j] > Available[j]) {
        cout << "Error: Resources not available." << endl;
        return false;
      }
    }

    // Step 3: Try allocation
    for (int j = 0; j < resource; ++j) {
      Available[j] -= request[j];
      Allocation[proce][j] += request[j];
      Need[proce][j] -= request[j];
    }

    // Step 4: Check safety
    if (isSafe()) {
      cout << "Request granted. System remains safe." << endl;
      return true;
    } else {
      // Step 5: Rollback
      cout << "Request denied. System would be unsafe. Rolling back..." << endl;
      for (int j = 0; j < resource; ++j) {
        Available[j] += request[j];
        Allocation[proce][j] -= request[j];
        Need[proce][j] += request[j];
      }
      return false;
    }
  }

  void print() {
    cout << "Available:\n";
    for (auto x : Available)
      cout << x << " ";
    cout << "\n\n";

    cout << "MAX Matrix:\n";
    for (const auto &row : MAX) {
      for (auto val : row)
        cout << val << " ";
      cout << "\n";
    }
    cout << "\n";

    cout << "Allocation Matrix:\n";
    for (const auto &row : Allocation) {
      for (auto val : row)
        cout << val << " ";
      cout << "\n";
    }
    cout << "\n";

    cout << "Need Matrix:\n";
    for (const auto &row : Need) {
      for (auto val : row)
        cout << val << " ";
      cout << "\n";
    }
    cout << "\n";

    if (!SafeSeq.empty()) {
      cout << "Safe Sequence: ";
      for (auto p : SafeSeq)
        cout << "P" << p << " ";
      cout << "\n";
    } else {
      cout << "No safe sequence ok.\n";
    }
  }

private:
  int process, resource;
  vector<vector<int>> MAX;
  vector<vector<int>> Need;
  vector<vector<int>> Allocation;
  vector<int> Available;
  vector<int> SafeSeq;
};

// ===================== 主函数测试逻辑 =====================
int main() {
  BankerAlgo banker(5, 3); // 5 processes, 3 resources
  vector<int> avail = {3, 3, 2};
  vector<vector<int>> max = {
      {7, 5, 3},
      {3, 2, 2},
      {9, 0, 2},
      {2, 2, 2},
      {4, 3, 3}
  };
  vector<vector<int>> alloc = {
      {0, 1, 0},
      {2, 0, 0},
      {3, 0, 2},
      {2, 1, 1},
      {0, 0, 2}
  };
  banker.setAvail(avail);
  banker.setMAX(max);
  banker.setAllo(alloc);

  cout << "Checking initial state...\n";
  if (banker.isSafe()) {
    cout << "System is in a safe state.\n";
  } else {
    cout << "System is NOT in a safe state.\n";
  }

  cout << "\nInitial Resource State:\n";
  banker.print();

  cout << "\nTesting resource request from Process 1...\n";
  vector<int> req1 = {1, 0, 2};
  banker.RequestReso(1, req1);

  cout << "\nAfter request, current state:\n";
  banker.print();

  return 0;
}