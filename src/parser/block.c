#include "block.h"


void add_block(Parser *parser, char *new_block, char block_type, Array *block_body)
{
    Data *block_data=new_data(new_block, block_type);

    push(parser->blocks_pos, parser->cur_block);
    array_push(parser->cur_body, block_data);
    parser->cur_body=block_body;
    parser->cur_block=block_data;
}


void update_cur_body(Parser *parser)
{
    While    *while_data;
    If       *if_data;
    Function *function_data;
    Data     *cur_block=parser->cur_block;

    switch(cur_block->type)
    {
    case IF:
        if_data=cur_block->data;
        parser->cur_body=if_data->body;
        break;

    case WHILE:
        while_data=cur_block->data;
        parser->cur_body=while_data->body;
        break;

    case ELSE:
    case LOOP:
    case FUNCTION:
        function_data=cur_block->data;
        parser->cur_body=function_data->body;
        parser->cur_function=function_data;
        break;
    }
}
