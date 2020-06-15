#include "fastterm.h"
#include <stdio.h>
#include <cstring>
#include <assert.h>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <set>
#include <algorithm>
#include <functional>
#include <iostream>
#include <map>

using namespace std;

uint32 termDataSize = 0;
uint32 termData[MAXDATA];
vector<FastTerm> *varsAndFreshCache[MAXDATA];

void initFastTerm()
{
  initSorts();
  initFuncs();
}

bool validFastVarTerm(FastTerm term)
{
  assert(validFastTerm(term));
  return isVariable(term);
}

bool validFastFuncTerm(FastTerm term)
{
  assert(validFastTerm(term));
  return isFuncTerm(term);
}

bool validFastTerm(FastTerm term)
{
  assert(/* 0 <= termDataSize && */termDataSize < MAXDATA);
  return (validFastVar(term)) ||
    (MAXVARS <= term && term < MAXVARS + termDataSize);
}

#ifndef ALLOW_DUPLICATE_TERMS
string toStringLong(FastFunc funct, FastTerm *args) {
  ostringstream oss;
  oss << "(" << getFuncName(funct);
  for (uint i = 0, len = getArity(funct); i < len; ++i) {
    oss << " " << toString(args[i]);
  }
  oss << ")";
  return oss.str();
}

typedef pair< FastFunc, vector<FastTerm> > funTermKey;
struct allFunTermsComparer {
  const bool operator()(const funTermKey &x, const funTermKey &y) const {
    if (x.first != y.first) return x.first < y.first;
    int ar0 = getArity(x.first), ar1 = getArity(y.first);
    if (ar0 != ar1) return ar0 < ar1;
    for (int j = 0; j < ar0; ++j)
      if (x.second[j] != y.second[j]) return x.second[j] < y.second[j];
    return false;
  }
};
map< funTermKey, FastTerm, allFunTermsComparer > allFunTerms;

vector<FastTerm> ptrToVector(FastFunc func, FastTerm *args) {
  vector<FastTerm> v;
  for (uint i = 0, len = getArity(func); i < len; ++i)
    v.push_back(args[i]);
  return v;
}
#endif

FastTerm newFuncTerm(FastFunc func, FastTerm *args)
{
#ifndef ALLOW_DUPLICATE_TERMS
  funTermKey ftKey = make_pair(func, ptrToVector(func, args));
  /*cout << toStringLong(func, args) << endl;
  if (allFunTerms.count(ftKey) > 1) {
    allFunTermsComparer cmp;
    cout << "## Problem with " << toStringLong(func, args) << endl;
    int smaller = 0, bigger = 0, eq = 0;
    for (const auto &it : allFunTerms) {
      bool r0 = cmp(it.first, ftKey), r1 = cmp(ftKey, it.first);
      if (r0) {
        assert(!r1);
        ++smaller;
      }
      else if (r1) {
        assert(!r0);
        ++bigger;
      }
      else {
        cout << "## Equiv with " << toStringLong(it.first.first, it.first.second) << " : " << toString(it.second) << endl;
        ++eq;
      }
    }
    cout << "## Stats :  sz " << allFunTerms.size() << "  sm " << smaller << "  bg " << bigger << "  eq " << eq << endl;
    auto rng = allFunTerms.equal_range(ftKey);
    for (auto it = rng.first; it != rng.second; ++it) {
      cout << "%% Equiv with " << toStringLong(it->first.first, it->first.second) << " : " << toString(it->second) << endl;
    }
  }*/
  if (allFunTerms.count(ftKey)) {
    //cout << "$$ Equivalence detected." << endl;
    //cout << toStringLong(func, args) << endl;
    //cout << toString(allFunTerms.at(ftKey)) << endl;
    return allFunTerms.at(ftKey);
  }
  uint nexti = 0;
  for (uint32 i = MAXVARS; i < MAXVARS + termDataSize; i = nexti) {
    if (isVariable(i)) {
      assert(0);
    }
    assert(isFuncTerm(i));
    FastFunc ifunc = getFunc(i);
    nexti = i + 1 + getArity(ifunc);
    if (func == ifunc) {
      bool ok = true;
      uint len = getArity(func);
      for (uint j = 0; j < len; ++j) {
        if (args[j] != getArg(i, j)) {
          ok = false;
          break;
        }
      }
      if (ok) {
        return i;
      }
    }
  }
#endif

  if (termDataSize + getArity(func) > MAXDATA) {
    fprintf(stderr, "Too much term data.\n");
    exit(-1);
  }
  uint32 result = termDataSize;
  termData[result] = func;
  termDataSize++;
  for (uint i = 0; i < getArity(func); ++i) {
    termData[result + i + 1] = args[i];
    termDataSize++;
  }
  assert(validFastTerm(result + MAXVARS));
#ifdef ALLOW_DUPLICATE_TERMS
  return result + MAXVARS;
#else
  //return allFunTerms[make_pair(func, termData + result + 1)] = (result + MAXVARS);
  return allFunTerms[ftKey] = (result + MAXVARS);
#endif
}

