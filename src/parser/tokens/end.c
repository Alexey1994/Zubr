#include "end.h"
#include "../block.h"
#include "../debug.h"


char parser_end(Parser *parser)
{
    if(!parser->blocks_pos->begin)
    {
        printf("end не принадлежит ни одному блоку\n");
        return 0;
    }

    if(debug)
        block_pos--;

    switch(parser->cur_block->type)
    {
        case FUNCTION:
            parser->cur_scope=pop(parser->scopes);
            parser->is_end_function=1;
            break;

        case ELSE:
            free(parser->cur_block);
            break;

        case DO:
            printf("do без while\n");
            return 0;
            break;
    }

    parser->cur_block=pop(parser->blocks_pos);
    update_cur_body(parser);

    return 1;
}
