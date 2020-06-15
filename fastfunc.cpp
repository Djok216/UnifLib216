#include "fastterm.h"
#include <cassert>
#include <sstream>
#include <cstring>
#include <iostream>

bool funcIsBuiltin[MAXFUNCS];
BuiltinFuncType builtinFunc[MAXFUNCS];
int builtinFuncExtra[MAXFUNCS]; // if builtinFunc[i] == bltnNumeral, then
                                // builtinFuncExtra[i] in { -256, ..., 256 } is the numeral

FastFunc funcAnd;
FastFunc funcImplies;
FastFunc funcOr;
FastFunc funcNot;
FastFunc funcTrue;
FastFunc funcFalse;

FastFunc funcForallInt;
FastFunc funcExistsInt;

FastFunc funcIteInt;

FastFunc funcLE;
FastFunc funcGE;
FastFunc funcLt;
FastFunc funcGt;
FastFunc funcPlus;
FastFunc funcTimes;
FastFunc funcDiv;
FastFunc funcMod;
FastFunc funcMinus;

FastFunc funcEqInt;
FastFunc funcEqBool;

bool hasEqFunc[MAXSORTS];
FastFunc eqFunc[MAXSORTS];

FastTerm termTrue;
FastTerm termFalse;

bool isAC[MAXFUNCS];
bool isC[MAXFUNCS];
FastFunc uElem[MAXFUNCS];

uint32 funcCount = 0;
uint32 arities[MAXFUNCS]; // number of arguments
uint32 arityIndex[MAXFUNCS]; // where in arityData the argument sorts are
FastSort resultSorts[MAXFUNCS]; // the result sort
FastSort arityData[MAXARITYDATA]; // at arityData[arityIndex[f]] start the
                                  // arities[f] sorts of the arguments
uint32 arityDataIndex = 0;
const char *funcNames[MAXFUNCS];

bool isBuiltinFunc(FastFunc func)
{
  assert(validFastFunc(func));
  return funcIsBuiltin[func];
}

BuiltinFuncType getBuiltinFuncType(FastFunc func)
{
  assert(validFastFunc(func));
  assert(isBuiltinFunc(func));
  return builtinFunc[func];
}

