#ifndef UNIFEQ_H__
#define UNIFEQ_H__

#include "fastterm.h"

struct UnifEq {
  FastTerm t1;
  FastTerm t2;

  UnifEq(FastTerm t1, FastTerm t2);
};

#endif