bool isVariable(FastTerm term)
{
  assert(validFastTerm(term));
  return validFastVar(term);
}

bool isFuncTerm(FastTerm term)
{
  assert(validFastTerm(term));
  return MAXVARS <= term && term < MAXVARS + termDataSize;
}

size_t printTermRec(FastTerm term, char *buffer, size_t size)
{
  if (isVariable(term)) {
    assert(/* 0 <= term && */term < MAXVARS);
    size_t len = strlen(getVarName(term));
    if (len <= size) {
      for (size_t i = 0; i < len; ++i) {
        buffer[i] = getVarName(term)[i];
      }
    }
    return len;
  }
  else {
    assert(isFuncTerm(term));
    assert(term >= MAXVARS);
    size_t index = term - MAXVARS;
    assert(/* 0 <= index && */ index < termDataSize);

    FastFunc func = termData[index];
    size_t len = strlen(getFuncName(func));
    size_t printedResult = 0;
    if (len <= size) {
      for (size_t i = 0; i < len; ++i) {
        buffer[i] = getFuncName(func)[i];
        printedResult++;
      }

      buffer = buffer + len;
      size = size - len;
      if (size >= 1 && getArity(func) > 0) {
        buffer[0] = '(';
        size--;
        printedResult++;
        buffer++;
      }
      for (uint i = 0; i < getArity(func); ++i) {
        if (size >= 1 && i > 0) {
          buffer[0] = ',';
          size--;
          printedResult++;
          buffer++;
        }
        size_t printed = printTermRec(termData[index + i + 1], buffer, size);
        buffer = buffer + printed;
        size = size - printed;
        printedResult += printed;
      }
      if (size >= 1 && getArity(func) > 0) {
        buffer[0] = ')';
        size--;
        printedResult++;
        buffer++;
      }
    }
    return printedResult;
  }
}

size_t printTerm(FastTerm term, char *buffer, size_t size)
{
  assert(size > 0);
  size_t printed = printTermRec(term, buffer, size);
  if (printed < size) {
    buffer[printed] = 0;
    return printed;
  }
  else {
    buffer[size - 1] = 0;
    return size - 1;
  }
}

FastFunc getFunc(FastTerm term)
{
  assert(isFuncTerm(term));
  assert(term >= MAXVARS);
  uint32 index = term - MAXVARS;
  return termData[index];
}

FastTerm getArg(FastTerm term, uint arg)
{
  assert(validFastTerm(term));
  assert(validFastFuncTerm(term));
  assert(/* 0 <= arg && */arg < getArity(getFunc(term)));
  return termData[term - MAXVARS + arg + 1];
}

FastTerm *args(FastTerm term)
{
  assert(isFuncTerm(term));
  assert(term >= MAXVARS);
  uint32 index = term - MAXVARS;
  return &termData[index + 1];
}

bool occurs(FastVar var, FastTerm term)
{
  if (term == var) {
    return true;
  }
  if (isFuncTerm(term)) {
    FastFunc func = getFunc(term);
    for (uint i = 0; i < getArity(func); ++i) {
      if (occurs(var, getArg(term, i))) {
        return true;
      }
    }
    return false;
  }
  return false;
}

bool identifierTaken(const char *name)
{
  return existsSort(name) || existsVar(name) || existsFunc(name);
}

FastSort getSort(FastTerm term)
{
  assert(validFastTerm(term));
  if (isVariable(term)) {
    return getVarSort(term);
  }
  else {
    assert(validFastFuncTerm(term));
    return getFuncSort(getFunc(term));
  }
}

bool eq_term_list(FastTerm *tl1, FastTerm *tl2, uint count)
{
  for (uint i = 0; i < count; ++i) {
    if (!eq_term(tl1[i], tl2[i])) {
      return false;
    }
  }
  return true;
}

char buffer[1024];