void initFuncs()
{
  funcTrue = newConst("true", fastBoolSort());
  funcIsBuiltin[funcTrue] = true;
  builtinFunc[funcTrue] = bltnTrue;

  funcFalse = newConst("false", fastBoolSort());
  funcIsBuiltin[funcFalse] = true;
  builtinFunc[funcFalse] = bltnFalse;

  FastSort args[16];
  args[0] = fastBoolSort();
  args[1] = fastBoolSort();

  funcAnd = newFunc("band", fastBoolSort(), 2, args);
  funcIsBuiltin[funcAnd] = true;
  builtinFunc[funcAnd] = bltnAnd;

  funcImplies = newFunc("bimplies", fastBoolSort(), 2, args);
  funcIsBuiltin[funcImplies] = true;
  builtinFunc[funcImplies] = bltnImplies;

  funcOr = newFunc("bor", fastBoolSort(), 2, args);
  funcIsBuiltin[funcOr] = true;
  builtinFunc[funcOr] = bltnOr;

  funcNot = newFunc("bnot", fastBoolSort(), 1, args);
  funcIsBuiltin[funcNot] = true;
  builtinFunc[funcNot] = bltnNot;

  funcEqBool = newFunc("beq", fastBoolSort(), 2, args);
  funcIsBuiltin[funcEqBool] = true;
  builtinFunc[funcEqBool] = bltnEqBool;
  hasEqFunc[fastBoolSort()] = true;
  eqFunc[fastBoolSort()] = funcEqBool;

  termTrue = newFuncTerm(funcTrue, args);
  termFalse = newFuncTerm(funcFalse, args);

  for (int i = -256; i <= 256; ++i) {
    std::ostringstream oss;
    oss << i;
    FastFunc funci = newConst(oss.str().c_str(), fastIntSort());
    funcIsBuiltin[funci] = true;
    builtinFunc[funci] = bltnNumeral;
    builtinFuncExtra[funci] = i;
  }

  args[0] = fastIntSort();
  args[1] = fastBoolSort();

  funcForallInt = newFunc("forallInt", fastBoolSort(), 2, args);
  funcIsBuiltin[funcForallInt] = true;
  builtinFunc[funcForallInt] = bltnForall;

  funcExistsInt = newFunc("existsInt", fastBoolSort(), 2, args);
  funcIsBuiltin[funcExistsInt] = true;
  builtinFunc[funcExistsInt] = bltnExists;

  args[0] = fastBoolSort();
  args[1] = fastIntSort();
  args[2] = fastIntSort();

  funcIteInt = newFunc("iteInt", fastIntSort(), 3, args);
  funcIsBuiltin[funcIteInt] = true;
  builtinFunc[funcIteInt] = bltnIte;

  args[0] = fastIntSort();
  args[1] = fastIntSort();

  funcLE = newFunc("mle", fastBoolSort(), 2, args);
  funcIsBuiltin[funcLE] = true;
  builtinFunc[funcLE] = bltnLE;

  funcLt = newFunc("mlt", fastBoolSort(), 2, args);
  funcIsBuiltin[funcLt] = true;
  builtinFunc[funcLt] = bltnLt;

  funcGE = newFunc("mge", fastBoolSort(), 2, args);
  funcIsBuiltin[funcGE] = true;
  builtinFunc[funcGE] = bltnGE;

  funcGt = newFunc("mgt", fastBoolSort(), 2, args);
  funcIsBuiltin[funcGt] = true;
  builtinFunc[funcGt] = bltnGt;

  funcPlus = newFunc("mplus", fastIntSort(), 2, args);
  funcIsBuiltin[funcPlus] = true;
  builtinFunc[funcPlus] = bltnPlus;

  funcTimes = newFunc("mtimes", fastIntSort(), 2, args);
  funcIsBuiltin[funcTimes] = true;
  builtinFunc[funcTimes] = bltnTimes;

  funcDiv = newFunc("mdiv", fastIntSort(), 2, args);
  funcIsBuiltin[funcDiv] = true;
  builtinFunc[funcDiv] = bltnDiv;

  funcMod = newFunc("mmod", fastIntSort(), 2, args);
  funcIsBuiltin[funcMod] = true;
  builtinFunc[funcMod] = bltnMod;

  funcMinus = newFunc("mminus", fastIntSort(), 2, args);
  funcIsBuiltin[funcMinus] = true;
  builtinFunc[funcMinus] = bltnMinus;

  funcEqInt = newFunc("mequals", fastBoolSort(), 2, args);
  funcIsBuiltin[funcEqInt] = true;
  builtinFunc[funcEqInt] = bltnEqInt;

  hasEqFunc[fastIntSort()] = true;
  eqFunc[fastIntSort()] = funcEqInt;
}


FastTerm fastEq(FastTerm t1, FastTerm t2)
{
  FastSort sort1 = getSort(t1);
  FastSort sort2 = getSort(t2);
  if (sort1 != sort2) {
    std::cerr << "Internal error: tried to add equality between terms of different sorts." << std::endl;
    exit(-1);
  }
  if (!hasEqFunc[sort1]) {
    std::cerr << "Internal error: tried to add equality between terms of sort that does not feature equality." << std::endl;
    exit(-1);
  }
  if (eq_term(t1, t2)) {
    return fastTrue();
  }

  FastTerm args[4];
  args[0] = t1;
  args[1] = t2;
  return newFuncTerm(eqFunc[sort1], args);
}

FastTerm fastEqInt(FastTerm t1, FastTerm t2)
{
  if (eq_term(t1, t2)) {
    return fastTrue();
  }
  FastTerm args[4];
  args[0] = t1;
  args[1] = t2;
  return newFuncTerm(funcEqInt, args);
}

FastTerm fastEqBool(FastTerm t1, FastTerm t2)
{
  if (eq_term(t1, t2)) {
    return fastTrue();
  }
  FastTerm args[4];
  args[0] = t1;
  args[1] = t2;
  return newFuncTerm(funcEqBool, args);
}

FastTerm fastExists(FastVar x, FastTerm constraint)
{
  FastSort sort = getVarSort(x);
  //assert(sort == fastIntSort());
  FastTerm args[4];
  args[0] = x;
  args[1] = constraint;
  return newFuncTerm(funcExistsInt, args);
}

FastTerm fastForall(FastVar x, FastTerm constraint)
{
  FastSort sort = getVarSort(x);
  assert(sort == fastIntSort());
  FastTerm args[4];
  args[0] = x;
  args[1] = constraint;
  return newFuncTerm(funcForallInt, args);
}

