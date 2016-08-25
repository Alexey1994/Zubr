#include "push.h"


static char new_push(Parser *parser, Array *array_operand)
{
    Push *push_data;
    Array *expression;

    expression=parser_get_expression(parser);

    if(!expression)
        return 0;

    push_data=new(Push);
    push_data->expression = expression;
    push_data->array      = array_operand;

    array_push(parser->cur_body, new_data(push_data, PUSH));

    return 1;
}

char parser_push(Parser *parser)
{
    Array *array_operand;

    str_clear(parser->token);
    skip(parser);
    get_token(parser, parser->token);
    array_operand=get_compound_operand(parser);

    if(!array_operand)
        return 0;

    if(!new_push(parser, array_operand))
        return 0;

    while(parser->head==',')
    {
        read_byte(parser);

        if(!new_push(parser, array_operand))
            return 0;
    }

    return 1;
}
