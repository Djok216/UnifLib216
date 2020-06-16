# How to use it?
Just add ``#include "UnifLib216.h"``to your code.

# Example
Below is an example for the equation ``f(y1, x5, g(x4)) =? f(y2, g(x2))``
```C++
// solve AC-unify: f(y1, x5, g(x4)) =? f(y2, g(x2))
// Unifications: 4
// {y2 |-> f(_13, _14, g(x4)), y1 |-> _13, x5 |-> f(_14, g(x2))}
// {y2 |-> f(_13, g(x4)), y1 |-> _13, x5 |-> g(x2)}
// {y2 |-> f(_13, _14, g(x4)), y1 |-> f(_13, g(x2)), x5 |-> _14}
// {y2 |-> f(_14, g(x4)), y1 |-> g(x2), x5 |-> _14}
void example() {
  FastVar x2 = newVar("x2", fastStateSort());
  FastVar x4 = newVar("x4", fastStateSort());
  FastVar x5 = newVar("x5", fastStateSort());
  FastVar y1 = newVar("y1", fastStateSort());
  FastVar y2 = newVar("y2", fastStateSort());
  FastSort sorts[2] = {fastStateSort(), fastStateSort()};
  FastFunc g = newFunc("g", fastStateSort(), 1, sorts);
  FastFunc f = newACFunc("f", fastStateSort());
  FastTerm args[2] = {x4, x2};
  FastTerm g4 = newFuncTerm(g, args);
  FastTerm g2 = newFuncTerm(g, args + 1);
  args[0] = y1; args[1] = x5;
  FastTerm t1 = newFuncTerm(f, args);
  args[0] = t1; args[1] = g4;
  t1 = newFuncTerm(f, args);
  args[0] = y2; args[1] = g2;
  FastTerm t2 = newFuncTerm(f, args);

  std::cout << "AC-Unify: " << toString(t1) << ' ' << toString(t2) << '\n';
  FastQueryACUnify solver(t1, t2);
  auto ans = solver.solve();
  std::cout << "Number of unifiers: " << ans.size() << '\n';
  for (auto& subst : ans) {
    std::cout << toString(subst) << '\n';
  }
}
```