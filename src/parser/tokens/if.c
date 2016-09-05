#include "if.h"
#include "../expression.h"
#include "../debug.h"


char parser_if(Parser *parser)
{
    Array *condition;
    If    *if_data;

    if(debug)
    {
        print_block();
        printf("IF ");
    }

    condition=parser_get_condition(parser);

    if(!condition)
        return 0;

    if_data            = new(If);
    if_data->condition = condition;
    if_data->body      = array_init(8);
    if_data->else_body = array_init(8);

    add_block(parser, if_data, IF, if_data->body);

    if(debug)
    {
        printf("\n");
        block_pos++;
    }

    return 1;
}
