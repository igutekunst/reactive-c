#include <stdio.h>
#include "node.h"

void add_one(ComputationNode * self) {
  self->value = getIntDep(self, 0) + 1;
  printf("Evalutated. addOne = %d\n", self->value);
}


int main(int argc, char * argv[]) {
  ComputationNode * x = Int(5);
  ComputationNode * y = Int(2);
  ComputationNode * s = FoldR(do_Sum, Int(5), x, End());
  ComputationNode * s1 = FoldR(do_Sum, s, Int(5), y, End());
  printf("\n\nComputation Graph\n"
             "=================\n\n");
  evaluate(s1);
  print_tree(s1);
    
  x->value = 7;
  invalidate(x);
  print_tree(s1);
  evaluate(s1);

  print_tree(s1);
  free_computation_node(s);
}