FastTerm fastAndVector(std::vector<FastTerm> vt)
{
  if (vt.size() == 0) {
    return fastFalse();
  } else if (vt.size() == 1) {
    return vt[0];
  } else {
    FastTerm result = fastAnd(vt[0], vt[1]);
    for (uint i = 2; i < vt.size(); ++i) {
      result = fastAnd(result, vt[i]);
    }
    return result;
  }
}

FastTerm fastOrVector(std::vector<FastTerm> vt)
{
  if (vt.size() == 0) {
    return fastFalse();
  } else if (vt.size() == 1) {
    return vt[0];
  } else {
    FastTerm result = fastOr(vt[0], vt[1]);
    for (uint i = 2; i < vt.size(); ++i) {
      result = fastOr(result, vt[i]);
    }
    return result;
  }
}

FastTerm fastAnd(FastTerm t1, FastTerm t2)
{
  if (t1 == fastFalse()) {
    return fastFalse();
  }
  if (t2 == fastFalse()) {
    return fastFalse();
  }
  if (t1 == fastTrue()) {
    return t2;
  }
  if (t2 == fastTrue()) {
    return t1;
  }
  if (eq_term(t1, t2)) {
    return t1;
  }
  FastTerm args[4];
  args[0] = t1;
  args[1] = t2;
  return newFuncTerm(funcAnd, args);
}

FastTerm fastPlus(FastTerm t1, FastTerm t2)
{
  FastTerm args[4];
  args[0] = t1;
  args[1] = t2;
  return newFuncTerm(funcPlus, args);
}

FastTerm fastZero()
{
  assert(0);
  exit(-1);
  return 0;
}

FastTerm fastOne()
{
  assert(0);
  exit(-1);
  return 0;
}

FastTerm fastPlusVector(std::vector<FastTerm> vt)
{
  if (vt.size() == 0) {
    return fastZero();
  } else if (vt.size() == 1) {
    return vt[0];
  } else {
    FastTerm result = fastPlus(vt[0], vt[1]);
    for (uint i = 2; i < vt.size(); ++i) {
      result = fastPlus(result, vt[i]);
    }
    return result;
  }
}

FastTerm fastTimesVector(std::vector<FastTerm> vt)
{
  if (vt.size() == 0) {
    return fastOne();
  } else if (vt.size() == 1) {
    return vt[0];
  } else {
    FastTerm result = fastTimes(vt[0], vt[1]);
    for (uint i = 2; i < vt.size(); ++i) {
      result = fastTimes(result, vt[i]);
    }
    return result;
  }
}

FastTerm fastTimes(FastTerm t1, FastTerm t2)
{
  FastTerm args[4];
  args[0] = t1;
  args[1] = t2;
  return newFuncTerm(funcTimes, args);
}

FastTerm fastDiv(FastTerm t1, FastTerm t2)
{
  FastTerm args[4];
  args[0] = t1;
  args[1] = t2;
  return newFuncTerm(funcDiv, args);
}

FastTerm fastMod(FastTerm t1, FastTerm t2)
{
  FastTerm args[4];
  args[0] = t1;
  args[1] = t2;
  return newFuncTerm(funcMod, args);
}

FastTerm fastImplies(FastTerm t1, FastTerm t2)
{
  if (t1 == fastFalse()) {
    return fastTrue();
  }
  if (t2 == fastFalse()) {
    return t1;
  }
  if (t1 == fastTrue()) {
    return t2;
  }
  if (t2 == fastTrue()) {
    return fastTrue();
  }
  if (eq_term(t1, t2)) {
    return fastTrue();
  }
  FastTerm args[4];
  args[0] = t1;
  args[1] = t2;
  return newFuncTerm(funcImplies, args);
}

FastTerm fastOr(FastTerm t1, FastTerm t2)
{
  if (t1 == fastTrue()) {
    return fastTrue();
  }
  if (t2 == fastTrue()) {
    return fastTrue();
  }
  if (t1 == fastFalse()) {
    return t2;
  }
  if (t2 == fastFalse()) {
    return t1;
  }
  if (eq_term(t1, t2)) {
    return t1;
  }
  FastTerm args[4];
  args[0] = t1;
  args[1] = t2;
  return newFuncTerm(funcOr, args);
}

FastTerm fastLt(FastTerm t1, FastTerm t2)
{
  FastTerm args[4];
  args[0] = t1;
  args[1] = t2;
  return newFuncTerm(funcLt, args);
}

