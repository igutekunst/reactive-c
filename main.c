#include <stdio.h>
#include "node.h"

void add_one(ComputationNode * self) {
  self->value = getIntDep(self, 0) + 1;
  printf("Evalutated. adOne = %d\n", self->value);
}
int main(int argc, char * argv[]) {
  printf("OMG BITCHES\r\n");
  ComputationNode * x = Int(5);
  ComputationNode * c = create_computation("AddOne", add_one, x,  Int(7), End());
  int * v = (int *) evaluate(c);
  printf("Value: %d\n", *v);
  v = (int *) evaluate(c);
  printf("Value: %d\n", *v);

  x->value = 8;
  propagate(x);

  v = (int *) evaluate(c);
  printf("Value: %d\n", *v);
  print_tree(c, 0);

  free_computation_node(c);
}



