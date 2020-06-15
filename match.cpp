#include "fastterm.h"
#include "fastqueryacunify.h"
#include <cassert>
#include <vector>
#include <algorithm>
#include <functional>
#include <string>
#include <string.h>

using namespace std;

// bool matchList(FastTerm *tl1, FastTerm *tl2, uint32 count, FastSubst &subst)
// {
//   for (uint i = 0; i < count; ++i) {
//     if (!match(tl1[i], tl2[i], subst).size()) {
//       return false;
//     }
//   }
//   return true;
// }

#include <iostream>

vector<FastSubst> match(FastTerm subject, FastTerm pattern) {
  vector<FastVar> vars;
  function<void(FastTerm)> getVars = [&](FastTerm t) {
    if (isVariable(t)) {
      vars.push_back(t);
      return;
    }
    for (uint32 i = 0; i < getArity(getFunc(t)); ++i) {
      getVars(getArg(t, i));
    }
  };
  getVars(subject);
  sort(vars.begin(), vars.end());
  vars.erase(unique(vars.begin(), vars.end()), vars.end());
  vector<FastTerm> consts(vars.size());
  uint save = startTempConsts();
  for (uint i = 0; i < vars.size(); ++i) {
    consts[i] = newFuncTerm(newTempConst(getVarSort(vars[i])), nullptr);
    subject = applyUnitySubst(subject, vars[i], consts[i]);
  }
  // char buffer1[1024];
  // printTerm(subject, buffer1, 1024);
  // printf("match %s\n", buffer1);
  // char buffer2[1024];
  // printTerm(pattern, buffer2, 1024);
  //  printf("     against %s\n", buffer2);
  auto minSubstSet = FastQueryACUnify(subject, pattern).solve();
  for (auto &s : minSubstSet) {
    //    printf("    One solution.\n");
    for (uint i = 0; i < vars.size(); ++i) {
      //      printf("    Replace %d with %d\n", consts[i], vars[i]);
      s.replaceConstWithVar(consts[i], vars[i]);
    }
  }
  endTempConsts(save);
  //  printf("    done match\n");
  return minSubstSet;
}
