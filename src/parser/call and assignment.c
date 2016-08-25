#include "call and assignment.h"
#include "debug.h"


char get_call_or_assignment(Parser *parser)
{
    int         i;
    Array      *lvalue;
    Array      *rvalue;
    Data       *first_function;
    Data       *data;
    Assignment *assignment;
    char        expression_type;
    Variable   *variable;

    lvalue=get_compound_operand(parser);

    if(!lvalue)
        return 0;

    skip(parser);
/*
    for(i=0; i<lvalue->length; i++)
    {
        first_function=lvalue->data[i];

        if(first_function->type=='(')
            break;
    }
*/
    first_function=lvalue->data[lvalue->length-1];

    if(first_function->type=='(')
    {
        if(debug)
        {
            first_function=lvalue->data[lvalue->length-2];
            Variable *function_variable=first_function->data;
            print_block();
            printf("CALL ");
            str_print(function_variable->name);
        }

        array_push(parser->cur_body, new_data(lvalue, CALL));
    }
    else if(parser->head=='=')
    {
        if(debug)
        {
            first_function=lvalue->data[lvalue->length-1];
            Variable *function_variable=first_function->data;
            print_block();
            printf("ASSIGNMENT ");
            str_print(function_variable->name);
            printf(" =");
        }

        read_byte(parser);
        skip(parser);
        str_clear(parser->token);

        rvalue=parser_get_expression(parser);

        if(!rvalue)
            return 0;
/*
        expression_type=infer_type(parser, rvalue);

        if(expression_type==UNDEFINED)
            return 0;

        data=lvalue->data[0];
        variable=data->data;

        if(variable->type==UNDEFINED)
            variable->type=expression_type;
        else if(variable->type!=expression_type)
        {
            printf("\nтип переменной не может меняться. Он поменялся с ");
            print_infered_type(variable->type);
            printf(" на ");
            print_infered_type(expression_type);
            return 0;
        }*/

        assignment=new(Assignment);
        assignment->left_operand=lvalue;
        assignment->right_expression=rvalue;

        array_push(parser->cur_body, new_data(assignment, ASSIGNMENT));
    }

    if(debug)
        printf("\n");

    return 1;
}
