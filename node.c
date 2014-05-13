#include "node.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

void color_printf(const char * color, const char * format, ...) {
  va_list args;
  printf(color);
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf(ANSI_COLOR_RESET);
}

ComputationList * append_arg(ComputationList ** computationList, ComputationNode * computation) {
    ComputationList * newComputationList = (ComputationList *) malloc(sizeof(ComputationList));
    newComputationList->computation = computation;
    if (computationList) {
        newComputationList->next  = *computationList;
    } else {
        newComputationList->next  = NULL;
    }
    *computationList = newComputationList;
}


void print_pipes(int space){
  for(int i = 0; i < space; i++) {
        printf("  │") ;
  }
}


void print_space(int space, bool last){
  for(int i = 0; i < space; i++) {
    if (i == space - 1){
        if (last) {
          printf("  └") ;
        } else {
          printf("  ├") ;
        }
    } else {
        printf("  │") ;
    }
  }
  if (space) {
        printf("── ") ;
  }
}


int m_print_tree(ComputationNode * node, int depth, bool last) {
  int numArgs = node->numArgs;
  bool hasChildren = (bool) numArgs;

  print_space(depth, last );
  const char * color = node->valid ? GREEN : RED;
  color_printf(color, "%s ", node->name);
  printf("[%d]\n", node->value);

  if (!last) {
    printf("  ");
    print_pipes(depth + hasChildren);
    printf("\n");
  } else {
    printf("  ");
    print_pipes(depth - 1);
    printf("\n");
  }

  for (int i = 0; i < numArgs; i++){
    printf("  ");
    m_print_tree(node->args[i], depth + 1, i == numArgs - 1);
  }

}
void print_tree(ComputationNode * node) {
  m_print_tree(node, 0, false);
}


ComputationNode * v_create_computation(const char * name, ComputationCallback computation, va_list args ) {
    ComputationNode * computationNode = (ComputationNode *) malloc(sizeof(ComputationNode));
    
    computationNode->name = name;
    computationNode->computation = computation;
    computationNode->dependencies = NULL;

    ComputationNode * dependency;
    computationNode->numArgs = 0;

    do {
        dependency = va_arg (args, ComputationNode * );
        if ( dependency->type == END){
          break;
        }
        append_arg(&(computationNode->dependencies), dependency);
        ComputationList * dep = computationNode->dependencies;
        ComputationNode * comp = dep->computation;
        printf("Append arg of type %s\n", arg_strings[comp->type]);
        computationNode->numArgs++;
    } while (true);

    computationNode->args = (struct ComputationNode **) 
                             malloc(sizeof(ComputationNode) * computationNode->numArgs);         

    printf("There are %d args\n", computationNode->numArgs); 

    int i;
    ComputationList * currentDependency = computationNode->dependencies;
    int len = computationNode->numArgs;
    for(i = 0; i < len; i++) {
        computationNode->args[len - 1 - i] = (currentDependency->computation);
        currentDependency = currentDependency->next;
    }

    computationNode->valid = false;
    return computationNode;
}

ComputationNode * create_computation(const char * name, ComputationCallback computation, ... ) {
  va_list args;
  va_start(args, computation);
  ComputationNode * newComputation = v_create_computation(name, computation, args);
  va_end(args);
  return newComputation;

}


bool is_dependent(ComputationNode * self, ComputationNode * other) {

    ComputationList * currentDependent = self->dependents;
    while(currentDependent != NULL) {
        if(other == currentDependent->computation) {
            return true; 
        }
    }
    return false;
}


/**
 * Regiser other as a dependent of self
 */
void register_dependent(ComputationNode * self, ComputationNode * other) {
  if (! is_dependent(self, other)) {
    append_arg(&(self->dependents), other);
  }
}


void free_computation_node(ComputationNode * self) {
  if (self) {
    int numArgs = self->numArgs;
    for (int i = 0; i < numArgs; i++){
      free_computation_node(self->args[i]);

    }
    free(self);
  }
}


/*
 * Use a dependency
 *
 * Returns a pointer to the value of this type.
 *
 * So if the computation has a type INT, then 
 * the return value of this can be interpreted as an int *
 *
 * This function will transparently register oneself as a dependent
 * of this dependency. If the dependency is invalid, re-run it's computation
 */
void * dep(ComputationNode * self, int number) {
    ComputationNode * dep = self->args[number];
    
    // Register as dependent
    register_dependent(dep, self);
    // Return 
    if (dep->valid) {
       printf("Dep is valid \r\n") ;
    } else {
        evaluate(dep);
        // trigger evaluation 
    }

    return dep;
}


int getIntDep(ComputationNode * self, int number) {
    return *( (int *) dep(self, number));
}


/**
 * Triggers evaluation of a computation
 * If a computation is valid, does nothing
 *
 * If not, will loop over dependencies checking their 
 * validity, evaluating them if necessary.
 *
 * Evaluation will then trigger the computation and 
 * set itself to valid
 */
void * evaluate(ComputationNode * self) {
    if (self->valid){
        printf("Node is valid\n");
        return (void * ) &(self->value) ;
    }
    printf("Node is invalid. Evaluating\n");
    for(int i = 0; i < self->numArgs; i++) {
        printf("Evaluated dependency %d\n", i );
        ComputationNode * arg = self->args[i] ;
        if (! arg->valid) {
            evaluate(arg) ;
        }
    }
    self->computation(self);
    self->valid = true;
    return (void *)&(self->value);
}


/**
 * Invalidate a computation, and all 
 * it's dependents
 *
 */
void invalidate(ComputationNode * self) {
    printf("Invalidating %s \n", self->name);
    self->valid = false;
    ComputationList * currentDependent = self->dependents;
    while(currentDependent != NULL) {
        invalidate(currentDependent->computation) ;
        currentDependent = currentDependent->next;
    }
}


void propagate(ComputationNode * self) {
    printf("Propagating %s \n", self->name);
    self->valid = false;
    ComputationList * currentDependent = self->dependents;
    while(currentDependent != NULL) {
        propagate(currentDependent->computation) ;
        evaluate(currentDependent->computation);
        currentDependent = currentDependent->next;
    }
}


void nop(ComputationNode * self) {
}

/**
 * Dumb computation to represent an Int
 * Is more of a source than a computation
 */
ComputationNode * Int(int a){
    
    char * str = (char * ) malloc(50);
    snprintf(str, 60, "Int(%d)", a);
    ComputationNode * newComputation = create_computation(str, nop, End());
    newComputation->type = INT;
    newComputation->value = a;
    newComputation->valid = true;
    return newComputation;
}


void foldr(Int_Int_Int f, ComputationNode * self) { 
  if (self->numArgs == 0) {
    self->value = 0;
  } else {
    int start = getIntDep(self, 0);
    for(int i = 1; i < self->numArgs; i++) {
      start = f(start, getIntDep(self, i));
    }
    self->value = start;
  }

}


int sum(int a, int b) {
  return a + b;
}


void do_Sum(ComputationNode * self) {
  foldr(sum, self);
}


ComputationNode * FoldR(ComputationCallback f, ...) {
  va_list args;
  va_start(args, f);
  ComputationNode * newComputation = v_create_computation("Sum()", f, args);
  va_end(args);
  return newComputation;

}


ComputationNode * End(){
  ComputationNode * newComputation = (ComputationNode *) malloc(sizeof(ComputationNode));
  newComputation->name = "END";
  newComputation->valid = true;
  newComputation->type = END;
  return newComputation;
}