FastTerm fastGt(FastTerm t1, FastTerm t2)
{
  FastTerm args[4];
  args[0] = t1;
  args[1] = t2;
  return newFuncTerm(funcGt, args);
}

FastTerm fastLE(FastTerm t1, FastTerm t2)
{
  FastTerm args[4];
  args[0] = t1;
  args[1] = t2;
  return newFuncTerm(funcLE, args);
}

FastTerm fastGE(FastTerm t1, FastTerm t2)
{
  FastTerm args[4];
  args[0] = t1;
  args[1] = t2;
  return newFuncTerm(funcGE, args);
}

FastTerm fastNot(FastTerm t1)
{
  FastTerm args[4];
  args[0] = t1;
  return newFuncTerm(funcNot, args);
}

FastTerm fastTrue()
{
  return termTrue;
}

FastTerm fastFalse()
{
  return termFalse;
}

bool validFastFunc(FastFunc func)
{
  assert(/* 0 <= funcCount && */ funcCount <= MAXFUNCS);
  return /* 0 <= func && */ func < funcCount;
}

uint startTempConsts()
{
  return funcCount;
}

FastFunc newTempConst(FastSort sort)
{
  if (funcCount == MAXFUNCS) {
    fprintf(stderr, "Too many functions.\n");
    exit(-1);
  }
  arities[funcCount] = 0;
  funcNames[funcCount] = 0;
  arityIndex[funcCount] = 0; // not to be used (as arity is 0)
  resultSorts[funcCount] = sort;
  uElem[funcCount] = MISSING_UELEM;
  isAC[funcCount] = false;
  isC[funcCount] = false;
  FastFunc result = funcCount++;
  assert(validFastFunc(result));
  return result;
}

void endTempConsts(uint initial)
{
  assert(initial <= funcCount);
  funcCount = initial;
}

FastFunc newConst(const char *name, FastSort sort)
{
  if (funcCount == MAXFUNCS) {
    fprintf(stderr, "Too many functions.\n");
    exit(-1);
  }
  arities[funcCount] = 0;
  funcNames[funcCount] = strdup(name);
  if (funcNames[funcCount] == 0) {
    fprintf(stderr, "Not enough memory to duplicate C string (function name).\n");
    exit(-1);
  }
  arityIndex[funcCount] = 0; // not to be used (as arity is 0)
  resultSorts[funcCount] = sort;
  uElem[funcCount] = MISSING_UELEM;
  isAC[funcCount] = false;
  isC[funcCount] = false;
  FastFunc result = funcCount++;
  assert(validFastFunc(result));
  return result;
}

FastFunc newInterpretedFunc(const char *name, FastSort resultSort, uint32 argCount, FastSort *args)
{
  FastFunc result = newFunc(name, resultSort, argCount, args);
  funcIsBuiltin[result] = true;
  builtinFunc[result] = bltnUD;
  // TODO: treat interpretation
  return result;
}

FastFunc newFunc(const char *name, FastSort resultSort, uint32 argCount, FastSort *args)
{
  if (funcCount == MAXFUNCS) {
    fprintf(stderr, "Too many functions.\n");
    exit(-1);
  }
  if (arityDataIndex + argCount > MAXARITYDATA) {
    fprintf(stderr, "Too many function arguments.\n");
    exit(-1);
  }
  arities[funcCount] = argCount;
  funcNames[funcCount] = strdup(name);
  if (funcNames[funcCount] == 0) {
    fprintf(stderr, "Not enough memory to duplicate C string (function name).\n");
    exit(-1);
  }
  resultSorts[funcCount] = resultSort;
  arityIndex[funcCount] = arityDataIndex;
  for (uint i = 0; i < argCount; ++i) {
    arityData[arityDataIndex++] = args[i];
  }
  uElem[funcCount] = MISSING_UELEM;
  isAC[funcCount] = false;
  isC[funcCount] = false;
  FastFunc result = funcCount++;
  assert(validFastFunc(result));
  return result;
}