bool eq_term(FastTerm t1, FastTerm t2)
{
  assert(validFastTerm(t1));
  assert(validFastTerm(t2));
  if (isFuncTerm(t1) && isFuncTerm(t2)) {
    FastFunc func1 = getFunc(t1);
    FastFunc func2 = getFunc(t2);
    if (!eq_func(func1, func2)) {
      return false;
    }
    assert(getArity(func1) == getArity(func2));
    if (getArity(func1) == 0) return true;
    if (!isFuncAC(func1)) return eq_term_list(args(t1), args(t2), getArity(func1));
    // is AC Term
    vector<FastTerm> args1, args2;
    function<void(FastTerm, FastFunc, vector<FastTerm>&)> getArgs;
    getArgs = [&getArgs](FastTerm t, FastFunc f, vector<FastTerm>& argsStar) {
      for (int i = 0; i < getArity(getFunc(t)); ++i) {
        FastTerm arg = getArg(t, i);
        if (isVariable(arg) || !eq_func(f, getFunc(arg))) {
          argsStar.push_back(arg);
          continue;
        }
        getArgs(arg, f, argsStar);
      }
    };
    getArgs(t1, func1, args1);
    getArgs(t2, func2, args2);
    if (args1.size() != args2.size()) return false;
    auto argsComp = [](FastTerm t1, FastTerm t2) {
      if (isFuncTerm(t1) == isFuncTerm(t2)) return toString(t1) < toString(t2);
      return isFuncTerm(t1);
    };
    sort(args1.begin(), args1.end(), argsComp);
    sort(args2.begin(), args2.end(), argsComp);
    return eq_term_list(args1.data(), args2.data(), args1.size());
  }
  if (isVariable(t1) && isVariable(t2)) {
    return eq_var(t1, t2);
  }
  return false;
}

bool containsUnboundVariable(FastTerm term, FastVar var) {
  if (isVariable(term))
    return term == var;
  FastFunc func = getFunc(term);
  if (isBuiltinFunc(func) &&
    ((getBuiltinFuncType(func) == bltnExists) || (getBuiltinFuncType(func) == bltnForall)))
    return false;
  for (int i = 0, arity = getArity(func); i < arity; ++i) {
    if (containsUnboundVariable(getArg(term, i), var))
      return true;
  }
  return false;
}

//returns a term which var can be substituted with to eliminate "exists",
//or 0, if no such term exists
FastTerm simplifyExists(FastTerm term, FastVar var) {
  if (isVariable(term))
    return 0;
  FastFunc func = getFunc(term);
  if (!isBuiltinFunc(func))
    return 0;
  BuiltinFuncType tip = getBuiltinFuncType(func);
  if (tip == bltnEqArray || tip == bltnEqInt || tip == bltnEqBool) {
    assert(getArity(func) == 2);
    FastTerm arg0 = getArg(term, 0), arg1 = getArg(term, 1);
    if (arg1 == var)
      swap(arg0, arg1);
    if (arg0 != var)
      return 0;
    if (containsUnboundVariable(arg1, var))
      return 0;
    return arg1;
  }
  if (tip != bltnAnd)
    return 0;
  for (int i = 0, arity = getArity(func); i < arity; ++i) {
    FastTerm res = simplifyExists(getArg(term, i), var);
    if (res != 0) return res;
  }
  return 0;
}

