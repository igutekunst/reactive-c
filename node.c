#include "node.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

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


void print_space(int space){
  for(int i = 0; i < space; i++) {
    printf(" ") ;
  }
}


int print_tree(ComputationNode * node, int depth) {
  int numArgs = node->numArgs;
  int numArgsBelow = 0;
  for (int i = 0; i < numArgs; i++){
    numArgsBelow += print_tree(node->args[i], depth + 1 );
  }

  for (int i = 0; i < numArgs; i++){
    print_space(numArgsBelow * 2 / (numArgs  +1) );
    printf("%s", node->args[i]->name);
  }
  printf("\n");
  if (depth == 0) {
    print_space((numArgsBelow * 2 )/ 2);
        
    printf("%s", node->name);
  }
}


ComputationNode * create_computation(const char * name, ComputationCallback computation, ... ) {
    ComputationNode * computationNode = (ComputationNode *) malloc(sizeof(ComputationNode));
    
    computationNode->name = name;
    computationNode->computation = computation;
    computationNode->dependencies = NULL;
    //
    va_list args;
    va_start(args, computation);
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

    computationNode->args = (struct ComputationNode **) malloc(sizeof(ComputationNode) * computationNode->numArgs);         
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
        return &(self->value) ;
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
    return &(self->value);
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
    
    ComputationNode * newComputation = create_computation("Int", nop, End());
    newComputation->type = INT;
    newComputation->value = a;
    newComputation->valid = true;
    return newComputation;
}


ComputationNode * End(){
    ComputationNode * newComputation = (ComputationNode *) malloc(sizeof(ComputationNode));
    newComputation->name = "END";
    newComputation->valid = true;
    newComputation->type = END;
    return newComputation;
}
