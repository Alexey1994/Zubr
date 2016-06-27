#include "interpretator.h"
#include "../lexer/lexer_operations.h"
#include "interpretator_operations.h"

Variable     *undefined;
static char (*interpretator_table[256])(char *data, Interpretator *interpretator);


void interpretator_table_init()
{
    interpretator_table[PRINT]      = interpretator_print;
    interpretator_table[IF]         = interpretator_if;
    interpretator_table[LOOP]       = interpretator_loop;
    interpretator_table[WHILE]      = interpretator_while;
    interpretator_table[DO]         = interpretator_do;
    interpretator_table[BREAK]      = interpretator_break;
    interpretator_table[CONTINUE]   = interpretator_continue;
    interpretator_table[CALL]       = interpretator_call;
    interpretator_table[ASSIGNMENT] = interpretator_assignment;
    interpretator_table[RETURN]     = interpretator_return;
    interpretator_table[PUSH]       = interpretator_push;
}


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


int u=0;

char execute(Interpretator *interpretator, List *body)
{
    struct ListNode *i=body->begin;
    Data  *data;
    char   loop_state;

    if(u==10000000)
    {
        printf("%d\n", u);
        u=0;
    }
    u++;

    while(i)
    {
        data=i->data;
        loop_state=interpretator_table[data->type](data->data, interpretator);

        i=i->next;

        if(loop_state)
        {
            switch(loop_state)
            {
                case LOOP_BREAK    : return loop_state; break;
                case LOOP_CONTINUE : i=body->begin; break;
                case RETURN        : return RETURN; break;
            }
        }
    }

    return LOOP_NORMAL;
}


static void push_args(Interpretator *interpretator, Array *args, Array *function_args, int *new_base)
{
    int       i;
    Variable *function_arg,
             *current_arg;
    List     *arg_expression;

    for(i=0; i<function_args->length; i++)
    {
        if(i<args->length)
        {
            arg_expression=args->data[i];
            current_arg=eval(interpretator, arg_expression->begin);

            function_arg=function_args->data[i];
            new_base[function_arg->shift]=current_arg;
        }
        else
        {
            function_arg=function_args->data[i];
            new_base[function_arg->shift]=undefined;
        }
    }
}


static void to_const(Variable *var, Interpretator *interpretator)
{
    switch(var->type)
    {
    case INTEGER:
        var->type=CONST_INTEGER;
        var->shift=interpretator->stack_base[var->shift];
        break;
    }
}


Variable* run_function(Interpretator *interpretator, Function *f, Array *args)
{
    Variable *return_var=undefined;
    int       i;
    int      *current_base;

    if(f->return_var)
    {
        return_var=interpretator->stack_head;
        return_var->type=UNDEFINED;
        return_var->shift=-1;
        return_var->name=f->return_var->name;
        return_var->is_closed=f->return_var->is_closed;
        interpretator->stack_head++;

        interpretator->stack_head=(int*)interpretator->stack_head+1;
    }

    current_base=interpretator->stack_head;
    *current_base=interpretator->stack_base;

    interpretator->stack_head=(int*)interpretator->stack_head+f->variables->length+1; //+base

    if(args)
        push_args(interpretator, args, f->args, current_base);

    interpretator->stack_base=current_base;

    if(f->return_var)
        current_base[f->return_var->shift]=return_var;//инициализация return var

    execute(interpretator, f->body);

    if(f->return_var)
        to_const(return_var, interpretator);

    interpretator->stack_head=(int*)interpretator->stack_head-f->variables->length-1;
    interpretator->stack_head-=f->args->length;
    interpretator->stack_base=*interpretator->stack_base;

    if(f->return_var)
    {
        printf("\n<function ");
        str_print(f->name);
        printf(" return ");
        str_print(f->return_var->name);
        printf(">\n");
    }

    if(f->return_var)
    {
        interpretator->stack_head=(int*)interpretator->stack_head-1;
        interpretator->stack_head--;
        return return_var;
    }
    return undefined;
}


static int tree_str_function_cmp(String *s, Function *f)
{
    return str_comparision(s, f->name);
}


void interpretator(Function *main)
{
    undefined=new(Variable);
    undefined->name=str_init("undefined");
    undefined->type=UNDEFINED;

    Interpretator *interpretator=new(Interpretator);

    interpretator->stack=malloc(10000);
    interpretator->stack_head=interpretator->stack;

    run_function(interpretator, main, 0);
}
