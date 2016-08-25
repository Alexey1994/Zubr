#include "TypeInference.h"
#include "../extends.h"
#include "../types.h"
#include "../Map.h"


void print_infered_type(char type)
{
    switch(type)
    {
        case UNDEFINED:     printf("undefined");     break;
        case STRING:        printf("string");        break;
        case INTEGER:
        case GC_INTEGER:
        case CONST_INTEGER: printf("integer"); break;
        case REAL:
        case GC_REAL:
        case CONST_REAL:    printf("real");    break;

        default:            printf("NOT TYPE");
    }
}


static Variable* eval_operand(Array *arr)
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
            /*
        case 's':
            system_call=data->data;

            system_variable = interpretator->stack_head;

            system_variable->name  = interpretator_system_name;
            system_variable->type  = CONST_INTEGER;
            system_variable->shift = interpretator_system_call(system_call, interpretator);

            return system_variable;
            break;*/

        case 'v':
            cur_var=data->data;
            break;
/*
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

            cur_var=eval(interpretator, index->begin);

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
            break;*/
        }
    }

    return cur_var;
}


char infer_type(Parser *parser, Array *expression)
{
    char       type1;
    char       type2;
    Variable  *var;
    Data      *data;
    char     **array_data=expression->data;
    int        length=expression->length;
    int        i;

    array_clear(parser->stack_infered_types);

    for(i=0; i<length; i++)
    {
        data=array_data[i];

        switch(data->type)
        {
            case OPERAND:
                var=data->data;

                if(var->type==UNDEFINED)
                    return UNDEFINED;

                array_push(parser->stack_infered_types, var->type);
                break;

            case COMPOUND_OPERAND:
                var=eval_operand(data->data);

                if(!var)
                    return UNDEFINED;

                array_push(parser->stack_infered_types, var->type);

                break;

            case OPERATION:
                type1=array_pop(parser->stack_infered_types);
                type2=array_pop(parser->stack_infered_types);

                if(type1!=CONST_INTEGER && type1!=INTEGER && type1!=GC_INTEGER && type1!=CONST_REAL && type1!=REAL && type1!=GC_REAL)
                {
                    if(type1==ARGUMENT)
                    {

                    }
                    else
                    {
                        printf("попытка совершить арифметическую операцию с не числовыми данными");
                        return UNDEFINED;
                    }
                }

                if(type2!=CONST_INTEGER && type2!=INTEGER && type2!=GC_INTEGER && type2!=CONST_REAL && type2!=REAL && type2!=GC_REAL)
                {
                    if(type2==ARGUMENT)
                    {

                    }
                    else
                    {
                        printf("попытка совершить арифметическую операцию с не числовыми данными");
                        return UNDEFINED;
                    }
                }

                if(type1!=type2 && type1!=ARGUMENT && type2!=ARGUMENT)
                {
                    printf("ошибка: возможна потеря точности в арифметических выражениях");
                    return UNDEFINED;
                }
/*
                print_infered_type(type1);
                printf(" ");
                print_infered_type(type2);
                printf(" %c\n", data->data);
*/
                array_push(parser->stack_infered_types, type1);
                break;

            default:
                printf("\nошибка в выводе типа");
        }
    }

    return array_pop(parser->stack_infered_types);
}


Array* infer_function(Function *function, Array *arguments)
{

}
