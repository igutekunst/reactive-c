#ifndef CIRCUIT_NODE_H
#define CIRCUIT_NODE_H
#include <stdbool.h>
#include <stdarg.h>

typedef enum {
    CHAR,
    INT,
    CHAR_PTR,
    END
} Arg_t;



static const char * arg_strings[] = {
    "char",
    "int",
    "char*",
    "END"
};



typedef struct ComputationNode ComputationNode;
typedef struct ComputationList ComputationList;

struct ComputationList{
    ComputationNode * computation;
    ComputationList * next;
};
typedef void (* ComputationCallback) (ComputationNode * );
struct ComputationNode {
    int value;
    const char * name;
    void * data;
    bool valid;
    int numArgs;
    Arg_t type;
    ComputationNode ** args;
    ComputationList * dependencies;
    ComputationList * dependents;
    void  (* computation) (ComputationNode * self);
};


ComputationNode * create_computation( const char * name, ComputationCallback, ...);
void * dep(ComputationNode * self, int number);
int getIntDep(ComputationNode * self, int number);
void * evaluate(ComputationNode * self);
void invalidate(ComputationNode * self);
void propagate(ComputationNode * self);
int print_tree(ComputationNode * self, int depth);
void free_computation_node(ComputationNode * self);
ComputationNode * Int(int a);
ComputationNode * End();

#endif
