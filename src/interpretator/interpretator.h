#ifndef INTERPRETATOR_H_INCLUDED
#define INTERPRETATOR_H_INCLUDED

#include "../tree.h"
#include "../Array.h"
#include "../types.h"

typedef struct
{
    Variable *stack;
    Variable *stack_head;
    int      *stack_base;
}
Interpretator;

Variable* run_function(Interpretator *interpretator, Function *f, Array *args);
char execute(Interpretator *interpretator, Array *body);
void interpretator(Function *main);

void init_interpretator();

#endif // INTERPRETATOR_H_INCLUDED
