//1.Ok
#include "print.h"
#include "../expression.h"
#include "../debug.h"


static char get_print_expression(Parser *parser)
{
    Array *expression;

    if(debug)
    {
        print_block();
        printf("PRINT ");
    }

    expression=parser_get_expression(parser);

    if(!expression)
        return 0;
/*
    if(infer_type(parser, expression)==UNDEFINED)
        return 0;
*/
    array_push(parser->cur_body, new_data(expression, PRINT));

    if(debug)
        printf("\n");

    return 1;
}

char parser_print(Parser *parser)
{
    skip(parser);

    if(!get_print_expression(parser))
        return 0;

    while(parser->head==',')
    {
        read_byte(parser);

        if(!get_print_expression(parser))
            return 0;
    }

    return 1;
}
