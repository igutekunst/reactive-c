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

struct ComputationList {
    ComputationNode * computation;
    ComputationList * next;
};

typedef void (* ComputationCallback) (ComputationNode * );

typedef int (* Int_Int_Int) (int a, int b);
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
ComputationNode * v_create_computation( const char * name, ComputationCallback, va_list);
void * dep(ComputationNode * self, int number);
int getIntDep(ComputationNode * self, int number);
void * evaluate(ComputationNode * self);
void invalidate(ComputationNode * self);
void propagate(ComputationNode * self);
void print_tree(ComputationNode * self);
void free_computation_node(ComputationNode * self);
ComputationNode * Int(int a);
ComputationNode * End();
ComputationNode * FoldR(ComputationCallback f, ...);
void do_Sum(ComputationNode * self);
int sum(int, int);

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


void color_printf(const char * color, const char * format, ...);

#endif
