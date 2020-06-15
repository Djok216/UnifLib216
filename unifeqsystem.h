#ifndef UNIFEQSYSTEM_H__
#define UNIFEQSYSTEM_H__

#include <vector>
#include "unifeq.h"
#include "fastterm.h"


struct UnifEqSystem : std::vector<UnifEq> {
  UnifEqSystem();
  UnifEqSystem(FastTerm t1, FastTerm t2);
  UnifEqSystem(const UnifEq &eq);
  UnifEqSystem(const UnifEqSystem &ues);
  UnifEqSystem(const FastSubst &sols, const UnifEqSystem &ues);
  void addEq(const UnifEq &eq, bool toSort = false);
  void decomp(FastTerm t1, FastTerm t2);
  void decompInverse(FastTerm t1, FastTerm t2);
  void sortUES();
};

#endif
