#include "else.h"
#include "../debug.h"


char parser_else(Parser *parser)
{
    If *if_data,
       *else_if_data,
       *cur_if;

    if(debug)
        printf("ELSE ");

    if(parser->cur_block->type!=IF)
    {
        printf("else находится не в if\n");
        return 0;
    }

    if_data=parser->cur_block->data;
    parser->cur_body=if_data->else_body;

    skip(parser);

    if(is_true_word(parser, "if"))
    {
        else_if_data=new(If);
        else_if_data->body      = array_init(8);
        else_if_data->else_body = array_init(8);
        else_if_data->condition = parser_get_expression(parser);

        parser->cur_block=new_data(else_if_data, IF);

        array_push(if_data->else_body, parser->cur_block);
        parser->cur_body=else_if_data->body;
    }
    else
        parser->cur_block=new_data(if_data->body, ELSE);

    printf("\n");

    return 1;
}
