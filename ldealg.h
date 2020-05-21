#ifndef LDEALG_H__
#define LDEALG_H__
#include <vector>
#include <utility>
#include <array>

using namespace std;

struct LDEAlg {
  LDEAlg();
  bool isLeq(const pair<vector<int>, vector<int>> &x, const vector<int> &y);
  bool isLeq(const vector<int> &x, const vector<int> &y);
  void addSolution(const vector<int> &sol);
  bool isMinimal(const vector<int> &sol);
  int getBitMaskFromVector(const vector<int> &sol);

  protected:
    vector<int> a;
    vector<int> b;
    int c;
    vector<pair<vector<int>, vector<int>>> basis;
    array<vector<vector<int>>, 1 << 12> basisByMask;
};

#endif