FastTerm simplifyFast(FastTerm term)
{
  // printTerm(term, buffer, 1024);
  // printf("simplifyFast %s (%d)\n", buffer, term);
  if (isVariable(term)) {
    return term;
  }
  else {
    assert(isFuncTerm(term));
    FastFunc func = getFunc(term);
    std::vector<FastTerm> arguments;
    for (uint i = 0; i < getArity(func); ++i) {
      arguments.push_back(simplifyFast(getArg(term, i)));
      // printTerm(getArg(term, i), buffer, 1024);
      // printf("1. inner simplifyFast %s (%d)\n", buffer, getArg(term, i));
      // printTerm(arguments[i], buffer, 1024);
      // printf("2. inner simplifyFast %s (%d)\n", buffer, arguments[i]);
    }
    FastTerm result = newFuncTerm(func, arguments.data());
    FastTerm t1 = fastTrue(), t2 = fastTrue();
    if (isBuiltinFunc(func)) {
      //      printf("is builtin\n");
      if (getArity(func) >= 1) {
        t1 = arguments[0];
      }
      if (getArity(func) >= 2) {
        t2 = arguments[1];
      }
      switch (getBuiltinFuncType(func)) {
      case bltnAnd:
        if (eq_term(t1, fastFalse())) {
          return fastFalse();
        }
        if (eq_term(t2, fastFalse())) {
          return fastFalse();
        }
        if (eq_term(t1, fastTrue())) {
          return t2;
        }
        if (eq_term(t2, fastTrue())) {
          return t1;
        }
        if (eq_term(t1, t2)) {
          return t1;
        }
        break;
      case bltnOr:
        if (eq_term(t1, fastTrue())) {
          return fastTrue();
        }
        if (eq_term(t2, fastTrue())) {
          return fastTrue();
        }
        if (eq_term(t1, fastFalse())) {
          return t2;
        }
        if (eq_term(t2, fastFalse())) {
          return t1;
        }
        if (eq_term(t1, t2)) {
          return t1;
        }
        break;
      case bltnNot:
        if (eq_term(t1, fastFalse())) {
          return fastTrue();
        }
        if (eq_term(t1, fastTrue())) {
          return fastFalse();
        }
        break;
      case bltnImplies:
        if (eq_term(t1, fastFalse())) {
          return fastTrue();
        }
        if (eq_term(t2, fastFalse())) {
          return t1;
        }
        if (eq_term(t1, fastTrue())) {
          return t2;
        }
        if (eq_term(t2, fastTrue())) {
          return fastTrue();
        }
        if (eq_term(t1, t2)) {
          return fastTrue();
        }
        break;
      case bltnEqInt:
        if (eq_term(t1, t2)) {
          return fastTrue();
        }
        break;
      case bltnEqArray:
        if (eq_term(t1, t2)) {
          return fastTrue();
        }
        break;
      case bltnSelect:
        if (isFuncTerm(arguments[0]) &&
          isBuiltinFunc(getFunc(arguments[0])) &&
          getBuiltinFuncType(getFunc(arguments[0])) == bltnStore) {
          FastTerm *argsstore = args(arguments[0]);
          assert(getArity(getFunc(arguments[0])) == 3);
          if (eq_term(arguments[1], argsstore[1])) {
            return argsstore[2];
          }
        }
        return result;
        break;
      case bltnStore:
        if (isFuncTerm(arguments[0]) &&
          isBuiltinFunc(getFunc(arguments[0])) &&
          getBuiltinFuncType(getFunc(arguments[0])) == bltnStore) {
          FastTerm *argsstore = args(arguments[0]);
          assert(getArity(getFunc(arguments[0])) == 3);
          assert(validFastTerm(arguments[1]));
          assert(validFastTerm(argsstore[1]));
          if (eq_term(arguments[1], argsstore[1])) {
            FastTerm newargs[4];
            newargs[0] = argsstore[0];
            newargs[1] = argsstore[1];
            newargs[2] = arguments[2];
            return newFuncTerm(func, newargs);
          }
        }
        return result;
        break;
      case bltnEqBool:
        if (eq_term(t1, t2)) {
          return fastTrue();
        }
        if (eq_term(t1, fastTrue())) {
          return t2;
        }
        if (eq_term(t2, fastTrue())) {
          return t1;
        }
        break;
      case bltnExists:
      {
        assert(arguments.size() == 2);
        assert(isVariable(arguments[0]));
        FastTerm toSubst = simplifyExists(arguments[1], arguments[0]);
        if (toSubst != 0) {
          FastSubst subst;
          subst.addToSubst(arguments[0], toSubst);
          return subst.applySubst(arguments[1]);
        }
        return result;
        break;
      }
      default:
        break;
      }
    }
    return result;
  }
}

FastTerm replaceConstWithVar(FastTerm term, FastTerm c, FastVar v)
{
  if (isVariable(term)) {
    return term;
  }
  else if (eq_term(term, c)) {
    return v;
  }
  else {
    bool changed = false;
    FastFunc func = getFunc(term);
    uint32 count = getArity(func);
    std::vector<FastTerm> newargs;
    for (uint i = 0; i < count; ++i) {
      FastTerm result = replaceConstWithVar(getArg(term, i), c, v);
      if (result != getArg(term, i)) {
        changed = true;
      }
      newargs.push_back(result);
    }
    if (changed) {
      return newFuncTerm(func, &newargs[0]);
    }
    else {
      return term;
    }
  }
}

std::string toString(FastTerm term)
{
  ostringstream oss;
  printToOss(term, oss);
  return oss.str();
}

