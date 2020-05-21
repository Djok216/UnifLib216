#ifndef FASTQUERYACUNIFY_H__
#define FASTQUERYACUNIFY_H__

#include <string>
#include <vector>
#include <map>
#include "fastterm.h"
#include "unifeqsystem.h"
using namespace std;

struct FastQueryACUnify {
  FastTerm t1;
  FastTerm t2;

  FastQueryACUnify(FastTerm t1, FastTerm t2);
  std::vector<FastSubst> solve();

private:
  std::vector<FastSubst> solveAC(UnifEq ues);
  void delSameCoeffs(std::map<FastTerm, int> &l, std::map<FastTerm, int> &r);
  std::vector<int> fromMapToVector(const std::map<FastTerm, int> &M);
  FastTerm createFuncWithSameVar(int cnt, FastTerm var, FastFunc f, FastTerm unityElement);
};

#endif
