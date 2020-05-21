#ifndef LDEGRAPHALG_H__
#define LDEGRAPHALG_H__
#include <vector>
#include <utility>

using namespace std;

struct LDEGraphAlg {
  LDEGraphAlg();
  LDEGraphAlg(const vector<int> &a, const vector<int> &b, int c = 0);
  void graphAlg(int diff, int apos, int bpos);
  bool isMinimal(int diff, int apos, int bpos);
  void addSolution(const vector<int> &sol);
  vector<pair<vector<int>, vector<int>>> solve();

  private:
    vector<int> a;
    vector<int> b;
    int c;
    vector<pair<vector<int>, vector<int>>> basis;

    vector<char> viz;
    vector<int> partialSol;
    int offset, sumPartialSol;
};

#endif