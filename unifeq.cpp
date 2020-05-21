#include <algorithm>
#include "unifeq.h"
#include "fastterm.h"

UnifEq::UnifEq(FastTerm t1, FastTerm t2) {
  if (isFuncTerm(t1) && isVariable(t2)) {
    std::swap(t1, t2);
  }
  if (isVariable(t1) && isVariable(t2)) {
    FastSort s1 = getSort(t1);
    FastSort s2 = getSort(t2);
    if (!isSubSortTransitive(s2, s1)) {
      std::swap(t1, t2);
    }
  }
  this->t1 = t1;
  this->t2 = t2;
}
