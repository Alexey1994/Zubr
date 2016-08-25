#include "EvalExpression.h"
#include "EvalExpressionOperations.h"
#include "../extends.h"
#include "../Map.h"

extern String *interpretator_system_name;
static char* (*op[256])(Interpretator *interpretator);


void interpretator_operation_table_init()
{
    op['+']=add;
    op['-']=sub;
    op['*']=mul;
    op['/']=division;

    op['=']=equal;
    op['!']=not_equal;

    op['<']=lesser;
    op['1']=lesser_or_equal;

    op['>']=greather;
    op['2']=greather_or_equal;

    op['a']=and;
}


Variable* eval_operand(Array *arr, Interpretator *interpretator)
{
    Data       *data;

    Variable   *prev_var,
               *cur_var;
    int         i;
    Array      *index;

    Array      *array;
    Map        *map;
    Array      *expression;
    SystemCall *system_call;
    Function   *function;
    Variable   *system_variable;

    for(i=0; i<arr->length; i++)
    {
        data=arr->data[i];
        switch(data->type)
        {
        case 's':
            system_call=data->data;

            system_variable = interpretator->stack_head;

            system_variable->name  = interpretator_system_name;
            system_variable->type  = CONST_INTEGER;
            system_variable->shift = interpretator_system_call(system_call, interpretator);

            return system_variable;
            break;

        case 'v':
            cur_var=data->data;
            break;

        case '[':
            index=data->data;

            if(cur_var->type!=ARRAY)
            {
                printf("\nvariable '");
                str_print(cur_var->name);
                printf("' is not array");

                return undefined;
            }

            array=cur_var->shift;

            cur_var=eval(interpretator, index);

            switch(cur_var->type)
            {
            case CONST_INTEGER:
                if(cur_var->shift>=array->length)
                {
                    printf("\nвыход за границы массива");
                    return cur_var;
                }

                cur_var=array->data[cur_var->shift];
                break;
            }
            break;

        case '(':
            if(cur_var->type!=FUNCTION)
            {
                printf("\nvariable ");
                str_print(cur_var->name);
                printf(" is not function");

                return 0;
            }

            cur_var=run_function(interpretator, cur_var->shift, data->data);
            break;

        case '.':
            if(cur_var->type!=MAP)
            {
                if(cur_var->type!=MODULE)
                {
                    printf("\nеременная ");
                    str_print(cur_var->name);
                    printf(" не является ни множеством ни классом");

                    return 0;
                }

                function=cur_var->shift;
                cur_var=tree_find(function->variables, data->data);

                break;
            }

            map=cur_var->shift;

            cur_var=map_get(map, data->data);
            if(!cur_var)
            {
                printf("\nkey ");
                str_print(data->data);
                printf(" not found");

                return undefined;
            }
            break;
        }
    }

    return cur_var;
}


Variable* eval(Interpretator *interpretator, Array *expression)
{
    Variable *rez,
             *var,
             *op1,
             *op2;
    Data     *data;
    int      *op1_data,
             *op2_data;
    char    **array_data=expression->data;
    int       length=expression->length;
    int       i;

    for(i=0; i<length; i++)
    {
        data=array_data[i];

        switch(data->type)
        {
            case OPERAND:
                interpretator_add_var(interpretator, data->data);
                break;

            case COMPOUND_OPERAND:
                var=eval_operand(data->data, interpretator);

                if(!var)
                    return undefined;

                interpretator_add_var(interpretator, var);
                break;

            case OPERATION:
                op[(char)data->data](interpretator);
                break;

            default:
                printf("\nerror in eval");
        }
    }

    //return interpretator_pop_var(interpretator);
    return interpretator->stack_head-1;
}
