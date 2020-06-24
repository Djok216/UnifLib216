#ifndef ACUNIFIER_H__
#define ACUNIFIER_H__

#include <string>
#include <vector>
#include <map>
#include "fastterm.h"
#include "unifeqsystem.h"
using namespace std;

struct ACUnifier {
  FastTerm t1;
  FastTerm t2;

  ACUnifier(FastTerm t1 = 0, FastTerm t2 = 0);
  std::vector<FastSubst> solve();
  std::vector<FastSubst> solve(UnifEqSystem ues);

private:
  std::vector<FastSubst> solveAC(UnifEq ues);
  void delSameCoeffs(std::map<FastTerm, int> &l, std::map<FastTerm, int> &r);
  std::vector<int> fromMapToVector(const std::map<FastTerm, int> &M);
  FastTerm createFuncWithSameVar(int cnt, FastTerm var, FastFunc f, FastTerm unityElement);
};

#endif
