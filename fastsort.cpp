#include "fastterm.h"
#include "log.h"
#include <cassert>
#include <cstring>
#include <iostream>

using namespace std;

uint32 sortCount = 0;
const char *sortNames[MAXSORTS];

bool sortIsBuiltin[MAXSORTS];
BuiltinSortType builtinSortType[MAXSORTS];
FastSort sortArguments[MAXSORTS][4]; // currently arguments for array

FastSort sortBool;
FastSort sortInt;
FastSort sortState;

FastSort subsortings[MAXSUBSORTINGS * 2];
uint32 subsortingCount = 0;

void initSorts()
{
  for (uint i = 0; i < MAXSORTS; ++i) {
    sortIsBuiltin[i] = false;
  }

  sortBool = newSort("Bool");
  sortIsBuiltin[sortBool] = true;
  builtinSortType[sortBool] = bltnBool;

  sortInt = newSort("Int");
  sortIsBuiltin[sortInt] = true;
  builtinSortType[sortInt] = bltnInt;
}

bool isBuiltinSort(FastSort sort)
{
  assert(validFastSort(sort));
  return sortIsBuiltin[sort];
}

FastSort fastBoolSort()
{
  return sortBool;
}

FastSort fastIntSort()
{
  return sortInt;
}

#include <map>

std::map<std::pair<FastSort, std::pair<FastSort, FastSort>>, FastFunc> selectFunc;
std::map<std::pair<FastSort, FastSort>, FastFunc> constArrayFunc;
std::map<std::pair<FastSort, std::pair<FastSort, FastSort>>, FastFunc> storeFunc;

FastSort newArraySort(const char *name, FastSort domainSort, FastSort rangeSort)
{
  extern bool hasEqFunc[MAXSORTS];
  extern FastFunc eqFunc[MAXSORTS];
  assert(isBuiltinSort(domainSort));
  assert(isBuiltinSort(rangeSort));
  FastSort result = newSort(name);
  sortIsBuiltin[result] = true;
  builtinSortType[result] = bltnArray;
  sortArguments[result][0] = domainSort;
  sortArguments[result][1] = rangeSort;

  FastSort args[16];
  args[0] = result;
  args[1] = result;
  FastFunc eqf = newFunc((std::string("equals") + name).c_str(), fastBoolSort(), 2, args);
  extern bool funcIsBuiltin[MAXFUNCS];
  extern BuiltinFuncType builtinFunc[MAXFUNCS];
  funcIsBuiltin[eqf] = true;
  builtinFunc[eqf] = bltnEqArray;

  hasEqFunc[result] = true;
  eqFunc[result] = eqf;

  return result;
}

FastSort fastStateSort() {
  return sortState;
}

bool validFastSort(FastSort sort)
{
  assert(/* 0 <= sortCount && */sortCount < MAXSORTS);
  return /* 0 <= sort && */ sort < sortCount;
}

FastSort newSort(const char *name)
{
  if (sortCount == MAXSORTS) {
    fprintf(stderr, "Too many sorts.\n");
    exit(-1);
  }
  sortNames[sortCount] = strdup(name);
  if (sortNames[sortCount] == 0) {
    fprintf(stderr, "Not enough memory to duplicate C string.\n");
    exit(-1);
  }
  FastSort result = sortCount++;
  assert(validFastSort(result));
  return result;
}

void newSubSort(FastSort subsort, FastSort supersort)
{
  if (subsortingCount == MAXSUBSORTINGS) {
    fprintf(stderr, "Too many subsortings.\n");
    exit(-1);
  }
  subsortings[subsortingCount * 2] = subsort;
  subsortings[subsortingCount * 2 + 1] = supersort;
  subsortingCount++;
}

const char *getSortName(FastSort sort)
{
  assert(validFastSort(sort));
  return sortNames[sort];
}

BuiltinSortType getBuiltinSortType(FastSort sort)
{
  assert(validFastSort(sort));
  return builtinSortType[sort];
}

bool existsSort(const char *name)
{
  for (uint32 i = 0; i < sortCount; ++i) {
    if (strcmp(sortNames[i], name) == 0) {
      return true;
    }
  }
  return false;
}

FastSort getSortByName(const char *name)
{
  LOG(DEBUG15, cerr << name);
  assert(existsSort(name));
  for (uint32 i = 0; i < sortCount; ++i) {
    if (strcmp(sortNames[i], name) == 0) {
      assert(validFastSort(i));
      return i;
    }
  }
  assert(0);
  return 0;
}

bool isSubSortTransitive(FastSort subsort, FastSort supersort)
{
  if (subsort == supersort) {
    return true;
  }
  for (uint32 i = 0; i < subsortingCount; i++) {
    FastSort subsortp = subsortings[i * 2];
    FastSort supersortp = subsortings[i * 2 + 1];
    if (subsortp == subsort && isSubSortTransitive(supersortp, supersort)) {
      return true;
    }
  }
  return false;
}