std::string toString(FastSubst subst)
{
  ostringstream oss;
  oss << "{ ";
  for (uint i = 0; i < subst.count; i += 2) {
    if (i > 0) {
      oss << ", ";
    }
    assert(validFastTerm(subst.data[i + 1]));
    assert(isVariable(subst.data[i]));
    printToOss(subst.data[i], oss);
    oss << " |-> ";
    assert(validFastTerm(subst.data[i + 1]));
    printToOss(subst.data[i + 1], oss);
  }
  oss << " }";
  return oss.str();
}

void printToOss(FastTerm term, ostringstream &oss)
{
  if (isVariable(term)) {
    assert(/* 0 <= term && */term < MAXVARS);
    oss << getVarName(term);// << "(" << getSortName(getSort(term)) << ")";
  }
  else {
    assert(isFuncTerm(term));
    assert(term >= MAXVARS);
    size_t index = term - MAXVARS;
    assert(/* 0 <= index && */ index < termDataSize);

    FastFunc func = termData[index];
    if (getArity(func) > 0) {
      oss << "(";
    }
    oss << getFuncName(func);// << "(" << getSortName(getSort(term)) << ")";
    for (uint i = 0; i < getArity(func); ++i) {
      oss << " ";
      printToOss(termData[index + i + 1], oss);
    }
    if (getArity(func) > 0) {
      oss << ")";
    }
  }
}

#include <iostream>

void abortWithMessage(const std::string &error)
{
  std::cout << "Error: " << error << endl;
  exit(0);
}

void varsOf(FastTerm term, vector<FastVar> &vars)
{
  if (isVariable(term)) {
    vars.push_back(term);
  }
  else {
    assert(isFuncTerm(term));
    FastFunc func = getFunc(term);
    for (uint i = 0; i < getArity(func); ++i) {
      FastTerm nt = getArg(term, i);
      varsOf(nt, vars);
    }
  }
}

vector<FastVar> uniqueVars(FastTerm term)
{
  vector<FastVar> result;
  varsOf(term, result);
  std::sort(result.begin(), result.end());
  auto it = std::unique(result.begin(), result.end());
  result.resize(std::distance(result.begin(), it));
  return result;
}

FastTerm introduceExists(FastTerm constraint, vector<FastVar> vars)
{
  for (uint i = 0; i < vars.size(); ++i) {
    if (occurs(vars[i], constraint)) {
      for (uint j = 0; j < vars.size(); ++j) {
      }
      constraint = fastExists(vars[i], constraint);
    }
  }
  return constraint;
}

vector<FastTerm> *varsAndFresh(FastTerm term) {
  if (varsAndFreshCache[term] != NULL)
    return varsAndFreshCache[term];
  varsAndFreshCache[term] = new vector<FastTerm>;
  if (isVariable(term)) {
    varsAndFreshCache[term]->push_back(term);
    return varsAndFreshCache[term];
  }
  assert(isFuncTerm(term));
  set<FastTerm> seenVarsAndFresh;
  FastFunc func = getFunc(term);
  if (getBuiltinFuncType(func) == bltnFreshConstant) {
    if (seenVarsAndFresh.count(term))
      return varsAndFreshCache[term];
    varsAndFreshCache[term]->push_back(term);
    seenVarsAndFresh.insert(term);
    return varsAndFreshCache[term];
  }
  int len = getArity(func);
  for (int i = 0; i < len; ++i) {
    FastTerm arg = getArg(term, i);
    for (const auto &it : *varsAndFresh(arg)) {
      if (seenVarsAndFresh.count(it)) continue;
      varsAndFreshCache[term]->push_back(it);
      seenVarsAndFresh.insert(it);
    }
  }
  return varsAndFreshCache[term];
}

FastTerm toUniformTerm_full(FastTerm term, vector<FastTerm> *usedVars) {
  if (isVariable(term)) {
    return makeUniformVariable(getVarSort(term), distance(usedVars->begin(),
      find(usedVars->begin(), usedVars->end(), term)));
  }
  set<FastTerm> seenVarsAndFresh;
  FastFunc func = getFunc(term);
  int len = getArity(func);

  if (getBuiltinFuncType(func) == bltnFreshConstant) {
    return makeUniformVariable(getSort(term), distance(usedVars->begin(),
      find(usedVars->begin(), usedVars->end(), term)));
  }

  FastTerm *newArguments = (FastTerm*)(malloc(len * (sizeof(FastTerm))));
  for (int i = 0; i < len; ++i) {
    newArguments[i] = toUniformTerm_full(getArg(term, i), usedVars);
  }
  return newFuncTerm(func, newArguments);
}

FastTerm toUniformTerm(FastTerm term) {
  return toUniformTerm_full(term, varsAndFresh(term));
}
