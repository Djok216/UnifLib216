#include "fastterm.h"
#include <cassert>
#include <cstring>

FastSubst::FastSubst()
{
  size = 4;
  data = (uint *)malloc(sizeof(uint) * size);
  count = 0;
}

FastSubst::FastSubst(const FastSubst &s)
{
  size = s.size;
  count = s.count;
  data = (uint *)malloc(sizeof(uint) * size);
  memmove(data, s.data, count * sizeof(uint32));
}

FastSubst &FastSubst::operator=(const FastSubst &s)
{
  size = s.size;
  count = s.count;
  data = (uint *)malloc(sizeof(uint) * size);
  memmove(data, s.data, count * sizeof(uint32));
  return *this;
}

FastSubst::~FastSubst()
{
  free(data);
}

void FastSubst::addToSubst(FastVar var, FastTerm term)
{
  if (count >= size) {
    uint *newdata = (uint *)realloc(data, sizeof(uint) * size * 2);
    size = size * 2;
    data = newdata;
  }
  data[count++] = var;
  data[count++] = term;
}

bool FastSubst::inDomain(FastVar var)
{
  for (uint i = 0; i < count; i += 2) {
    if (data[i] == var) {
      return true;
    }
  }
  return false;
}

FastTerm FastSubst::image(FastVar var)
{
  assert(this->inDomain(var));
  for (uint i = 0; i < count; i += 2) {
    if (data[i] == var) {
      return data[i + 1];
    }
  }
  assert(0);
  return 0;
}

void FastSubst::replaceConstWithVar(FastTerm c, FastVar v)
{
  for (uint i = 1; i < count; i += 2) {
    data[i] = ::replaceConstWithVar(data[i], c, v);
  }
}

FastSubst compose(FastSubst sigma1, FastSubst sigma2)
{
  // returns a substitution sigma s.t.
  // sigma(x) == sigma2(sigma1(x)) forall x
  FastSubst result;
  for (uint32 i = 0; i < sigma1.count; i += 2) {
    FastVar var = sigma1.data[i];
    FastTerm term = sigma1.data[i + 1];
    assert(sigma1.inDomain(var));
    assert(sigma1.image(var) == term);
    result.addToSubst(var, sigma2.applySubst(term));
  }
  for (uint32 i = 0; i < sigma2.count; i += 2) {
    FastVar var = sigma2.data[i];
    FastTerm term = sigma2.data[i + 1];
    assert(sigma2.inDomain(var));
    assert(sigma2.image(var) == term);
    if (!sigma1.inDomain(var)) {
      result.addToSubst(var, term);
    }
  }
  return result;
}

FastTerm FastSubst::applySubst(FastTerm term)
{
  if (isVariable(term)) {
    if (inDomain(term)) {
      return image(term);
    }
    return term;
  } else {
    assert(isFuncTerm(term));
    FastFunc func = getFunc(term);
    //    if (getArity(func) != 0) {
    FastTerm result = newFuncTerm(func, args(term));
    FastTerm *arguments = args(result);
    for (uint i = 0; i < getArity(func); ++i) {
      *arguments = this->applySubst(*arguments);
      arguments++;
    }
    return result;
    // } else {
    //   return term;
    // }
  }
}

void FastSubst::composeWith(FastVar v, FastTerm t)
{
  bool found = false;
  for (uint i = 0; i < count; i += 2) {
    FastVar x = data[i];
    FastTerm s = data[i + 1];
    if (x == v) {
      found = true;
    }
    data[i + 1] = applyUnitySubst(s, v, t);
  }
  if (!found) {
    addToSubst(v, t);
  }
}

void FastSubst::applyInRange(FastVar v, FastTerm t)
{
  for (uint i = 0; i < count; i += 2) {
    data[i + 1] = applyUnitySubst(data[i + 1], v, t);
  }
}

FastTerm applyUnitySubst(FastTerm term, FastVar v, FastTerm t)
{
  if (isVariable(term)) {
    if (term == v) {
      return t;
    }
    return term;
  } else {
    assert(isFuncTerm(term));
    FastFunc func = getFunc(term);
    FastTerm result = newFuncTerm(func, args(term));
    FastTerm *arguments = args(result);
    for (uint i = 0; i < getArity(func); ++i) {
      *arguments = applyUnitySubst(*arguments, v, t);
      arguments++;
    }
    return result;
  }
}

size_t printSubst(FastSubst &subst, char *buffer, size_t size)
{
  size_t result = 0;
  if (size >= 1) {
    buffer[0] = '{';
    size--;
    result++;
    buffer++;
  }
  if (size >= 1) {
    buffer[0] = ' ';
    size--;
    result++;
    buffer++;
  }
  for (uint i = 0; i < subst.count; i += 2) {
    if (i > 0) {
      if (size >= 1) {
        buffer[0] = ',';
        size--;
        result++;
        buffer++;
      }
      if (size >= 1) {
        buffer[0] = ' ';
        size--;
        result++;
        buffer++;
      }
    }
    assert(validFastTerm(subst.data[i + 1]));
    assert(isVariable(subst.data[i]));
    uint32 printed = printTerm(subst.data[i], buffer, size);
    buffer += printed;
    size -= printed;
    result += printed;
    if (size >= 1) {
      buffer[0] = '-';
      size--;
      result++;
      buffer++;
    }
    if (size >= 1) {
      buffer[0] = '>';
      size--;
      result++;
      buffer++;
    }
    assert(validFastTerm(subst.data[i + 1]));
    printed = printTerm(subst.data[i + 1], buffer, size);
    buffer += printed;
    size -= printed;
    result += printed;
  }
  if (size >= 1) {
    buffer[0] = ' ';
    size--;
    result++;
    buffer++;
  }
  if (size >= 1) {
    buffer[0] = '}';
    size--;
    result++;
    buffer++;
  }
  return result;
}

