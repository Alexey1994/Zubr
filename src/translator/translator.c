#include "translator.h"
#include "../extends.h"
#include "../types.h"
#include "../Map.h"


extern int priorities[128];


typedef struct
{
    Data *operand1;
    char  operation;
    Data *operand2;
    char  is_in_brace;
}
BinaryOperator;


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


static void print_var(Variable *var)
{
    switch(var->type)
    {
    case CONST_INTEGER:
        printf("%d", var->shift);
        break;

    case STRING:
        printf("%s", var->shift);
        break;

    default:
        printf("error");
    }
}


static void print_operand(Data *operand)
{
    BinaryOperator *binary_operator;

    switch(operand->type)
    {
    case OPERAND:
        print_var(operand->data);
        break;

    case COMPOUND_OPERAND:
        print_var(eval_operand(operand->data));
        break;

    case EVALED_OPERAND:
        binary_operator=operand->data;

        if(binary_operator->is_in_brace)
            printf("(");

        print_operand(binary_operator->operand1);
        printf("%c", binary_operator->operation);
        print_operand(binary_operator->operand2);

        if(binary_operator->is_in_brace)
            printf(")");
        break;
    }
}


static void update_braces(Data *operand, unsigned char operation)
{
    if(operand->type!=EVALED_OPERAND)
        return;

    BinaryOperator *binary_operator=operand->data;

    if(priorities[(int)operation] > priorities[(int)binary_operator->operation])
        binary_operator->is_in_brace=1;
}


void convert_from_postfix_to_infix_notation(Array *expression)
{
    Variable       *var;
    Data           *data;
    char          **array_data=expression->data;
    int             length=expression->length;
    int             i;
    Array          *stack=array_init(10);
    Data           *operand1;
    Data           *operand2;
    BinaryOperator *binary_operator;

    for(i=0; i<length; i++)
    {
        data=array_data[i];

        switch(data->type)
        {
            case OPERAND:
            case COMPOUND_OPERAND:
                array_push(stack, data);
                break;

            case OPERATION:
                binary_operator=new(BinaryOperator);
                binary_operator->operand2    = array_pop(stack);
                binary_operator->operand1    = array_pop(stack);
                binary_operator->operation   = data->data;
                binary_operator->is_in_brace = 0;

                update_braces(binary_operator->operand1, binary_operator->operation);
                update_braces(binary_operator->operand2, binary_operator->operation);

                array_push(stack, new_data(binary_operator, EVALED_OPERAND));
                break;

            default:
                printf("\nошибка в преобразователе из постфиксной в инфиксную нотацию");
        }
    }

    print_operand(array_pop(stack));
}
