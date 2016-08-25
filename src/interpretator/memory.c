#include "memory.h"
#include "../extends.h"


Variable* interpretator_var_alloc(Interpretator *interpretator)
{
    Variable *alloc_var=interpretator->stack_head;
    interpretator->stack_head++;
    return alloc_var;
}


Variable* interpretator_pop_var(Interpretator *interpretator)
{
    interpretator->stack_head--;
    return interpretator->stack_head;
}


Variable* interpretator_heap_var_alloc(Interpretator *interpretator, Variable *variable)
{
    Variable *new_variable=new(Variable);

    new_variable->is_closed=variable->is_closed;
    new_variable->type=variable->type;
    new_variable->name=variable->name;
    new_variable->shift=variable->shift;

    return new_variable;
}


Variable* interpretator_add_var(Interpretator *interpretator, Variable *var)
{
    Variable *new_var=interpretator_var_alloc(interpretator);
    Variable *arg;

    new_var->name=var->name;

    switch(var->type)
    {
    case FUNCTION:
        new_var->type=FUNCTION;
        new_var->shift=var->shift;
        break;

    case UNDEFINED:
        ///////////////////////
        //new_var->type=UNDEFINED;
        //new_var->shift=0;
        //printf("\n:)");
        break;

    case ARGUMENT:
        arg=interpretator->stack_base[var->shift];

        new_var->name=var->name;
        new_var->type=arg->type;
        new_var->shift=arg->shift;
        break;

    case REAL:
        new_var->type=CONST_REAL;
        new_var->shift=interpretator->stack_base[var->shift];
        break;

    case CONST_REAL:
        new_var->type=CONST_REAL;
        new_var->shift=var->shift;
        break;

    case INTEGER:
        new_var->type=CONST_INTEGER;
        new_var->shift=interpretator->stack_base[var->shift];
        break;

    case CONST_INTEGER:
        new_var->type=CONST_INTEGER;
        new_var->shift=var->shift;
        break;

    case GC_INTEGER:
        new_var->type=CONST_INTEGER;
        new_var->shift=*(int*)var->shift;
        break;

    case STRING:
        new_var->type=STRING;
        new_var->shift=var->shift;
        break;

    case MAP:
        new_var->type=MAP;
        new_var->shift=var->shift;
        break;

    case ARRAY:
        new_var->type=ARRAY;
        new_var->shift=var->shift;
        break;

    case MODULE:
        break;

    default:
        printf("\nне определён тип в аллокаторе стека");
        break;
    }

    return new_var;
}


GCVariable* alloc_gc_variable(Variable *variable, Interpretator *interpretator)
{
    GCVariable *gc_variable;

    switch(variable->type)
    {
    case INTEGER:
        gc_variable=new(GCVariable);
        gc_variable->data=*((int*)interpretator->stack_base+variable->shift);
        break;

    case CONST_INTEGER:
        gc_variable=new(GCVariable);
        gc_variable->data=variable->shift;
        break;

    case GC_INTEGER:
        ((GCVariable*)variable->shift)->count++;
        return variable->shift;
        break;

    default:
        printf("\nне определён тип в аллокаторе кучи");
        break;
    }

    gc_variable->count=1;

    return gc_variable;
}


void free_gc_variable(GCVariable *gc_variable)
{
    gc_variable->count--;

    if(!gc_variable->count)
    {
        free(gc_variable->data);
        free(gc_variable);
    }
}
