#include <iostream>
#include <vector>

using std::cin;
using std::cout;
using std::vector;

class BankerAlgo {
public:
  BankerAlgo(int proc, int reso)
      : n(proc), m(reso), Available(reso, 0), MAX(proc, vector<int>(reso, 0)),
        Need(proc, vector<int>(reso, 0)),
        Allocation(proc, vector<int>(reso, 0)), Work(reso, 0) {}

private:
  int n, m;
  vector<vector<int>> MAX;
  vector<vector<int>> Need;
  vector<vector<int>> Allocation;
  vector<int> Work;
  vector<int> Available;
};

int main() {
  BankerAlgo banker(5, 4);
  cout << "Banker's Algorithm initialized successfully.\n";
  return 0;
}