#include "while.h"
#include "../debug.h"


char parser_while(Parser *parser)
{
    While *while_data;
    Do    *do_data;
    Array *condition;

    condition=parser_get_condition(parser);

    if(!condition)
        return 0;

    if(parser->cur_block->type==DO)
    {
        if(debug)
        {
            block_pos--;
            print_block();
            printf("WHILE ");
        }

        do_data=parser->cur_block->data;
        do_data->condition=condition;

        parser->cur_block=array_pop(parser->blocks_pos);
        update_cur_body(parser);
    }
    else
    {
        if(debug)
        {
            print_block();
            printf("WHILE ");
            block_pos++;
        }

        while_data=new(While);
        while_data->condition=condition;
        while_data->body=array_init(8);

        add_block(parser, while_data, WHILE, while_data->body);
    }

    if(debug)
        printf("\n");

    return 1;
}
