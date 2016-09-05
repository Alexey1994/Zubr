#include "loop.h"
#include "../debug.h"


char parser_loop(Parser *parser)
{
    Array *loop_body=array_init(8);

    if(debug)
    {
        print_block();
        printf("LOOP\n");
        block_pos++;
    }

    add_block(parser, loop_body, LOOP, loop_body);

    return 1;
}
