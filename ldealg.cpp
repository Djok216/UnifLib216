#include <vector>
#include <utility>
#include <algorithm>
#include "ldealg.h"

using namespace std;

LDEAlg::LDEAlg() {}

bool LDEAlg::isLeq(const pair<vector<int>, vector<int>> &x, const vector<int> &y) {
  for (int i = 0; i < (int)a.size(); ++i) {
    if (x.first[i] > y[i]) {
      return false;
    }
  }
  for (int i = 0; i < (int)b.size(); ++i) {
    if (x.second[i] > y[i + a.size()]) {
      return false;
    }
  }
  return true;
}

bool LDEAlg::isLeq(const vector<int> &x, const vector<int> &y) {
  for (int i = 0; i < (int)x.size(); ++i) {
    if (x[i] > y[i]) {
      return false;
    }
  }
  return true;
}

int LDEAlg::getBitMaskFromVector(const vector<int> &sol) {
  int mask = 0;
  for (int i = 0; i < (int)sol.size(); ++i) {
    if (sol[i]) {
      mask |= (1 << i);
    }
  }
  return mask;
}

void LDEAlg::addSolution(const vector<int> &sol) {
  if (*max_element(sol.begin(), sol.end()) != 0) {
    vector<int> x = vector<int>(sol.begin(), sol.begin() + a.size());
    vector<int> y = vector<int>(sol.begin() + a.size(), sol.end());
    basis.emplace_back(x, y);
    basisByMask[getBitMaskFromVector(sol)].push_back(sol);
  }
}

bool LDEAlg::isMinimal(const vector<int> &sol) {
  int firstMask = getBitMaskFromVector(sol);
  for (int mask = firstMask; mask; mask = (mask - 1) & firstMask) {
    for (auto &v : basisByMask[mask]) {
      if (isLeq(v, sol)) {
        return false;
      }
    }
  }
  return true;
}