FastFunc newCFunc(const char *name, FastSort sort) {
  if (funcCount == MAXFUNCS) {
    fprintf(stderr, "Too many functions.\n");
    exit(-1);
  }
  if (arityDataIndex + 2 > MAXARITYDATA) {
    fprintf(stderr, "Too many function arguments.\n");
    exit(-1);
  }
  arities[funcCount] = 2;
  funcNames[funcCount] = strdup(name);
  if (funcNames[funcCount] == 0) {
    fprintf(stderr, "Not enough memory to duplicate C string (function name).\n");
    exit(-1);
  }
  resultSorts[funcCount] = sort;
  arityIndex[funcCount] = arityDataIndex;
  for (uint i = 0; i < 2; ++i) {
    arityData[arityDataIndex++] = sort;
  }
  uElem[funcCount] = MISSING_UELEM;
  isAC[funcCount] = false;
  isC[funcCount] = true;
  FastFunc result = funcCount++;
  assert(validFastFunc(result));
  return result;
}

FastFunc newACFunc(const char *name, FastSort sort) {
  if (funcCount == MAXFUNCS) {
    fprintf(stderr, "Too many functions.\n");
    exit(-1);
  }
  if (arityDataIndex + 2 > MAXARITYDATA) {
    fprintf(stderr, "Too many function arguments.\n");
    exit(-1);
  }
  arities[funcCount] = 2;
  funcNames[funcCount] = strdup(name);
  if (funcNames[funcCount] == 0) {
    fprintf(stderr, "Not enough memory to duplicate C string (function name).\n");
    exit(-1);
  }
  resultSorts[funcCount] = sort;
  arityIndex[funcCount] = arityDataIndex;
  for (uint i = 0; i < 2; ++i) {
    arityData[arityDataIndex++] = sort;
  }
  uElem[funcCount] = MISSING_UELEM;
  isAC[funcCount] = true;
  isC[funcCount] = false;
  FastFunc result = funcCount++;
  assert(validFastFunc(result));
  return result;
}

FastFunc newACUFunc(const char *name, FastFunc uElem) {
  if (funcCount == MAXFUNCS) {
    fprintf(stderr, "Too many functions.\n");
    exit(-1);
  }
  if (arityDataIndex + 2 > MAXARITYDATA) {
    fprintf(stderr, "Too many function arguments.\n");
    exit(-1);
  }
  arities[funcCount] = 2;
  funcNames[funcCount] = strdup(name);
  if (funcNames[funcCount] == 0) {
    fprintf(stderr, "Not enough memory to duplicate C string (function name).\n");
    exit(-1);
  }
  resultSorts[funcCount] = getFuncSort(uElem);
  arityIndex[funcCount] = arityDataIndex;
  for (uint i = 0; i < 2; ++i) {
    arityData[arityDataIndex++] = getFuncSort(uElem);
  }
  ::uElem[funcCount] = uElem;
  isAC[funcCount] = true;
  isC[funcCount] = false;
  FastFunc result = funcCount++;
  assert(validFastFunc(result));
  return result;
}

FastFunc getUnityElement(FastFunc func) {
  assert(validFastFunc(func));
  return uElem[func]; 
}

bool isFuncC(FastFunc func) {
  assert(validFastFunc(func));
  return isC[func];
}

bool isFuncAC(FastFunc func) {
  assert(validFastFunc(func));
  return isAC[func];
}

FastSort getFuncSort(FastFunc func)
{
  assert(validFastFunc(func));
  return resultSorts[func];
}

FastSort getArgSort(FastFunc func, uint arg)
{
  assert(validFastFunc(func));
  return arityData[arityIndex[func] + arg];
}

uint getArity(FastFunc func)
{
  if (func == MISSING_UELEM) {
    return 0;
  }
  assert(validFastFunc(func));
  return arities[func];
}

const char *getFuncName(FastTerm func)
{
  assert(validFastFunc(func));
  return funcNames[func];
}

bool eq_func(FastFunc func1, FastFunc func2)
{
  if (func1 == MISSING_UELEM) return func1 == func2;
  if (func2 == MISSING_UELEM) return func1 == func2;
  assert(validFastFunc(func1));
  assert(validFastFunc(func2));
  return func1 == func2;
}

bool existsFunc(const char *name)
{
  for (uint32 i = 0; i < funcCount; ++i) {
    if (strcmp(funcNames[i], name) == 0) {
      return true;
    }
  }
  return false;
}

FastFunc getFuncByName(const char *name)
{
  assert(existsFunc(name));
  for (uint32 i = 0; i < funcCount; ++i) {
    if (strcmp(funcNames[i], name) == 0) {
      assert(validFastFunc(i));
      return i;
    }
  }
  assert(0);
  return 0;
}
