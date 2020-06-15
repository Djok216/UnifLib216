#include <algorithm>
#include "unifeqsystem.h"
#include "fastterm.h"

UnifEqSystem::UnifEqSystem() {}

UnifEqSystem::UnifEqSystem(FastTerm t1, FastTerm t2) {
  if (isFuncTerm(t1) && isVariable(t2)) {
    std::swap(t1, t2);
  }
  this->emplace_back(t1, t2);
}

UnifEqSystem::UnifEqSystem(const UnifEq &eq) {
  this->push_back(eq);
}

UnifEqSystem::UnifEqSystem(const UnifEqSystem &ues) : std::vector<UnifEq> (ues) {
  this->sortUES();
}

UnifEqSystem::UnifEqSystem(const FastSubst &sol, const UnifEqSystem &ues) : std::vector<UnifEq> (ues) {
  for (int i = 0; i < (int)sol.count; i += 2) {
    this->emplace_back(sol.data[i], sol.data[i + 1]);
  }
  this->sortUES();
}

void UnifEqSystem::addEq(const UnifEq &eq, bool toSort) {
  this->push_back(eq);
  if (toSort) {
    this->sortUES();
  }
}

void UnifEqSystem::decomp(FastTerm t1, FastTerm t2) {
  int n = getArity(getFunc(t1));
  auto args1 = args(t1);
  auto args2 = args(t2);
  for (int i = 0; i < n; ++i) {
    this->push_back(UnifEq(args1[i], args2[i]));
  }
  this->sortUES();
}

void UnifEqSystem::decompInverse(FastTerm t1, FastTerm t2) {
  int n = getArity(getFunc(t1));
  auto args1 = args(t1);
  auto args2 = args(t2);
  for (int i = 0; i < n; ++i) {
    this->push_back(UnifEq(args1[i], args2[n - i - 1]));
  }
  this->sortUES();
}

void UnifEqSystem::sortUES() {
  std::sort(this->begin(), this->end(), [](const UnifEq &a, const UnifEq &b) {
    return isVariable(a.t1) + isVariable(a.t2) < isVariable(b.t1) + isVariable(b.t2);
  });
}
