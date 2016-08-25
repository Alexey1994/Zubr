#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#include "interpretator.h"

typedef struct
{
    unsigned int  count;
    char         *data;
}
GCVariable;


Variable* interpretator_var_alloc(Interpretator *interpretator);
Variable* interpretator_pop_var(Interpretator *interpretator);
Variable* interpretator_heap_var_alloc(Interpretator *interpretator, Variable *variable);
Variable* interpretator_add_var(Interpretator *interpretator, Variable *var);
GCVariable* alloc_gc_variable(Variable *variable, Interpretator *interpretator);
void free_gc_variable(GCVariable *gc_variable);

#endif // MEMORY_H_INCLUDED
