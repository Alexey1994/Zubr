#include "out body.h"
#include "types.h"


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


static void print_var(Translator *translator, Variable *var)
{
    char buf[10];

    switch(var->type)
    {
    case CONST_INTEGER:
        snprintf(buf, 10, "%d", var->shift);
        out_token_translator_parser(buf, translator);
        //printf("%d", var->shift);
        break;

    case STRING:
        {
        String *s=var->shift;
        out_token_translator_parser("'", translator);
        out_token_translator_parser(s->begin, translator);
        out_token_translator_parser("'", translator);
        //printf("%s", var->shift);
        break;
        }

    default:
        printf("error");
    }
}


static void print_operand(Translator *translator, Data *operand)
{
    BinaryOperator *binary_operator;
    char            buf[10];

    switch(operand->type)
    {
    case OPERAND:
        print_var(translator, operand->data);
        break;

    case COMPOUND_OPERAND:
        print_var(translator, eval_operand(operand->data));
        break;

    case EVALED_OPERAND:
        binary_operator=operand->data;

        if(binary_operator->is_in_brace)
            out_token_translator_parser("(", translator);
            //printf("(");

        print_operand(translator, binary_operator->operand1);

        snprintf(buf, 10, "%c", binary_operator->operation);
        out_token_translator_parser(buf, translator);
        //printf("%c", binary_operator->operation);

        print_operand(translator, binary_operator->operand2);

        if(binary_operator->is_in_brace)
            out_token_translator_parser(")", translator);
            //printf(")");
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


void write_translator_expression(Translator *translator, Array *expression)
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
                //printf(" op");
                break;

            case OPERATION:
                //printf(" o");
                //printf("{%d}", stack->length);
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

    print_operand(translator, array_pop(stack));
}


void write_translator_body(Translator *translator, Array *body)
{
    int i;
    Data *body_data;

    for(i=0; i<body->length; i++)
    {
        body_data=body->data[i];

        switch(body_data->type)
        {
            case TRANSLATOR_OUT: out_token_translator_parser(body_data->data, translator); break;
            case TRANSLATOR_EXPRESSION: write_translator_expression(translator, translator->current_expression); break;
        }
    }
}
